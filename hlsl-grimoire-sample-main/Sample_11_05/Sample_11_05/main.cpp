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

    // step-2 �V���h�E�}�b�v���ڂ������߂̃I�u�W�F�N�g������������

    // step-3 GPU���ŗ��p����V���h�E�p�̍\���̂��`����

    // step-4 GPU���ɑ���f�[�^��ݒ肷��

    // step-5 �V���h�E�}�b�v�ɕ`�悷�郂�f��������������

    // �ʏ�`��̃e�B�[�|�b�g���f����������
    ModelStandard teapotModel;
    teapotModel.Init("Assets/modelData/teapot.tkm");
    teapotModel.Update(
        { 0, 50, 0 },
        g_quatIdentity,
        g_vec3One
    );

    // step-6 �e���󂯂�w�i���f����������

    //////////////////////////////////////
    // ���������s���R�[�h�������̂͂����܂ŁI�I�I
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    //  ��������Q�[�����[�v
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
        //�G��`���R�[�h�������̂͂����܂ŁI�I�I
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
