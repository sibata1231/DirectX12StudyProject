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

struct DispSetting
{
    Vector3 cameraPos;
    Vector3 cameraTarget;
};

enum
{
    Monster,
    Human,
    Lantern,
    Num
};

DispSetting dispSettings[Num];

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

    // ���z��
    light.directionalLight[0].color.x = 3.0f;
    light.directionalLight[0].color.y = 3.0f;
    light.directionalLight[0].color.z = 3.0f;

    light.directionalLight[0].direction.x = 2.0f;
    light.directionalLight[0].direction.y = -1.0f;
    light.directionalLight[0].direction.z = 3.0f;
    light.directionalLight[0].direction.Normalize();

    // �n�ʂ���̏Ƃ�Ԃ�
    light.directionalLight[1].color.x = 1.5f;
    light.directionalLight[1].color.y = 1.5f;
    light.directionalLight[1].color.z = 1.5f;

    light.directionalLight[1].direction.x = 0.0f;
    light.directionalLight[1].direction.y = 0.0f;
    light.directionalLight[1].direction.z = -1.0f;
    light.directionalLight[1].direction.Normalize();

    light.ambinetLight.x = 0.4f;
    light.ambinetLight.y = 0.4f;
    light.ambinetLight.z = 0.4f;
    
    light.eyePos = g_camera3D->GetPosition();
    light.specPow = 5.0f;

    // 3D���f�����쐬
    Model models[Num];
    ModelInitData initData;

    initData.m_tkmFilePath = "Assets/modelData/monster.tkm";
    initData.m_fxFilePath = "Assets/shader/sample.fx";
    initData.m_expandConstantBuffer = &light;
    initData.m_expandConstantBufferSize = sizeof(light);

    models[Monster].Init(initData);

    initData.m_tkmFilePath = "Assets/modelData/Human.tkm";
    models[Human].Init(initData);

    initData.m_tkmFilePath = "Assets/modelData/lantern.tkm";
    models[Lantern].Init(initData);

    Vector3 pos, scale;


    dispSettings[Monster].cameraPos = { 0.0f, 85.0f, 50.0f };
    dispSettings[Monster].cameraTarget = { 0.0f, 85.0f, 0.0f };
    dispSettings[Human].cameraPos = { 0.0f, 160.0f, 50.0f };
    dispSettings[Human].cameraTarget = { 0.0f, 160.0f, 0.0f };
    dispSettings[Lantern].cameraPos = { 0.0f, 50.0f, 120.0f };
    dispSettings[Lantern].cameraTarget = { 0.0f, 50.0f, 0.0f };

    //////////////////////////////////////
    // ���������s���R�[�h�������̂͂����܂ŁI�I�I
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    int dispModelNo = Monster;

    g_camera3D->SetPosition(dispSettings[dispModelNo].cameraPos);
    g_camera3D->SetTarget(dispSettings[dispModelNo].cameraTarget);

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

        models[dispModelNo].Draw(renderContext);
        if (g_pad[0]->IsTrigger(enButtonA))
        {
            dispModelNo = (dispModelNo + 1) % Num;
            g_camera3D->SetPosition(dispSettings[dispModelNo].cameraPos);
            g_camera3D->SetTarget(dispSettings[dispModelNo].cameraTarget);
        }

        //////////////////////////////////////
        // �G��`���R�[�h�������̂͂����܂ŁI�I�I
        //////////////////////////////////////
        // �����_�����O�I��
        g_engine->EndFrame();
    }
    return 0;
}
