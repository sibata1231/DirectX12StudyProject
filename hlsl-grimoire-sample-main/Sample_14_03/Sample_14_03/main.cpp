#include "stdafx.h"
#include "system/system.h"
#include "RenderingEngine.h"
#include "ModelRender.h"

// �֐��錾
void InitRootSignature(RootSignature& rs);


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
    
    // ���[�g�V�O�l�`�����쐬
    RootSignature rootSignature;
    InitRootSignature(rootSignature);

    //�����_�����O�p�C�v���C����������
    
    myRenderer::RenderingEngine renderingEngine;
    renderingEngine.Init();

    // �w�i���f���̃����_���[��������
    myRenderer::ModelRender bgModelRender;
    bgModelRender.InitDeferredRendering(renderingEngine, "Assets/modelData/bg/bg.tkm", true);
    
    // �e�B�[�|�b�g���f���̃����_���[��������
    myRenderer::ModelInitDataFR modelInitData;
    modelInitData.m_tkmFilePath = "Assets/modelData/teapot.tkm";
    modelInitData.m_fxFilePath = "Assets/shader/sample.fx";
    modelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
    modelInitData.m_expandShaderResoruceView[0] = &renderingEngine.GetMainRenderTargetSnapshotDrawnOpacity();
    myRenderer::ModelRender teapotModelRender;
    teapotModelRender.InitForwardRendering(renderingEngine, modelInitData);
    teapotModelRender.SetShadowCasterFlag(true);

    teapotModelRender.UpdateWorldMatrix({ 0.0f, 20.0f, 0.0f }, g_quatIdentity, g_vec3One);


    //////////////////////////////////////
    // ���������s���R�[�h�������̂͂����܂ŁI�I�I
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    // ��������Q�[�����[�v
    while (DispatchWindowMessage())
    {
        // �����_�����O�J�n
        g_engine->BeginFrame();
        g_camera3D->MoveForward(g_pad[0]->GetLStickYF());
        g_camera3D->MoveRight(g_pad[0]->GetLStickXF());
        g_camera3D->MoveUp(g_pad[0]->GetRStickYF());
        
        //////////////////////////////////////
        // ��������G��`���R�[�h���L�q����
        //////////////////////////////////////

        bgModelRender.Draw();
        teapotModelRender.Draw();
        //�����_�����O�p�C�v���C�������s
        renderingEngine.Execute(renderContext);
        

        /////////////////////////////////////////
        // �G��`���R�[�h�������̂͂����܂ŁI�I�I
        //////////////////////////////////////
        // �����_�����O�I��
        g_engine->EndFrame();
    }

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
