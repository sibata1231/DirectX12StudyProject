#include "stdafx.h"
#include "system/system.h"
#include "Bitmap.h"
#include "sub.h"

// �\���̒�`
// �e�N�X�`�����p�̒萔�o�b�t�@�[
struct TexInfo
{
    int originalTexSize[2]; // �I���W�i���e�N�X�`���̃T�C�Y
    int xBlurTexSize[2];    // X�u���[�̏o�͐�̃e�N�X�`���̃T�C�Y
    int yBlurTexSize[2];    // Y�u���[�̏o�͐�̃e�N�X�`���̃T�C�Y
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

    // ���[�g�V�O�l�`���̏�����
    RootSignature rs;
    InitRootSignature(rs);

    // step-1 �摜�f�[�^�����C����������Ƀ��[�h����

    // step-2 �u�����f�B���O�W���𑗂邽�߂̒萔�o�b�t�@�[���쐬����

    // step-3 �e�N�X�`�����𑗂邽�߂̒萔�o�b�t�@�[���쐬����

    // step-4 �e��\�����o�b�t�@�[���쐬����

    // step-5 �e��f�B�X�N���v�^�q�[�v���쐬����

    // step-6 �e��p�C�v���C���X�e�[�g���쐬����

    //////////////////////////////////////
    // ���������s���R�[�h�������̂͂����܂ŁI�I�I
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    // ��������Q�[�����[�v
    while (DispatchWindowMessage())
    {
        // �t���[���J�n
        g_engine->BeginFrame();

        // �K�E�X�֐����g���ďd�݂��v�Z���A�O���t�B�b�N�X�������ɓ]��
        float total = 0;
        float weights[8];
        for (int i = 0; i < 8; i++)
        {
            weights[i] = expf(-0.5f * (float)(i * i) / 100.0f);
            if (i == 0)
            {
                total += weights[i];
            }
            else
            {
                total += 2.0f * weights[i];
            }
        }
        // �K�i��
        for (int i = 0; i < 8; i++)
        {
            weights[i] /= total;
        }

        // step-7 �d�݃e�[�u����VRAM�ɓ]��

        //////////////////////////////////////
        // ��������DirectCompute�ւ̃f�B�X�p�b�`����
        //////////////////////////////////////
        renderContext.SetComputeRootSignature(rs);

        // step-8 �e��R���s���[�g�V�F�[�_�[���f�B�X�p�b�`

        // �t���[���I��
        g_engine->EndFrame();

        // step-9 �摜���擾���A�ۑ�����

        MessageBox(nullptr, L"����", L"�ʒm", MB_OK);
        // �f�X�g���C
        DestroyWindow(g_hWnd);
    }
    return 0;
}
