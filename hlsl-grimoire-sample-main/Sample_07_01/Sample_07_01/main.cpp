#include "stdafx.h"
#include "system/system.h"

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
    Vector3 eyePos;         // �J�����̈ʒu
    float specPow;          // �X�y�L�����̍i��
    Vector3 ambinetLight;   // ����
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

    light.directionalLight[0].color.x = 5.8f;
    light.directionalLight[0].color.y = 5.8f;
    light.directionalLight[0].color.z = 5.8f;

    light.directionalLight[0].direction.x = 1.0f;
    light.directionalLight[0].direction.y = -1.0f;
    light.directionalLight[0].direction.z = -1.0f;
    light.directionalLight[0].direction.Normalize();

    light.directionalLight[1].color.x = 0.8f;
    light.directionalLight[1].color.y = 0.8f;
    light.directionalLight[1].color.z = 0.8f;

    light.directionalLight[1].direction.x = 1.0f;
    light.directionalLight[1].direction.y = 1.0f;
    light.directionalLight[1].direction.z = -1.0f;
    light.directionalLight[1].direction.Normalize();

    light.directionalLight[2].color.x = 1.2f;
    light.directionalLight[2].color.y = 1.2f;
    light.directionalLight[2].color.z = 1.2f;

    light.directionalLight[2].direction.x = 0.0f;
    light.directionalLight[2].direction.y = 0.0f;
    light.directionalLight[2].direction.z = 1.0f;
    light.directionalLight[2].direction.Normalize();

    light.directionalLight[3].color.x = 1.2f;
    light.directionalLight[3].color.y = 1.2f;
    light.directionalLight[3].color.z = 1.2f;

    light.directionalLight[3].direction.x = 0.0f;
    light.directionalLight[3].direction.y = 1.0f;
    light.directionalLight[3].direction.z = 0.0f;
    light.directionalLight[3].direction.Normalize();

    light.ambinetLight.x = 0.6f;
    light.ambinetLight.y = 0.6f;
    light.ambinetLight.z = 0.6f;
    light.eyePos = g_camera3D->GetPosition();
    light.specPow = 5.0f;

    // 3D���f�����쐬
    Model model;
    ModelInitData initData;

    initData.m_tkmFilePath = "Assets/modelData/sample.tkm";
    initData.m_fxFilePath = "Assets/shader/Sample.fx";
    initData.m_expandConstantBuffer = &light;
    initData.m_expandConstantBufferSize = sizeof(light);

    model.Init(initData);
    Vector3 pos, scale;

    scale.x = 9.0f;
    scale.y = 9.0f;
    scale.z = 9.0f;

    Quaternion rot;
    rot.SetRotationDegX(90.0f);
    model.UpdateWorldMatrix(pos, g_quatIdentity, scale);

    //////////////////////////////////////
    // ���������s���R�[�h�������̂͂����܂ŁI�I�I
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    g_camera3D->SetPosition({ 0.0f, 400.0f, 1000.0f });
    g_camera3D->SetTarget({ 0.0f, 400.0f, 0.0f });

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

        for (auto& lig : light.directionalLight)
        {
            qRot.Apply(lig.direction);
        }

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

