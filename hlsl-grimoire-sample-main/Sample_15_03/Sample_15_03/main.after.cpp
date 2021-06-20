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
    Bitmap imagebmp;
    imagebmp.Load("Assets/image/original.bmp");

    // step-2 �u�����f�B���O�W���𑗂邽�߂̒萔�o�b�t�@�[���쐬����
    ConstantBuffer weightsCB;
    weightsCB.Init(sizeof(float) * 8, nullptr);

    // step-3 �e�N�X�`�����𑗂邽�߂̒萔�o�b�t�@�[���쐬����
    TexInfo texInfo;
    texInfo.originalTexSize[0] = imagebmp.GetWidth();
    texInfo.originalTexSize[1] = imagebmp.GetHeight();
    texInfo.xBlurTexSize[0] = texInfo.originalTexSize[0] / 2;
    texInfo.xBlurTexSize[1] = texInfo.originalTexSize[1];
    texInfo.yBlurTexSize[0] = texInfo.originalTexSize[0] / 2;
    texInfo.yBlurTexSize[1] = texInfo.originalTexSize[1] / 2;
    ConstantBuffer texInfoCB;
    texInfoCB.Init(sizeof(texInfo), &texInfo);

    // step-4 �e��X�g���N�`���[�h�o�b�t�@�[���쐬����
    StructuredBuffer inputImageBmpSB;
    inputImageBmpSB.Init(
        imagebmp.GetPixelSizeInBytes(), // ��1������1��f�̃T�C�Y
        imagebmp.GetNumPixel(),         // �s�N�Z���̐����擾
        imagebmp.GetImageAddress()      // �摜�f�[�^�̐擪�A�h���X
    );

    // X�u���[���������摜���o�͂��邽�߂̓ǂݏ����\�\�����o�b�t�@�[���쐬
    RWStructuredBuffer outputXBlurImageRWSB;
    outputXBlurImageRWSB.Init(
        imagebmp.GetPixelSizeInBytes(), // ��1������1��f�̃T�C�Y
        imagebmp.GetNumPixel()/2,       // ��������1/2�̉𑜓x�փ_�E���T���v�����O���s���̂Ńs�N�Z�����𔼕��ɂ���
        nullptr
    );

    // Y�u���[���������摜���o�͂��邽�߂̓ǂݏ����\�\�����o�b�t�@�[���쐬
    RWStructuredBuffer outputYBlurImageRWSB;
    outputYBlurImageRWSB.Init(
        imagebmp.GetPixelSizeInBytes(), // ��1������1��f�̃T�C�Y
        imagebmp.GetNumPixel()/4,       // �c�A��������1/2�̉𑜓x�փ_�E���T���v�����O���s���̂Ńs�N�Z������1/4�ɂ���
        nullptr
    );
    // �ŏI���ʂ��o�͂��邽�߂̓ǂݏ����\�\�����o�b�t�@�[���쐬
    RWStructuredBuffer finalImageRWSB;
    finalImageRWSB.Init(
        imagebmp.GetPixelSizeInBytes(), // ��1������1��f�̃T�C�Y
        imagebmp.GetNumPixel(),
        nullptr
    );

    // step-5 �e��f�B�X�N���v�^�q�[�v���쐬����
    // X�u���[�p�̃f�B�X�N���v�^�q�[�v���쐬
    DescriptorHeap xBlurDS;
    xBlurDS.RegistShaderResource(0, inputImageBmpSB);
    xBlurDS.RegistConstantBuffer(0, weightsCB);
    xBlurDS.RegistConstantBuffer(1, texInfoCB);
    xBlurDS.RegistUnorderAccessResource(0, outputXBlurImageRWSB);
    xBlurDS.Commit();

    // Y�u���[�p�̃f�B�X�N���v�^�q�[�v���쐬
    DescriptorHeap yBlurDS;
    yBlurDS.RegistShaderResource(0, outputXBlurImageRWSB);
    yBlurDS.RegistConstantBuffer(0, weightsCB);
    yBlurDS.RegistConstantBuffer(1, texInfoCB);
    yBlurDS.RegistUnorderAccessResource(0, outputYBlurImageRWSB);
    yBlurDS.Commit();

    // �ŏI���ʏo�͗p�̃f�B�X�N���v�^�q�[�v���쐬
    DescriptorHeap finalDS;
    finalDS.RegistShaderResource(0, outputYBlurImageRWSB);
    finalDS.RegistConstantBuffer(0, weightsCB);
    finalDS.RegistConstantBuffer(1, texInfoCB);
    finalDS.RegistUnorderAccessResource(0, finalImageRWSB);
    finalDS.Commit();

    // step-6 �e��p�C�v���C���X�e�[�g���쐬����

    // X�u���[�p�̃p�C�v���C���X�e�[�g���쐬
    // X�u���[�p�̃R���s���[�g�V�F�[�_�����[�h����
    Shader xblurCS;
    xblurCS.LoadCS("Assets/shader/sample.fx", "XBlur");
    // �p�C�v���C���X�e�[�g���쐬����
    PipelineState xBlurPipelineState;
    InitPipelineState(rs, xBlurPipelineState, xblurCS);

    // Y�u���[�p�̃p�C�v���C���X�e�[�g���쐬
    // Y�u���[�p�̃R���s���[�g�V�F�[�_�̃��[�h
    Shader yblurCS;
    yblurCS.LoadCS("Assets/shader/sample.fx", "YBlur");
    // �p�C�v���C���X�e�[�g���쐬
    PipelineState yBlurPipelineState;
    InitPipelineState(rs, yBlurPipelineState, yblurCS);

    // �ŏI�o�͗p�̃p�C�v���C���X�e�[�g���쐬
    // �ŏI�o�͗p�̃R���s���[�g�V�F�[�_�[�̃��[�h
    Shader finalCS;
    finalCS.LoadCS("Assets/shader/sample.fx", "Final");
    // �p�C�v���C���X�e�[�g���쐬
    PipelineState finalPipelineState;
    InitPipelineState(rs, finalPipelineState, finalCS);

    //////////////////////////////////////
    // ���������s���R�[�h�������̂͂����܂ŁI�I�I
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    //  ��������Q�[�����[�v
    while (DispatchWindowMessage())
    {
        // �t���[���J�n
        g_engine->BeginFrame();

        // �J�E�X�֐����g���ďd�݂��v�Z���ăO���t�B�b�N�X�������ɓ]��
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
        weightsCB.CopyToVRAM(weights);

        //////////////////////////////////////
        // ��������DirectCompute�ւ̃f�B�X�p�b�`����
        //////////////////////////////////////
        renderContext.SetComputeRootSignature(rs);

        // step-8 �e��R���s���[�g�V�F�[�_�[���f�B�X�p�b�`
        // X�u���[���f�B�X�p�b�`
        renderContext.SetPipelineState(xBlurPipelineState);
        renderContext.SetComputeDescriptorHeap(xBlurDS);
        renderContext.Dispatch(texInfo.xBlurTexSize[0] / 4, texInfo.xBlurTexSize[1] / 4, 1);

        // Y�u���[���f�B�X�p�b�`
        renderContext.SetPipelineState(yBlurPipelineState);
        renderContext.SetComputeDescriptorHeap(yBlurDS);
        renderContext.Dispatch(texInfo.yBlurTexSize[0] / 4, texInfo.yBlurTexSize[1] / 4, 1);

        // �ŏI�������f�B�X�p�b�`
        renderContext.SetPipelineState(finalPipelineState);
        renderContext.SetComputeDescriptorHeap(finalDS);
        renderContext.Dispatch(texInfo.originalTexSize[0] / 4, texInfo.originalTexSize[1] / 4, 1);

        // �t���[���I��
        g_engine->EndFrame();

        // step-9 �摜���擾���A�ۑ�����
        imagebmp.Copy(finalImageRWSB.GetResourceOnCPU());
        imagebmp.Save("Assets/image/blur.bmp");

        MessageBox(nullptr, L"����", L"�ʒm", MB_OK);

        // �f�X�g���C
        DestroyWindow(g_hWnd);
    }
    return 0;
}
