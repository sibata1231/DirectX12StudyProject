#include "stdafx.h"
#include "system/system.h"
#include <random>
#include "util/stopwatch.h"

/////////////////////////////////////////////////////////////////
// �萔
/////////////////////////////////////////////////////////////////
const int TILE_WIDTH = 16;      // �^�C���̕�
const int TILE_HEIGHT = 16;     // �^�C���̍���
const int NUM_TILE = (FRAME_BUFFER_W / TILE_WIDTH) * (FRAME_BUFFER_H / TILE_HEIGHT); // �^�C���̐�

/////////////////////////////////////////////////////////////////
// �\����
/////////////////////////////////////////////////////////////////
// �f�B���N�V�������C�g�\����
struct alignas(16) DirectionalLight
{
    Vector3  color;     // ���C�g�̃J���[
    float pad0;
    Vector3  direction; // ���C�g�̕���
};

// �|�C���g���C�g�\����
struct alignas(16) PointLight
{
    Vector3 position;       // ���W
    float pad0;
    Vector3 positionInView; // �J������Ԃł̍��W
    float pad1;
    Vector3 color;          // ���C�g�̃J���[
    float range;            // ���C�g�̉e����^����͈�
};

const int MAX_POINT_LIGHT = 1000;   // �|�C���g���C�g�̍ő吔
const int NUM_DIRECTION_LIGHT = 4;  // �f�B���N�V�������C�g�̐�

// ���C�g�\����
struct Light
{
    DirectionalLight directionLights[ NUM_DIRECTION_LIGHT]; // �f�B���N�V�������C�g
    PointLight pointLights[MAX_POINT_LIGHT];                // �|�C���g���C�g
    Matrix mViewProjInv;                                    // �r���[�v���W�F�N�V�����s��̋t�s��
    Vector4 screenParam;                                    // �X�N���[�����
    Vector3 eyePos;                                         // ���_
    float specPow;                                          // �X�y�L�����̍i��
    int numPointLight;                                      // �|�C���g���C�g�̐�
};

/////////////////////////////////////////////////////////////////
/// �N���X
/////////////////////////////////////////////////////////////////
// ���C�g�J�����O�N���X
class LightCulling
{
private:
    // ���C�g�J�����O�Ŏg�p����J�������
    struct alignas(16) LightCullingCameraData
    {
        Matrix mProj;           // �v���W�F�N�V�����s��
        Matrix mProjInv;        // �v���W�F�N�V�����s��̋t�s��
        Matrix mCameraRot;      // �J�����̉�]�s��
    };

    RootSignature* m_rootSignature = nullptr;   // ���[�g�V�O�l�`��
    Shader m_cs;                                // �R���s���[�g�V�F�[�_�[
    PipelineState m_pipelineState;              // �p�C�v���C���X�e�[�g
    RWStructuredBuffer m_pointLightNoListInTileUAV; // �^�C�����Ƃ̃|�C���g���C�g�̔ԍ��̃��X�g���o�͂���UAV��������
    LightCullingCameraData m_cameraDataCPU;     // ���C�g�J�����O�̂��߂̃J�������𑗂�萔�o�b�t�@
    ConstantBuffer m_cameraDataGPU;             // ���C�g�J�����O�p�̃J�����f�[�^
    ConstantBuffer m_lightGPU;                  //
    DescriptorHeap m_descriptorHeap;            // �f�B�X�N���v�^�q�[�v
    Light* m_light = nullptr;

public:
    RWStructuredBuffer& GetPointLightNoListInTileUAV()
    {
        return m_pointLightNoListInTileUAV;
    }

    /// <summary>
    /// ������
    /// </summary>
    void Init(RootSignature& rs, Light& light, RenderTarget& depthRT)
    {
        m_light = &light;
        m_rootSignature = &rs;
        // ���C�g�J�����O�p�̃R���s���[�g�V�F�[�_�[�����[�h
        m_cs.LoadCS("Assets/shader/lightCulling.fx", "CSMain");

        // �p�C�v���C���X�e�[�g���쐬
        D3D12_COMPUTE_PIPELINE_STATE_DESC  psoDesc = { 0 };
        psoDesc.pRootSignature = rs.Get();
        psoDesc.CS = CD3DX12_SHADER_BYTECODE(m_cs.GetCompiledBlob());
        psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
        psoDesc.NodeMask = 0;

        m_pipelineState.Init(psoDesc);

        // �^�C�����Ƃ̃|�C���g���C�g�̔ԍ��̃��X�g���o�͂���UAV��������
        m_pointLightNoListInTileUAV.Init(
            sizeof(int),
            MAX_POINT_LIGHT * NUM_TILE,
            nullptr
        );

        // �|�C���g���C�g�̏��𑗂邽�߂̒萔�o�b�t�@���쐬
        // ���C�g�J�����O�̃J�����p�̒萔�o�b�t�@���쐬
        m_cameraDataCPU.mProj = g_camera3D->GetProjectionMatrix();
        m_cameraDataCPU.mProjInv.Inverse(g_camera3D->GetProjectionMatrix());
        m_cameraDataCPU.mCameraRot = g_camera3D->GetCameraRotation();

        m_cameraDataGPU.Init(sizeof(m_cameraDataCPU), &m_cameraDataCPU);

        // ���C�g�J�����O�̃��C�g�p�̒萔�o�b�t�@���쐬
        m_lightGPU.Init(sizeof(light), &light);

        // ���C�g�J�����O�p�̃f�B�X�N���v�^�q�[�v���쐬
        m_descriptorHeap.RegistShaderResource(0, depthRT.GetRenderTargetTexture());
        m_descriptorHeap.RegistUnorderAccessResource(0, m_pointLightNoListInTileUAV);
        m_descriptorHeap.RegistConstantBuffer(0, m_cameraDataGPU);
        m_descriptorHeap.RegistConstantBuffer(1, m_lightGPU);
        m_descriptorHeap.Commit();
    }

    /// <summary>
    /// �f�B�X�p�b�`
    /// </summary>
    /// <param name="renderContext"></param>
    void Dispatch(RenderContext& renderContext)
    {
        // ���C�g�J�����O���f�B�X�p�b�`
        renderContext.SetComputeRootSignature(*m_rootSignature);
        m_lightGPU.CopyToVRAM(*m_light);
        renderContext.SetComputeDescriptorHeap(m_descriptorHeap);
        renderContext.SetPipelineState(m_pipelineState);

        // �O���[�v�̐��̓^�C���̐�
        renderContext.Dispatch(
            FRAME_BUFFER_W / TILE_WIDTH,
            FRAME_BUFFER_H / TILE_HEIGHT,
            1
        );
    }
};

// step-1 ZPrepass�N���X���쐬
class ZPrepass
{
private:
    RenderTarget m_depthRT; // �[�x�l���������ރ����_�����O�^�[�Q�b�g
    Model m_teapotModel;    // �e�B�[�|�b�g�̃��f��
    Model m_bgModel;        // �w�i���f��

public:
    RenderTarget& GetDepthRenderTarget()
    {
        return m_depthRT;
    }

    void Init()
    {
        // �[�x�l���������ރ����_�����O�^�[�Q�b�g���쐬
        m_depthRT.Create(
            FRAME_BUFFER_W,
            FRAME_BUFFER_H,
            1,
            1,
            DXGI_FORMAT_R32_FLOAT,
            DXGI_FORMAT_D32_FLOAT
        );

        // ���f����������
        ModelInitData teapotModelInitData;
        teapotModelInitData.m_tkmFilePath = "Assets/modelData/teapot.tkm";

        // �V�F�[�_�[��ZPrepass�p�ɂ���
        teapotModelInitData.m_fxFilePath = "Assets/shader/zprepass.fx";
        // �o�͐�̃J���[�o�b�t�@�̃t�H�[�}�b�g���w�肷��B
        teapotModelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32_FLOAT;
        m_teapotModel.Init(teapotModelInitData);

        // �w�i�̃��f����������
        ModelInitData bgModelInitData;

        // ���[�U�[�g���f�[�^�Ƃ��ă|�C���g���C�g�̃��X�g��n��
        bgModelInitData.m_tkmFilePath = "Assets/modelData/bg.tkm";

        // �V�F�[�_�[��ZPrepass�p�ɂ���
        bgModelInitData.m_fxFilePath = "Assets/shader/zprepass.fx";
        // �o�͐�̃J���[�o�b�t�@�̃t�H�[�}�b�g���w�肷��B
        bgModelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32_FLOAT;
        m_bgModel.Init(bgModelInitData);
    }

    void Draw(RenderContext& renderContext)
    {
        // �����_�����O�^�[�Q�b�g��؂�ւ��ăh���[
        RenderTarget* rts[] = {
            &m_depthRT
        };
        renderContext.WaitUntilToPossibleSetRenderTargets(1, rts);

        // �����_�����O�^�[�Q�b�g��ݒ�
        renderContext.SetRenderTargets(1, rts);

        // �����_�����O�^�[�Q�b�g���N���A
        renderContext.ClearRenderTargetViews(1, rts);

        m_teapotModel.Draw(renderContext);
        m_bgModel.Draw(renderContext);

        // �����_�����O�^�[�Q�b�g�ւ̏������ݑ҂�
        renderContext.WaitUntilFinishDrawingToRenderTargets(1, rts);

        // �����_�����O����t���[���o�b�t�@�ɖ߂�
        g_graphicsEngine->ChangeRenderTargetToFrameBuffer(renderContext);
    }
};

/////////////////////////////////////////////////////////////////
//�֐��錾
/////////////////////////////////////////////////////////////////
void InitRootSignature(RootSignature& rs);
void InitStandard�h�nConsole();
void InitLight(Light& light);
void InitModel(Model& teapotModel, Model& bgModel, Light& light, IShaderResource& pointLightNoListInTileUAV);

///////////////////////////////////////////////////////////////////
// �E�B���h�E�v���O�����̃��C���֐�
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    // �Q�[���̏�����
    InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));

    // �W�����o�̓R���\�[���̏�����
    InitStandard�h�nConsole();

    //////////////////////////////////////
    // �������珉�������s���R�[�h���L�q����
    //////////////////////////////////////
    g_camera3D->SetPosition({ 0.0f, 200.0, 400.0f });
    g_camera3D->Update();

    // ���[�g�V�O�l�`�����쐬
    RootSignature rootSignature;
    InitRootSignature(rootSignature);

    // ���C�g��������
    Light light;
    InitLight(light);

    // step-2 ZPrepass�N���X�̃I�u�W�F�N�g���쐬���ď���������
    ZPrepass zprepass;
    zprepass.Init();

    // ���C�g�J�����O�̏�����
    LightCulling lightCulling;
    lightCulling.Init(rootSignature, light, zprepass.GetDepthRenderTarget());

    // �e�B�[�|�b�g�̃��f���Ɣw�i���f����������
    Model teapotModel, bgModel;
    InitModel(teapotModel, bgModel, light, lightCulling.GetPointLightNoListInTileUAV());

    //////////////////////////////////////
    // ���������s���R�[�h�������̂͂����܂ŁI�I�I
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    Stopwatch sw;
    //  ��������Q�[�����[�v
    while (DispatchWindowMessage())
    {
        sw.Start();
        // �����_�����O�J�n
        g_engine->BeginFrame();
        //////////////////////////////////////
        // ��������G��`���R�[�h���L�q����
        //////////////////////////////////////

        // ���C�g����
        Quaternion qRot;
        qRot.SetRotationDegY(1.0f);
        Matrix mView = g_camera3D->GetViewMatrix();
        for (int i = 0; i < light.numPointLight; i++)
        {
            auto& pt = light.pointLights[i];
            qRot.Apply(pt.position);
            pt.positionInView = pt.position;
            mView.Apply(pt.positionInView);
        }

        // step-3 ZPrepass�˃��C�g�J�����O�˃t�H���[�h�����_�����O�̎��s
        // ZPrepass���s
        zprepass.Draw(renderContext);

        // ���C�g�J�����O���f�B�X�p�b�`
        lightCulling.Dispatch(renderContext);

        // �t�H���[�h�����_�����O
        teapotModel.Draw(renderContext);
        bgModel.Draw(renderContext);

        /////////////////////////////////////////
        // �G��`���R�[�h�������̂͂����܂ŁI�I�I
        //////////////////////////////////////
        // �����_�����O�I��
        g_engine->EndFrame();
        sw.Stop();
        printf("fps = %0.2f\n", 1.0f / sw.GetElapsed() );
    }
    ::FreeConsole();
    return 0;
}

// ���[�g�V�O�l�`���̏�����
void InitRootSignature(RootSignature& rs)
{
    rs.Init(D3D12_FILTER_MIN_MAG_MIP_LINEAR,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP);
}

/// <summary>
/// �W�����o�̓R���\�[����������
/// </summary>
void InitStandard�h�nConsole()
{
    ::AllocConsole();               // �R�}���h�v�����v�g���\�������
    freopen("CON", "w", stdout);    // �W���o�͂̊��蓖��

    auto fhandle = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT rc;

    rc.Top = 0;
    rc.Left = 0;
    rc.Bottom = 30;
    rc.Right = 30;
    ::SetConsoleWindowInfo(fhandle, TRUE, &rc);
}

/// <summary>
/// ���C�g��������
/// </summary>
/// <param name="light"></param>
void InitLight(Light& light)
{
    std::random_device seed_gen;
    std::mt19937 random(seed_gen());

    light.eyePos = g_camera3D->GetPosition();
    light.specPow = 5.0f;
    light.mViewProjInv.Inverse(g_camera3D->GetViewProjectionMatrix());

    // �f�B���N�V�������C�g��������
    light.directionLights[0].direction.Set(1.0f, 0.0f, 0.0f);
    light.directionLights[0].color.Set(0.5f, 0.5f, 0.5f);

    light.directionLights[1].direction.Set(-1.0f, 0.0f, 0.0f);
    light.directionLights[1].color.Set(0.5f, 0.0f, 0.0f);

    light.directionLights[2].direction.Set(0.0f, 0.0f, 1.0f);
    light.directionLights[2].color.Set(0.0f, 0.5f, 0.0f);

    light.directionLights[3].direction.Set(0.0f, -1.0f, 0.0f);
    light.directionLights[3].color.Set(0.0f, 0.0f, 0.5f);

    // �|�C���g���C�g��������
    light.numPointLight = 1000;
    Matrix mView = g_camera3D->GetViewMatrix();
    for (int i = 0; i < light.numPointLight; i++)
    {
        auto& pt = light.pointLights[i];
        pt.position.x = static_cast<float>(random() % 1000) - 500.0f;
        pt.position.y = 20.0f; // ������20�Œ�
        pt.position.z = static_cast<float>(random() % 1000) - 500.0f;
        pt.positionInView = pt.position;
        mView.Apply(pt.positionInView);

        pt.range = 50.0f;       // �e���͈͂�50�ŌŒ肵�Ă���
        pt.color.x = static_cast<float>(random() % 255) / 255.0f;
        pt.color.y = static_cast<float>(random() % 255) / 255.0f;
        pt.color.z = static_cast<float>(random() % 255) / 255.0f;
    }
    light.screenParam.x = g_camera3D->GetNear();
    light.screenParam.y = g_camera3D->GetFar();
    light.screenParam.z = FRAME_BUFFER_W;
    light.screenParam.w = FRAME_BUFFER_H;
}

/// <summary>
/// ���f���̏�����
/// </summary>
/// <param name="teapotModel"></param>
/// <param name="bgModel"></param>
void InitModel(Model& teapotModel, Model& bgModel, Light& light, IShaderResource& pointLightNoListInTileUAV)
{
    // �e�B�[�|�b�g�̃��f����������
    ModelInitData teapotModelInitData;
    teapotModelInitData.m_tkmFilePath = "Assets/modelData/teapot.tkm";
    teapotModelInitData.m_fxFilePath = "Assets/shader/model.fx";
    teapotModelInitData.m_expandConstantBuffer = &light;
    teapotModelInitData.m_expandConstantBufferSize = sizeof(light);
    teapotModelInitData.m_expandShaderResoruceView[0] = &pointLightNoListInTileUAV;
    teapotModel.Init(teapotModelInitData);

    // �w�i�̃��f����������
    ModelInitData bgModelInitData;

    // ���[�U�[�g���f�[�^�Ƃ��ă|�C���g���C�g�̃��X�g��n��
    bgModelInitData.m_tkmFilePath = "Assets/modelData/bg.tkm";
    bgModelInitData.m_fxFilePath = "Assets/shader/model.fx";
    bgModelInitData.m_expandConstantBuffer = &light;
    bgModelInitData.m_expandConstantBufferSize = sizeof(light);
    bgModelInitData.m_expandShaderResoruceView[0] = &pointLightNoListInTileUAV;
    bgModel.Init(bgModelInitData);
}
