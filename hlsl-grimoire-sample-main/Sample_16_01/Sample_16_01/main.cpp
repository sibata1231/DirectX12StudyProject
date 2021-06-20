#include "stdafx.h"
#include "system/system.h"
#include <random>
#include "util/stopwatch.h"

// �֐��錾
void InitRootSignature(RootSignature& rs);
void InitStandardIOConsole();

// step-1 �|�C���g���C�g�\���̂��`����

// step-2 �|�C���g���C�g�̐���\���萔���`����

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

    // step-4 �\�����郂�f��������������

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

        // step-5 �|�C���g���C�g�𖈃t���[����

        // step-6 ���f���̃h���[�R�[�������s����

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
