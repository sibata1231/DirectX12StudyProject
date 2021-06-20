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
    Bitmap imagebmp;
    imagebmp.Load("Assets/image/original.bmp");

    // step-2 �摜�f�[�^���O���t�B�b�N�X�������ɑ��邽�߂ɃX�g���N�`���[�h�o�b�t�@���쐬
    StructuredBuffer inputImageBmpSB;
    inputImageBmpSB.Init(
        imagebmp.GetPixelSizeInBytes(), // ��������1��f�̃T�C�Y
        imagebmp.GetNumPixel(),         // �s�N�Z���̐����擾
        imagebmp.GetImageAddress()      // �摜�f�[�^�̐擪�A�h���X
    );

    // step-3 ���m�N���������摜���󂯎�邽�߂�RW�X�g���N�`���o�b�t�@���쐬
    RWStructuredBuffer outputImageBmpRWSB;
    outputImageBmpRWSB.Init(
        imagebmp.GetPixelSizeInBytes(), // ��������1��f�̃T�C�Y
        imagebmp.GetNumPixel(),         // �s�N�Z���̐����擾
        imagebmp.GetImageAddress()      // �摜�f�[�^�̐擪�A�h���X
    );

    // step-4 ���̓f�[�^�Əo�̓f�[�^���f�B�X�N���v�^�q�[�v�ɓo�^����
    DescriptorHeap ds;
    ds.RegistShaderResource(0, inputImageBmpSB);
    ds.RegistUnorderAccessResource(0, outputImageBmpRWSB);
    ds.Commit();

    //�R���s���[�g�V�F�[�_�̃��[�h
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
        renderContext.SetComputeRootSignature(rs);
        renderContext.SetPipelineState(pipelineState);
        renderContext.SetComputeDescriptorHeap(ds);

        // �s�N�Z������512�~512 = 262,144�s�N�Z��
        // 4�̃X���b�h�𐶐�����R���s���[�g�V�F�[�_�\�Ȃ̂ŁA
        // 262,144 �� 4 = 65,536�̃X���b�h�O���[�v���쐬����
        renderContext.Dispatch(65536, 1, 1);

        // �t���[���I��
        g_engine->EndFrame();

        // step-6 ���m�N���ɂ����摜��ۑ�
        imagebmp.Copy(outputImageBmpRWSB.GetResourceOnCPU());
        imagebmp.Save("Assets/image/monochrome.bmp");

        MessageBox(nullptr, L"����", L"�ʒm", MB_OK);
        // �f�X�g���C
        DestroyWindow(g_hWnd);
    }
    return 0;
}
