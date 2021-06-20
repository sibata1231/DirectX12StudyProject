#include "stdafx.h"
#include "system/system.h"

const int NUM_DIRECTIONAL_LIGHT = 4; // �f�B���N�V�������C�g�̐�

/// <summary>
/// ���C�g�\����
/// </summary>
struct Light
{
    Vector3 direction;      // ���C�g�̕���
    float pad0;             // �p�f�B���O
    Vector4 color;          // ���C�g�̃J���[
    Vector3 eyePos;         // �J�����̈ʒu
    float specPow;          // �X�y�L�����̍i��
    Vector3 ambinetLight;   // ����
};

struct DispSetting
{
    Vector3 cameraPos;
    Vector3 cameraTarget;
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

    // ���C�g��p�ӂ���
    Light light;

    light.color.x = 0.6f;
    light.color.y = 0.6f;
    light.color.z = 0.6f;

    light.direction.x = 1.0f;
    light.direction.y = -1.0f;
    light.direction.z = -1.0f;

    light.ambinetLight.x = 0.3f;
    light.ambinetLight.y = 0.3f;
    light.ambinetLight.z = 0.3f;

    light.eyePos = g_camera3D->GetPosition();
    light.specPow = 5.0f;

    // 3D���f�����쐬
    Model model;
    ModelInitData initData;

    initData.m_tkmFilePath = "Assets/modelData/sample.tkm";
    initData.m_fxFilePath = "Assets/shader/sample.fx";
    initData.m_expandConstantBuffer = &light;
    initData.m_expandConstantBufferSize = sizeof(light);

    model.Init(initData);
    model.UpdateWorldMatrix(g_vec3Zero, g_quatIdentity, g_vec3One);

    g_camera3D->SetPosition({ 0.0f, 150.0f, 300.0f });
    g_camera3D->SetTarget({ 0.0f, 150.0f, 0.0f });


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
        if (g_pad[0]->IsPress(enButtonRight))
        {
            qRot.SetRotationDegY(1.0f);
        }
        else if (g_pad[0]->IsPress(enButtonLeft))
        {
            qRot.SetRotationDegY(-1.0f);
        }

        qRot.Apply(light.direction);

        // �J��������
        qRot.SetRotationDegY(g_pad[0]->GetLStickXF());
        auto camPos = g_camera3D->GetPosition();
        qRot.Apply(camPos);
        g_camera3D->SetPosition(camPos);

        Vector3 rotAxis;
        auto toPos = g_camera3D->GetPosition() - g_camera3D->GetTarget();
        auto dir = toPos;
        dir.Normalize();
        rotAxis.Cross(dir, g_vec3AxisY);
        qRot.SetRotationDeg(rotAxis, g_pad[0]->GetLStickYF());
        qRot.Apply(toPos);
        g_camera3D->SetPosition(g_camera3D->GetTarget() + toPos);

        light.eyePos = g_camera3D->GetPosition();

        model.Draw(renderContext);

        //////////////////////////////////////
        // �G��`���R�[�h�������̂͂����܂ŁI�I�I
        //////////////////////////////////////
        // �����_�����O�I��
        g_engine->EndFrame();
    }
    return 0;
}
