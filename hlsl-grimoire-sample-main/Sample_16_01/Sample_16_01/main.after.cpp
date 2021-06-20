#include "stdafx.h"
#include "system/system.h"
#include <random>
#include "util/stopwatch.h"

// �֐��錾
void InitRootSignature(RootSignature& rs);
void InitStandardIOConsole();

// step-1 �|�C���g���C�g�\���̂��`����
// �|�C���g���C�g�\����
struct SPointLight
{
    Vector3 position;   // ���W
    float pad0;         // �p�f�B���O
    Vector3 color;      // ���C�g�̃J���[
    float range;        //  ���C�g�̉e����^����͈�
};

// step-2 �|�C���g���C�g�̐���\���萔���`����
const int NUM_POINT_LIGHT = 1000;

///////////////////////////////////////////////////////////////////
// �E�B���h�E�v���O�����̃��C���֐�
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    // �Q�[���̏�����
    InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));

    // �W�����o�̓R���\�[���̏�����
    InitStandardIOConsole();

    //////////////////////////////////////
    //  �������珉�������s���R�[�h���L�q����
    //////////////////////////////////////
    std::random_device seed_gen;
    std::mt19937 random(seed_gen());

    g_camera3D->SetPosition({ 0.0f, 200.0, 400.0f });

    // ���[�g�V�O�l�`�����쐬
    RootSignature rootSignature;
    InitRootSignature(rootSignature);

    // step-3 �|�C���g���C�g�������_���Ȉʒu�ƃJ���[�ŏ���������
    SPointLight pointLights[NUM_POINT_LIGHT];
    for (auto& pt : pointLights)
    {
        pt.position.x = static_cast<float>(random() % 1000) - 500.0f;
        pt.position.y = 20.0f;  // ������20�Œ�
        pt.position.z = static_cast<float>(random() % 1000) - 500.0f;
        pt.range = 50.0f;       // �e���͈͂�50�ŌŒ肵�Ă���
        pt.color.x = static_cast<float>(random() % 255) / 255.0f;
        pt.color.y = static_cast<float>(random() % 255) / 255.0f;
        pt.color.z = static_cast<float>(random() % 255) / 255.0f;
    }

    // step-4 �\�����郂�f��������������
    // �e�B�[�|�b�g���f����������
    ModelInitData teapotModelInitData;

    // ���[�U�[�g���f�[�^�Ƃ��ă|�C���g���C�g�̃��X�g��n��
    teapotModelInitData.m_expandConstantBuffer = pointLights;
    teapotModelInitData.m_expandConstantBufferSize = sizeof(pointLights);
    teapotModelInitData.m_tkmFilePath = "Assets/modelData/teapot.tkm";
    teapotModelInitData.m_fxFilePath = "Assets/shader/model.fx";
    Model teapotModel;
    teapotModel.Init(teapotModelInitData);

    // �w�i�̃��f����������
    ModelInitData bgModelInitData;

    // ���[�U�[�g���f�[�^�Ƃ��ă|�C���g���C�g�̃��X�g��n��
    bgModelInitData.m_expandConstantBuffer = pointLights;
    bgModelInitData.m_expandConstantBufferSize = sizeof(pointLights);
    bgModelInitData.m_tkmFilePath = "Assets/modelData/bg.tkm";
    bgModelInitData.m_fxFilePath = "Assets/shader/model.fx";
    Model bgModel;
    bgModel.Init(bgModelInitData);

    //////////////////////////////////////
    // ���������s���R�[�h�������̂͂����܂ŁI�I�I
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    Stopwatch sw;

    //  ��������Q�[�����[�v
    while (DispatchWindowMessage())
    {
        sw.Start();

        // �����_�����O�J�n
        g_engine->BeginFrame();

        //////////////////////////////////////
        // ��������G��`���R�[�h���L�q����
        //////////////////////////////////////

        // ste-5 �|�C���g���C�g�𖈃t���[����
        // ���C�g����
        Quaternion qRot;
        qRot.SetRotationDegY(1.0f);
        for (auto& pt : pointLights)
        {
            qRot.Apply(pt.position);
        }

        // step-6 ���f���̃h���[�R�[�������s����
        teapotModel.Draw(renderContext);
        bgModel.Draw(renderContext);

        /////////////////////////////////////////
        //�G��`���R�[�h�������̂͂����܂ŁI�I�I
        //////////////////////////////////////
        // �����_�����O�I��
        g_engine->EndFrame();
        sw.Stop();
        printf("fps = %0.2f\n", 1.0f / sw.GetElapsed());
    }

    ::FreeConsole();
    return 0;
}

// ���[�g�V�O�l�`���̏�����
void InitRootSignature(RootSignature& rs)
{
    rs.Init(D3D12_FILTER_MIN_MAG_MIP_LINEAR,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP);
}

/// <summary>
// / �W�����o�̓R���\�[����������
/// </summary>
void InitStandardIOConsole()
{
    ::AllocConsole();               // �R�}���h�v�����v�g���\�������
    freopen("CON", "w", stdout);    // �W���o�͂̊��蓖��

    auto fhandle = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT rc;

    rc.Top = 0;
    rc.Left = 0;
    rc.Bottom = 30;
    rc.Right = 30;
    ::SetConsoleWindowInfo(fhandle, TRUE, &rc);
}
