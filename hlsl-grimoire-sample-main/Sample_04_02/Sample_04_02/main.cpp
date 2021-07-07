#include "stdafx.h"
#include "system/system.h"

// step-1 �f�B���N�V�������C�g�p�̍\���̂��`����
struct DirectionLight {
    Vector3 lightDirection; // ���C�g

    float pad;

    Vector3 lightColor;
};

///////////////////////////////////////////////////////////////////
// �E�B���h�E�v���O�����̃��C���֐�
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    // �Q�[���̏�����
    InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));

    //////////////////////////////////////
    // �������珉�������s���R�[�h���L�q����
    //////////////////////////////////////

    g_camera3D->SetPosition({ 0.0f, 0.0f, 100.0f });
    g_camera3D->SetTarget({ 0.0f, 0.0f, 0.0f });

    // step-2 �f�B���N�V�������C�g�̃f�[�^���쐬����
    DirectionLight directionLig;
    directionLig.lightDirection.x = 1.0f;
    directionLig.lightDirection.y = -1.0f;
    directionLig.lightDirection.z = -1.0f;
    
    directionLig.lightDirection.Normalize();

    directionLig.lightColor.x = 0.5f;
    directionLig.lightColor.y = 0.5f;
    directionLig.lightColor.z = 0.5f;

    // step-3 ���̃��f��������������
    ModelInitData modelInitData;
    modelInitData.m_tkmFilePath = "Assets/modelData/teapot.tkm";
    modelInitData.m_fxFilePath  = "Assets/shader/sample.fx";

    modelInitData.m_expandConstantBuffer = &directionLig;
    modelInitData.m_expandConstantBufferSize = sizeof(directionLig);

    Model model;
    model.Init(modelInitData);

    //////////////////////////////////////
    // ���������s���R�[�h�������̂͂����܂ŁI�I�I
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    // ��������Q�[�����[�v
    while (DispatchWindowMessage()) {
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
