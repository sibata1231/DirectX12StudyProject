#include "stdafx.h"
#include "system/system.h"
#include <time.h>

const int NUM_DIRECTIONAL_LIGHT = 4; // �f�B���N�V�������C�g�̐�

/// <summary>
/// �f�B���N�V�������C�g
/// </summary>
struct DirectionalLight
{
    Vector3 direction;  // ���C�g�̕���
    float pad0;         // �p�f�B���O
    Vector4 color;      // ���C�g�̃J���[
};

/// <summary>
/// ���C�g�\����
/// </summary>
struct Light
{
    DirectionalLight directionalLight[NUM_DIRECTIONAL_LIGHT]; // �f�B���N�V�������C�g
    Vector3 eyePos;             // �J�����̈ʒu
    float specPow;              // �X�y�L�����̍i��
    Vector3 ambinetLight;       // ����
};

#define USE_UNITY_CHAN 0
#define USE_NINJA 1
#define USE_ROBO 2

#define USE_MODEL 2

///////////////////////////////////////////////////////////////////
// �E�B���h�E�v���O�����̃��C���֐�
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    // �Q�[���̏�����
    InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));

    srand(time(nullptr) );
    //////////////////////////////////////
    // �������珉�������s���R�[�h���L�q����
    //////////////////////////////////////

    // 3D���f�����쐬
    Model model, bgModel;
    ModelInitData initData;

    initData.m_tkmFilePath = "Assets/modelData/sample.tkm";
    initData.m_fxFilePath = "Assets/shader/NoAnimModel_PBR.fx";

    model.Init(initData);

    initData.m_tkmFilePath = "Assets/modelData/bg/bg.tkm";
    bgModel.Init(initData);

    //////////////////////////////////////
    // ���������s���R�[�h�������̂͂����܂ŁI�I�I
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    g_camera3D->SetPosition(0.0f, 50.0f, 120.0f);
    g_camera3D->SetTarget(0.0f, 50.0f, 200.0f);

    g_graphicsEngine->RegistModelToRaytracingWorld(model);
    g_graphicsEngine->RegistModelToRaytracingWorld(bgModel);
    g_graphicsEngine->BuildRaytracingWorld(renderContext);

    // ��������Q�[�����[�v
    while (DispatchWindowMessage())
    {
        // �����_�����O�J�n
        g_engine->BeginFrame();

        if (g_pad[0]->IsPress(enButtonLB1))
        {
            g_camera3D->MoveUp(g_pad[0]->GetLStickYF());
        }
        else
        {
            g_camera3D->MoveForward(-g_pad[0]->GetLStickYF());
        }
        g_camera3D->MoveRight(g_pad[0]->GetLStickXF());
        Quaternion qRotX, qRotY;
        qRotX.SetRotationX(g_pad[0]->GetRStickYF() * -0.005f);
        g_camera3D->RotateOriginTarget(qRotX);
        qRotY.SetRotationY(g_pad[0]->GetRStickXF() * 0.005f);
        g_camera3D->RotateOriginTarget(qRotY);

        //////////////////////////////////////
        // ��������G��`���R�[�h���L�q����
        //////////////////////////////////////
        g_graphicsEngine->DispatchRaytracing(renderContext);

        //////////////////////////////////////
        // �G��`���R�[�h�������̂͂����܂ŁI�I�I
        //////////////////////////////////////
        // �����_�����O�I��
        g_engine->EndFrame();
    }
    return 0;
}

