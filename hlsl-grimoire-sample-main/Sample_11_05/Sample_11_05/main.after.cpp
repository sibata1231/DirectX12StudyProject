#include "stdafx.h"
#include "system/system.h"
#include "ModelStandard.h"

// �֐��錾
void InitRootSignature(RootSignature& rs);
void InitLightCamera(Camera& lightCamera, Vector3& ligPos);
void MoveCamera();
///////////////////////////////////////////////////////////////////
// �E�B���h�E�v���O�����̃��C���֐�
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    // �Q�[���̏�����
    InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));

    //////////////////////////////////////
    // �������珉�������s���R�[�h���L�q����
    //////////////////////////////////////

    g_camera3D->SetPosition(0, 50.0f, 250.0f);
    g_camera3D->SetTarget(0, 0, 0);
    RootSignature rs;
    InitRootSignature(rs);

    // ���C�g�̍��W
    Vector3 ligPos = { 0, 300, 0 };
    // �e�`��p�̃��C�g�J�������쐬����
    Camera lightCamera;
    InitLightCamera(lightCamera, ligPos);

    // step-1 �V���h�E�}�b�v�`��p�̃����_�����O�^�[�Q�b�g���쐬����
    float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    RenderTarget shadowMap;
    shadowMap.Create(
        2048,
        2048,
        1,
        1,
        // �y���ځz�V���h�E�}�b�v�̃J���[�o�b�t�@�[�̃t�H�[�}�b�g��ύX���Ă���
        DXGI_FORMAT_R32G32_FLOAT,
        DXGI_FORMAT_D32_FLOAT,
        clearColor
    );

    // step-2 �V���h�E�}�b�v���ڂ������߂̃I�u�W�F�N�g������������
    GaussianBlur shadowBlur;
    shadowBlur.Init(
        &shadowMap.GetRenderTargetTexture() // �ڂ����e�N�X�`���̓V���h�E�}�b�v�̃e�N�X�`��
    );

    // step-3 GPU���ŗ��p����V���h�E�p�̍\���̂��`����
    struct ShadowParam
    {
        Matrix mLVP;        // ���C�g�r���[�v���W�F�N�V�����s��
        Vector3 lightPos;   // ���C�g�̍��W
    };

    // step-4 GPU���ɑ���f�[�^��ݒ肷��
    ShadowParam sp;
    sp.mLVP = lightCamera.GetViewProjectionMatrix();
    sp.lightPos.Set(ligPos);

    // step-5 �V���h�E�}�b�v�ɕ`�悷�郂�f��������������
    // �e�B�[�|�b�g���f�������������邽�߂̏������f�[�^��ݒ肷��
    ModelInitData teapotShadowModelInitData;
    // �V���h�E�}�b�v�`��p�̃V�F�[�_�[���w�肷��
    teapotShadowModelInitData.m_fxFilePath = "Assets/shader/sampleDrawShadowMap.fx";
    teapotShadowModelInitData.m_tkmFilePath = "Assets/modelData/teapot.tkm";
    // �y���ځz�e�p�̃p�����[�^���g���萔�o�b�t�@�[�ɐݒ肷��
    teapotShadowModelInitData.m_expandConstantBuffer = (void*)&sp;
    teapotShadowModelInitData.m_expandConstantBufferSize = sizeof(sp);
    // �y���ځz�J���[�o�b�t�@�[�̃t�H�[�}�b�g�ɕύX���������̂ŁA��������ύX����
    teapotShadowModelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32_FLOAT;

    // �e�B�[�|�b�g���f��������������
    Model teapotShadowModel;
    teapotShadowModel.Init(teapotShadowModelInitData);
    teapotShadowModel.UpdateWorldMatrix(
        { 0, 50, 0 },
        g_quatIdentity,
        g_vec3One
    );

    // �ʏ�`��̃e�B�[�|�b�g���f����������
    ModelStandard teapotModel;
    teapotModel.Init("Assets/modelData/teapot.tkm");
    teapotModel.Update(
        { 0, 50, 0 },
        g_quatIdentity,
        g_vec3One
    );

    // step-6 �e���󂯂�w�i���f����������
    ModelInitData bgModelInitData;
    // �V���h�E���V�[�o�[(�e�����Ƃ���郂�f��)�p�̃V�F�[�_�[���w�肷��
    bgModelInitData.m_fxFilePath = "Assets/shader/sampleShadowReciever.fx";
    // �y���ځz�V���h�E�}�b�v�́A�K�E�V�A���u���[�łڂ��������̂𗘗p����
    bgModelInitData.m_expandShaderResoruceView[0] = &shadowBlur.GetBokeTexture();
    // �y���ځz�e�p�̃p�����[�^���g���萔�o�b�t�@�[�ɐݒ肷��
    bgModelInitData.m_expandConstantBuffer = (void*)&sp;
    bgModelInitData.m_expandConstantBufferSize = sizeof(sp);
    bgModelInitData.m_tkmFilePath = "Assets/modelData/bg/bg.tkm";

    Model bgModel;
    bgModel.Init(bgModelInitData);

    //////////////////////////////////////
    // ���������s���R�[�h�������̂͂����܂ŁI�I�I
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    // ��������Q�[�����[�v
    while (DispatchWindowMessage())
    {
        // 1�t���[���̊J�n
        g_engine->BeginFrame();

        // �J�����𓮂���
        MoveCamera();

        //////////////////////////////////////
        // ��������G��`���R�[�h���L�q����
        //////////////////////////////////////

        // �V���h�E�}�b�v�Ƀ����_�����O
        // �����_�����O�^�[�Q�b�g���V���h�E�}�b�v�ɕύX����
        renderContext.WaitUntilToPossibleSetRenderTarget(shadowMap);
        renderContext.SetRenderTargetAndViewport(shadowMap);
        renderContext.ClearRenderTargetView(shadowMap);

        // �e���f����`��
        teapotShadowModel.Draw(renderContext, lightCamera);

        // �������݊����҂�
        renderContext.WaitUntilFinishDrawingToRenderTarget(shadowMap);

        // step-7 �V���h�E�}�b�v���ڂ������߂̃K�E�V�A���u���[�����s����
        shadowBlur.ExecuteOnGPU(renderContext, 5.0f);

        // �ʏ탌���_�����O
        // �����_�����O�^�[�Q�b�g���t���[���o�b�t�@�[�ɖ߂�
        renderContext.SetRenderTarget(
            g_graphicsEngine->GetCurrentFrameBuffuerRTV(),
            g_graphicsEngine->GetCurrentFrameBuffuerDSV()
        );
        renderContext.SetViewportAndScissor(g_graphicsEngine->GetFrameBufferViewport());

        // �e�B�[�|�b�g���f����`��
        teapotModel.Draw(renderContext);
        // �e���󂯂�w�i��`��
        bgModel.Draw(renderContext);

        //////////////////////////////////////
        // �G��`���R�[�h�������̂͂����܂ŁI�I�I
        //////////////////////////////////////

        // 1�t���[���I��
        g_engine->EndFrame();
    }
    return 0;
}

// ���[�g�V�O�l�`���̏�����
void InitRootSignature( RootSignature& rs )
{
    rs.Init(D3D12_FILTER_MIN_MAG_MIP_LINEAR,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP);
}

void InitLightCamera(Camera& lightCamera, Vector3& ligPos)
{
    // �J�����̈ʒu��ݒ�B����̓��C�g�̈ʒu
    lightCamera.SetPosition(ligPos);
    // �J�����̒����_��ݒ�B���ꂪ���C�g���Ƃ炵�Ă���ꏊ
    lightCamera.SetTarget(0, 0, 0);
    // �������ݒ�B����̓��C�g���^���������Ă���̂ŁAX��������ɂ��Ă���
    lightCamera.SetUp(1, 0, 0);

    // ���C�g�r���[�v���W�F�N�V�����s����v�Z���Ă���
    lightCamera.Update();
}

void MoveCamera()
{
    Quaternion qAddRot;
    qAddRot.SetRotationDegX(g_pad[0]->GetRStickYF() * 0.5f);
    g_camera3D->RotateOriginTarget(qAddRot);
    auto pos = g_camera3D->GetPosition();
    auto target = g_camera3D->GetTarget();
    pos.z -= g_pad[0]->GetLStickYF() * 0.5f;
    target.z -= g_pad[0]->GetLStickYF() * 0.5f;
    g_camera3D->SetPosition(pos);
    g_camera3D->SetTarget(target);
}