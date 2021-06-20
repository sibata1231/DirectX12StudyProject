#include "stdafx.h"
#include "system/system.h"
#include "ModelStandard.h"

// �֐��錾
void InitRootSignature(RootSignature& rs);

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

    g_camera3D->SetPosition(0, 100.0f, 350.0f);
    RootSignature rs;
    InitRootSignature(rs);

    // �e�`��p�̃��C�g�J�������쐬����
    Camera lightCamera;

    // �J�����̈ʒu��ݒ�B����̓��C�g�̈ʒu
    lightCamera.SetPosition(0, 500, 0);

    // �J�����̒����_��ݒ�B���ꂪ���C�g���Ƃ炵�Ă���ꏊ
    lightCamera.SetTarget(0, 0, 0);

    // �������ݒ�B����̓��C�g���^���������Ă���̂ŁAX��������ɂ��Ă���
    lightCamera.SetUp(1, 0, 0);

    // ���C�g�r���[�v���W�F�N�V�����s����v�Z���Ă���
    lightCamera.Update();

    // step-1 �V���h�E�}�b�v�`��p�̃����_�����O�^�[�Q�b�g���쐬����

    // step-2 �V���h�E�}�b�v�ɕ`�悷�郂�f��������������

    // �ʏ�`��̃e�B�[�|�b�g���f����������
    ModelStandard teapotModel;
    teapotModel.Init("Assets/modelData/teapot.tkm");
    teapotModel.Update(
        { 0, 50, 0 },
        g_quatIdentity,
        g_vec3One
    );

    // �e���󂯂�w�i���f����������
    ModelInitData bgModelInitData;

    // �V���h�E���V�[�o�[�i�e�����Ƃ���郂�f���j�p�̃V�F�[�_�[���w�肷��
    bgModelInitData.m_fxFilePath = "Assets/shader/sampleShadowReciever.fx";

    // �V���h�E�}�b�v���g��SRV�ɐݒ肷��
    bgModelInitData.m_expandShaderResoruceView = &shadowMap.GetRenderTargetTexture();

    // ���C�g�r���[�v���W�F�N�V�����s����g���萔�o�b�t�@�[�ɐݒ肷��
    bgModelInitData.m_expandConstantBuffer = (void*)&lightCamera.GetViewProjectionMatrix();
    bgModelInitData.m_expandConstantBufferSize = sizeof(lightCamera.GetViewProjectionMatrix());
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

        Quaternion qAddRot;
        qAddRot.SetRotationDegX(g_pad[0]->GetLStickYF());
        g_camera3D->RotateOriginTarget(qAddRot);
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

        // �ʏ탌���_�����O
        // �����_�����O�^�[�Q�b�g���t���[���o�b�t�@�ɖ߂�
        renderContext.SetRenderTarget(
            g_graphicsEngine->GetCurrentFrameBuffuerRTV(),
            g_graphicsEngine->GetCurrentFrameBuffuerDSV()
        );
        renderContext.SetViewport(g_graphicsEngine->GetFrameBufferViewport());

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
