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

// ���C�g�J�����O�Ŏg�p����J�������
struct alignas(16) LightCullingCameraData
{
    Matrix mProj;           // �v���W�F�N�V�����s��
    Matrix mProjInv;        // �v���W�F�N�V�����s��̋t�s��
    Matrix mCameraRot;      // �J�����̉�]�s��
    Vector4 screenParam;    // �X�N���[�����
};

const int MAX_POINT_LIGHT = 1000;   // �|�C���g���C�g�̍ő吔
const int NUM_DIRECTION_LIGHT = 4;  // �f�B���N�V�������C�g�̐�

// ���C�g�\����
struct Light
{
    DirectionalLight directionLights[ NUM_DIRECTION_LIGHT]; // �f�B���N�V�������C�g
    PointLight pointLights[MAX_POINT_LIGHT];                // �|�C���g���C�g
    Matrix mViewProjInv;                                    // �r���[�v���W�F�N�V�����s��̋t�s��
    Vector3 eyePos;                                         // ���_
    float specPow;                                          // �X�y�L�����̍i��
    int numPointLight;                                      // �|�C���g���C�g�̐�
};

/////////////////////////////////////////////////////////////////
//�֐��錾
/////////////////////////////////////////////////////////////////
void InitRootSignature(RootSignature& rs);
void InitStandardIOConsole();
void InitLight(Light& light);
void InitModel(Model& teapotModel, Model& bgModel);
void InitDefferedLightingSprite(Sprite& defferedSprite, RenderTarget* gbuffers[], int numGbuffer, Light& light, IShaderResource& srv);
void InitGBuffers(RenderTarget& albedoRT, RenderTarget& normalRT, RenderTarget& depthRT);
void RenderGBuffer(RenderContext& renderContext, RenderTarget* gbuffers[], int numGbuffer, Model& teapotModel, Model& bgModel);
void InitPipelineState(RootSignature& rs, PipelineState& pipelineState, Shader& cs);
void DefferedLighting(RenderContext& renderContext, Sprite& defferedSprite);

///////////////////////////////////////////////////////////////////
// �E�B���h�E�v���O�����̃��C���֐�
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    // �Q�[���̏�����
    InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));
    // �W�����o�̓R���\�[���̏�����
    InitStandardIOConsole();

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

    // �e�B�[�|�b�g�̃��f���Ɣw�i���f����������
    Model teapotModel, bgModel;
    InitModel(teapotModel, bgModel);

    // G-Buffer���쐬
    RenderTarget albedoRT;
    RenderTarget normalRT;
    RenderTarget depthRT;
    RenderTarget* gbuffers[] = {
        &albedoRT,      // 0�Ԗڂ̃����_�����O�^�[�Q�b�g
        &normalRT,      // 1�Ԗڂ̃����_�����O�^�[�Q�b�g
        &depthRT        // 2�Ԗڂ̃����_�����O�^�[�Q�b�g
    };
    InitGBuffers(albedoRT, normalRT, depthRT);

    // step-1 ���C�g�J�����O�p�̃R���s���[�g�V�F�[�_�[�����[�h

    // step-2 ���C�g�J�����O�p�̃p�C�v���C���X�e�[�g��������

    // step-3 �^�C�����Ƃ̃|�C���g���C�g�̔ԍ��̃��X�g���o�͂���UAV��������

    // step-4 �|�C���g���C�g�̏��𑗂邽�߂̒萔�o�b�t�@���쐬

    // step-5 ���C�g�J�����O�p�̃f�B�X�N���v�^�q�[�v���쐬

    // �|�X�g�G�t�F�N�g�I�Ƀf�B�t�@�[�h���C�e�B���O���s�����߂̃X�v���C�g��������
    Sprite defferdLightingSpr;
    InitDefferedLightingSprite(
        defferdLightingSpr,
        gbuffers,
        ARRAYSIZE(gbuffers),
        light,
        pointLightNoListInTileUAV
    );

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

        // G-Buffer�Ƀ����_�����O
        RenderGBuffer(renderContext, gbuffers, ARRAYSIZE(gbuffers), teapotModel, bgModel);

        // step-6 ���C�g�J�����O�̃R���s���[�g�V�F�[�_�[���f�B�X�p�b�`

        // ���\�[�X�o���A
        renderContext.TransitionResourceState(
            pointLightNoListInTileUAV.GetD3DResoruce(),
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        // �f�B�t�@�[�h���C�e�B���O
        DefferedLighting(renderContext, defferdLightingSpr);

        renderContext.TransitionResourceState(
            pointLightNoListInTileUAV.GetD3DResoruce(),
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

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
// / �p�C�v���C���X�e�[�g�̏�����
/// </summary>
/// <param name="rs"></param>
/// <param name="pipelineState"></param>
/// <param name="cs"></param>
void InitPipelineState(RootSignature& rs, PipelineState& pipelineState, Shader& cs)
{
    // �p�C�v���C���X�e�[�g���쐬
    D3D12_COMPUTE_PIPELINE_STATE_DESC  psoDesc = { 0 };
    psoDesc.pRootSignature = rs.Get();
    psoDesc.CS = CD3DX12_SHADER_BYTECODE(cs.GetCompiledBlob());
    psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    psoDesc.NodeMask = 0;

    pipelineState.Init(psoDesc);
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
}

/// <summary>
/// ���f���̏�����
/// </summary>
/// <param name="teapotModel"></param>
/// <param name="bgModel"></param>
void InitModel(Model& teapotModel, Model& bgModel)
{
    // �e�B�[�|�b�g���f����������
    ModelInitData teapotModelInitData;
    teapotModelInitData.m_tkmFilePath = "Assets/modelData/teapot.tkm";
    teapotModelInitData.m_fxFilePath = "Assets/shader/renderGBuffer.fx";
    teapotModelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    teapotModelInitData.m_colorBufferFormat[1] = DXGI_FORMAT_R16G16B16A16_FLOAT;
    teapotModelInitData.m_colorBufferFormat[2] = DXGI_FORMAT_R32_FLOAT;
    teapotModel.Init(teapotModelInitData);

    // �w�i�̃��f����������
    ModelInitData bgModelInitData;

    // ���[�U�[�g���f�[�^�Ƃ��ă|�C���g���C�g�̃��X�g��n��
    bgModelInitData.m_tkmFilePath = "Assets/modelData/bg.tkm";
    bgModelInitData.m_fxFilePath = "Assets/shader/renderGBuffer.fx";
    bgModelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    bgModelInitData.m_colorBufferFormat[1] = DXGI_FORMAT_R16G16B16A16_FLOAT;
    bgModelInitData.m_colorBufferFormat[2] = DXGI_FORMAT_R32_FLOAT;
    bgModel.Init(bgModelInitData);
}

/// <summary>
/// �f�B�t�@�[�h���C�e�B���O�p�̃X�v���C�g��������
/// </summary>
/// <param name="defferedSprite"></param>
void InitDefferedLightingSprite(
    Sprite& defferedSprite,
    RenderTarget* gbuffers[],
    int numGbuffer,
    Light& light,
    IShaderResource& srv)
{
    SpriteInitData spriteInitData;

    // ��ʑS�̂Ƀ����_�����O����̂ŕ��ƍ����̓t���[���o�b�t�@�̕��ƍ����Ɠ���
    spriteInitData.m_width = FRAME_BUFFER_W;
    spriteInitData.m_height = FRAME_BUFFER_H;
    spriteInitData.m_expandShaderResoruceView = &srv;

    // �f�B�t�@�[�h���C�e�B���O�Ŏg�p����e�N�X�`����ݒ�
    for (int i = 0; i < numGbuffer; i++)
    {
        spriteInitData.m_textures[i] = &gbuffers[i]->GetRenderTargetTexture();
    }

    spriteInitData.m_fxFilePath = "Assets/shader/defferedLighting.fx";
    spriteInitData.m_expandConstantBuffer = &light;
    spriteInitData.m_expandConstantBufferSize = sizeof(light);
    defferedSprite.Init(spriteInitData);
}

/// <summary>
/// G-Buffer�̏�����
/// </summary>
/// <param name="albedoRT"></param>
/// <param name="normalRT"></param>
/// <param name="depthRT"></param>
void InitGBuffers(RenderTarget& albedoRT, RenderTarget& normalRT, RenderTarget& depthRT)
{
    // �A���x�h�J���[�o�͗p�̃����_�����O�^�[�Q�b�g
    albedoRT.Create(
        FRAME_BUFFER_W,
        FRAME_BUFFER_H,
        1,
        1,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_D32_FLOAT);

    // �@���o�͗p�̃����_�����O�^�[�Q�b�g
    normalRT.Create(
        FRAME_BUFFER_W,
        FRAME_BUFFER_H,
        1,
        1,
        DXGI_FORMAT_R16G16B16A16_FLOAT,
        DXGI_FORMAT_UNKNOWN);

    // Z�l�o�͗p�̃����_�����O�^�[�Q�b�g
    depthRT.Create(
        FRAME_BUFFER_W,
        FRAME_BUFFER_H,
        1,
        1,
        DXGI_FORMAT_R32_FLOAT,
        DXGI_FORMAT_UNKNOWN);
}

/// <summary>
/// GBuffer�Ƀ����_�����O
/// </summary>
/// <param name="renderContext"></param>
/// <param name="gbuffers"></param>
/// <param name="numGbuffer"></param>
/// <param name="ladyModel"></param>
/// <param name="bgModel"></param>
void RenderGBuffer(RenderContext& renderContext, RenderTarget* gbuffers[], int numGbuffer, Model& teapotModel, Model& bgModel)
{
    // �܂��A�����_�����O�^�[�Q�b�g�Ƃ��Đݒ�ł���悤�ɂȂ�܂ő҂�
    renderContext.WaitUntilToPossibleSetRenderTargets(numGbuffer, gbuffers);

    // �����_�����O�^�[�Q�b�g��ݒ�
    renderContext.SetRenderTargets(numGbuffer, gbuffers);

    // �����_�����O�^�[�Q�b�g���N���A
    renderContext.ClearRenderTargetViews(numGbuffer, gbuffers);

    teapotModel.Draw(renderContext);
    bgModel.Draw(renderContext);

    // �����_�����O�^�[�Q�b�g�ւ̏������ݑ҂�
    renderContext.WaitUntilFinishDrawingToRenderTargets(numGbuffer, gbuffers);
}

/// <summary>
/// �f�B�t�@�[�h���C�e�B���O
/// </summary>
/// <param name="renderContext"></param>
/// <param name="defferedSprite"></param>
void DefferedLighting(RenderContext& renderContext, Sprite& defferedSprite)
{
    // �����_�����O����t���[���o�b�t�@�ɖ߂��ăX�v���C�g�������_�����O����
    g_graphicsEngine->ChangeRenderTargetToFrameBuffer(renderContext);

    // G-Buffer�̓��e�����ɂ��ăX�v���C�g�������_�����O
    defferedSprite.Draw(renderContext);
}
