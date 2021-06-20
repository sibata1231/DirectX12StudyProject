#include "stdafx.h"
#include "system/system.h"

struct DirectionLight
{
    Vector3 ligDirection;   // ���C�g�̕���
    float pad0;
    Vector3 ligColor;       // ���C�g�̃J���[
    float pad1;

    // step-1 �\���̂Ɏ��_�̈ʒu��ǉ�����
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

    // �f�B���N�V�������C�g�̃f�[�^���쐬����
    DirectionLight directionLig;

    // ���C�g�͉E�����瓖�����Ă���
    directionLig.ligDirection.x = 1.0f;
    directionLig.ligDirection.y = -1.0f;
    directionLig.ligDirection.z = -1.0f;
    directionLig.ligDirection.Normalize();

    // ���C�g�̃J���[�͔�
    directionLig.ligColor.x = 0.5f;
    directionLig.ligColor.y = 0.5f;
    directionLig.ligColor.z = 0.5f;

    // step-2 ���_�̈ʒu��ݒ肷��

    // ���f��������������
    // ���f�������������邽�߂̏����\�z����
    ModelInitData modelInitData;
    modelInitData.m_tkmFilePath = "Assets/modelData/teapot.tkm";

    // �g�p����V�F�[�_�[�t�@�C���p�X��ݒ肷��
    modelInitData.m_fxFilePath = "Assets/shader/sample.fx";

    // �f�B���N�V�������C�g�̏����f�B�X�N���v�^�q�[�v��
    // �萔�o�b�t�@�Ƃ��ēo�^���邽�߂Ƀ��f���̏��������Ƃ��ēn��
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
        Quaternion qRot;
        qRot.SetRotationDegY(g_pad[0]->GetLStickXF());
        qRot.Apply(directionLig.ligDirection);
        qRot.SetRotationDegX(g_pad[0]->GetLStickYF());
        qRot.Apply(directionLig.ligDirection);

        // �e�B�[�|�b�g���h���[����
        model.Draw(renderContext);

        //////////////////////////////////////
        // �G��`���R�[�h�������̂͂����܂ŁI�I�I
        //////////////////////////////////////
        // �����_�����O�I��
        g_engine->EndFrame();
    }
    return 0;
}
