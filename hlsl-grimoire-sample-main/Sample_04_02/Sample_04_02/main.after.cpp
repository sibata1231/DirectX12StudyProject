#include "stdafx.h"
#include "system/system.h"

//step-1 �f�B���N�V�������C�g�p�̍\���̂��`����
struct DirectionLight
{
    Vector3 ligDirection; //���C�g�̕���

    // HLSL���̒萔�o�b�t�@��float3�^�̕ϐ���
    // 16�̔{���̃A�h���X�ɔz�u����邽�߁AC++���ɂ̓p�f�B���O�𖄂߂Ă���
    float pad;

    Vector3 ligColor;     //���C�g�̃J���[
};

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

    g_camera3D->SetPosition({ 0.0f, 0.0f, 100.0f });
    g_camera3D->SetTarget({ 0.0f, 0.0f, 0.0f });

    // step-2 �f�B���N�V�������C�g�̃f�[�^���쐬����
    DirectionLight directionLig;
    // ���C�g�͎΂ߏォ�瓖�����Ă���
    directionLig.ligDirection.x = 1.0f;
    directionLig.ligDirection.y = -1.0f;
    directionLig.ligDirection.z = -1.0f;

    // ���K������
    directionLig.ligDirection.Normalize();

    // ���C�g�̃J���[�͊D�F
    directionLig.ligColor.x = 0.5f;
    directionLig.ligColor.y = 0.5f;
    directionLig.ligColor.z = 0.5f;

    // step-3 ���f��������������
    // ���f�������������邽�߂̏����\�z����
    ModelInitData modelInitData;
    modelInitData.m_tkmFilePath = "Assets/modelData/teapot.tkm";

    // �g�p����V�F�[�_�[�t�@�C���p�X��ݒ肷��
    modelInitData.m_fxFilePath = "Assets/shader/sample.fx";

    // �f�B���N�V�������C�g�̏����f�B�X�N���v�^�q�[�v�ɒ萔�o�b�t�@�[�Ƃ���
    // �o�^���邽�߂Ƀ��f���̏��������Ƃ��ēn��
    modelInitData.m_expandConstantBuffer = &directionLig;
    modelInitData.m_expandConstantBufferSize = sizeof(directionLig);

    // �����������g���ă��f��������������
    Model model;
    model.Init(modelInitData);

    //////////////////////////////////////
    // ���������s���R�[�h�������̂͂����܂ŁI�I�I
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    // ��������Q�[�����[�v
    while (DispatchWindowMessage())
    {
        // �����_�����O�J�n
        g_engine->BeginFrame();
        //////////////////////////////////////
        // ��������G��`���R�[�h���L�q����
        //////////////////////////////////////

        // step-4 ���f�����h���[����
        model.Draw(renderContext);

        //////////////////////////////////////
        // �G��`���R�[�h�������̂͂����܂ŁI�I�I
        //////////////////////////////////////
        // �����_�����O�I��
        g_engine->EndFrame();
    }
    return 0;
}

