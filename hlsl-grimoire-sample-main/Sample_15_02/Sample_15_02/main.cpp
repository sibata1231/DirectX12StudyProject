#include "stdafx.h"
#include "system/system.h"
#include "Bitmap.h"
#include "sub.h"


///////////////////////////////////////////////////////////////////
// �E�B���h�E�v���O�����̃��C���֐�
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    // �Q�[���̏�����
    InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));

    //////////////////////////////////////
    //  �������珉�������s���R�[�h���L�q����
    //////////////////////////////////////

    // step-1 �摜�f�[�^�����C����������Ƀ��[�h����

    // step-2 �摜�f�[�^���O���t�B�b�N�X�������ɑ��邽�߂ɍ\�����o�b�t�@�[���쐬

    // step-3 ���m�N���������摜���󂯎�邽�߂̓ǂݏ����\�ȍ\�����o�b�t�@�[���쐬

    // step-4 ���̓f�[�^�Əo�̓f�[�^���f�B�X�N���v�^�q�[�v�ɓo�^����

    // �R���s���[�g�V�F�[�_�[�̃��[�h
    Shader cs;
    cs.LoadCS("Assets/shader/sample.fx", "CSMain");

    RootSignature rs;
    InitRootSignature(rs, cs);

    PipelineState pipelineState;
    InitPipelineState(rs, pipelineState, cs);

    //////////////////////////////////////
    // ���������s���R�[�h�������̂͂����܂ŁI�I�I
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    // ��������Q�[�����[�v
    while (DispatchWindowMessage())
    {
        // �t���[���J�n
        g_engine->BeginFrame();

        //////////////////////////////////////
        // ��������DirectCompute�ւ̃f�B�X�p�b�`����
        //////////////////////////////////////
        // step-5 �f�B�X�p�b�`�R�[�������s����

        // �t���[���I��
        g_engine->EndFrame();

        // step-6 ���m�N���ɂ����摜��ۑ�

        MessageBox(nullptr, L"����", L"�ʒm", MB_OK);
        // �f�X�g���C
        DestroyWindow(g_hWnd);
    }
    return 0;
}
