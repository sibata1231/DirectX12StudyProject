#include "stdafx.h"
#include "system/system.h"

// �֐��錾
void InitRootSignature(RootSignature& rs);
void InitPipelineState(PipelineState& pipelineState, RootSignature& rs, Shader& vs, Shader& ps);

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

    // step-1 �I�t�X�N���[���`��p�̃����_�����O�^�[�Q�b�g���쐬
    // RenderTarget�N���X�̃I�u�W�F�N�g���`
    RenderTarget offscreenRenderTarget;

    // RenderTarget::Create()�𗘗p���āA�����_�����O�^�[�Q�b�g���쐬����
    offscreenRenderTarget.Create(
        1280,                       // �e�N�X�`���̕�
        720,                        // �e�N�X�`���̍���
        1,                          // Mipmap���x��
        1,                          // �e�N�X�`���z��̃T�C�Y
        DXGI_FORMAT_R8G8B8A8_UNORM, // �J���[�o�b�t�@�̃t�H�[�}�b�g
        DXGI_FORMAT_D32_FLOAT       // �f�v�X�X�e���V���o�b�t�@�̃t�H�[�}�b�g
    );

    // step-2 �e�탂�f��������������
    // �����f��������������
    ModelInitData boxModelInitData;
    boxModelInitData.m_tkmFilePath = "Assets/modelData/box.tkm";
    boxModelInitData.m_fxFilePath = "Assets/shader/sample3D.fx";
    Model boxModel;
    boxModel.Init(boxModelInitData);
    boxModel.UpdateWorldMatrix({ 100.0f, 0.0f, 0.0f }, g_quatIdentity, g_vec3One);

    // �w�i���f����������
    ModelInitData bgModelInitData;
    bgModelInitData.m_tkmFilePath = "Assets/modelData/bg/bg.tkm";
    bgModelInitData.m_fxFilePath = "Assets/shader/sample3D.fx";

    // �w�i���f����������
    Model bgModel;
    bgModel.Init(bgModelInitData);

    // �v���C���[���f����������
    ModelInitData plModelInitData;
    plModelInitData.m_tkmFilePath = "Assets/modelData/sample.tkm";
    plModelInitData.m_fxFilePath = "Assets/shader/sample3D.fx";
    Model plModel;
    plModel.Init(plModelInitData);

    // step-3 �����f���ɓ\��t����e�N�X�`����ύX����
    // �����f���̃e�N�X�`�����I�t�X�N���[�������_�����O�����e�N�X�`���ɐ؂�ւ���
    boxModel.ChangeAlbedoMap(
        "",
        offscreenRenderTarget.GetRenderTargetTexture()
    );

    Vector3 plPos;

    //////////////////////////////////////
    // ���������s���R�[�h�������̂͂����܂ŁI�I�I
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    //  ��������Q�[�����[�v
    while (DispatchWindowMessage())
    {
        // 1�t���[���̊J�n
        g_engine->BeginFrame();

        // �v���C���[�̍��W���Q�[���p�b�h���g���ē�����
        plPos.x -= g_pad[0]->GetLStickXF();
        plPos.z -= g_pad[0]->GetLStickYF();

        // �v���C���[�̃��[���h�s����X�V����
        plModel.UpdateWorldMatrix(plPos, g_quatIdentity, g_vec3One);

        //////////////////////////////////////
        //��������G��`���R�[�h���L�q����
        //////////////////////////////////////

        // step-4 �����_�����O�^�[�Q�b�g��offscreenRenderTarget�ɕύX����
        RenderTarget* rtArray[] = { &offscreenRenderTarget };

        // �����_�����O�^�[�Q�b�g�Ƃ��ė��p�ł���܂ő҂�
        renderContext.WaitUntilToPossibleSetRenderTargets(1, rtArray);

        // �����_�����O�^�[�Q�b�g��ݒ�
        renderContext.SetRenderTargets(1, rtArray);

        // �����_�����O�^�[�Q�b�g���N���A
        renderContext.ClearRenderTargetViews(1, rtArray);

        // step-5 offscreenRenderTarget�ɔw�i�A�v���C���[��`�悷��
        // �w�i���f�����h���[
        bgModel.Draw(renderContext);

        // �v���C���[���h���[
        plModel.Draw(renderContext);

        // �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
        renderContext.WaitUntilFinishDrawingToRenderTargets(1, rtArray);

        // step-6 ��ʂɕ\������郌���_�����O�^�[�Q�b�g�ɖ߂�
        renderContext.SetRenderTarget(
            g_graphicsEngine->GetCurrentFrameBuffuerRTV(),
            g_graphicsEngine->GetCurrentFrameBuffuerDSV()
        );

        // step-7 ��ʂɕ\������郌���_�����O�^�[�Q�b�g�Ɋe�탂�f����`�悷��
        // �w�i���f�����h���[
        bgModel.Draw(renderContext);

        // �v���C���[���h���[
        plModel.Draw(renderContext);

        // ����`��
        boxModel.Draw(renderContext);

        //////////////////////////////////////
        //�G��`���R�[�h�������̂͂����܂ŁI�I�I
        //////////////////////////////////////
        // 1�t���[���I��
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

// �p�C�v���C���X�e�[�g�̏�����
void InitPipelineState(PipelineState& pipelineState, RootSignature& rs, Shader& vs, Shader& ps)
{
    //  ���_���C�A�E�g���`����
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    // �p�C�v���C���X�e�[�g���쐬
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { 0 };
    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.pRootSignature = rs.Get();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(vs.GetCompiledBlob());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(ps.GetCompiledBlob());
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pipelineState.Init(psoDesc);
}
