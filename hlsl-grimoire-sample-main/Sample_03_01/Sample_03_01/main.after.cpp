#include "stdafx.h"
#include "system/system.h"
#include "TrianglePolygon.h"

// �֐��錾
void InitRootSignature(RootSignature& rs);

///////////////////////////////////////////////////////////////////
// �E�B���h�E�v���O�����̃��C���֐�
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    //�Q�[���̏�����
    InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));

    //////////////////////////////////////
    // �������珉�������s���R�[�h���L�q����
    //////////////////////////////////////

    // ���[�g�V�O�l�`�����쐬
    RootSignature rootSignature;
    InitRootSignature(rootSignature);

    // �O�p�`�|���S�����`
    TrianglePolygon triangle;
    triangle.Init(rootSignature);

    // step-1 �萔�o�b�t�@���쐬
    ConstantBuffer cb;
    cb.Init( sizeof( Matrix ) );

    // step-2 �f�B�X�N���v�^�q�[�v���쐬
    DescriptorHeap ds;
    ds.RegistConstantBuffer( 0, cb );   //�f�B�X�N���v�^�q�[�v�ɒ萔�o�b�t�@��o�^
    ds.Commit();                        //�f�B�X�N���v�^�q�[�v�ւ̓o�^���m��

    //////////////////////////////////////
    // ���������s���R�[�h�������̂͂����܂ŁI�I�I
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    // ��������Q�[�����[�v
    while (DispatchWindowMessage())
    {
        //�t���[���J�n
        g_engine->BeginFrame();

        //////////////////////////////////////
        //��������G��`���R�[�h���L�q����
        //////////////////////////////////////

        //���[�g�V�O�l�`����ݒ�
        renderContext.SetRootSignature(rootSignature);

        // step-3 ���[���h�s����쐬
        Matrix mWorld;
        mWorld.MakeTranslation(0.5f, 0.4f, 0.0f);
        cb.CopyToVRAM(mWorld);

        // step-4 �f�B�X�N���v�^�q�[�v��ݒ�
        renderContext.SetDescriptorHeap(ds);

        //�O�p�`���h���[
        triangle.Draw(renderContext);

        /// //////////////////////////////////////
        //�G��`���R�[�h�������̂͂����܂ŁI�I�I
        //////////////////////////////////////
        //�t���[���I��
        g_engine->EndFrame();
    }
    return 0;
}

//���[�g�V�O�l�`���̏�����
void InitRootSignature( RootSignature& rs )
{
    rs.Init(D3D12_FILTER_MIN_MAG_MIP_LINEAR,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP);
}
