#include "stdafx.h"
#include "system/system.h"
#include "sub.h"

// �֐��錾
void InitRootSignature(RootSignature& rs);
void InitPipelineState(PipelineState& pipelineState, RootSignature& rs, Shader& vs, Shader& ps);

///////////////////////////////////////////////////////////////////
// �E�B���h�E�v���O�����̃��C���֐�
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    srand((unsigned int)time(nullptr));
    // �Q�[���̏�����
    InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));

    //////////////////////////////////////
    // �������珉�������s���R�[�h���L�q����
    //////////////////////////////////////

    // ���C�������_�����O�^�[�Q�b�g�Ɛ[�x�����_�����O�^�[�Q�b�g���쐬
    RenderTarget mainRenderTarget, depthRenderTarget;;
    InitMainDepthRenderTarget(mainRenderTarget, depthRenderTarget);

    // step-1 �e�탌���_�����O�^�[�Q�b�g������������

    // step-2 �����A�Ίp���u���[�������邽�߂̃X�v���C�g��������

    // step-3 �Z�p�`�u���[�������邽�߂̃X�v���C�g��������

    // �{�P�摜�����C�������_�����O�^�[�Q�b�g�ɍ������邽�߂̂̃X�v���C�g������������
    Sprite combineBokeImageSprite;
    InitCombimeBokeImageToSprite(
        combineBokeImageSprite,                     // �����������X�v���C�g
        rtPhomboidBlur.GetRenderTargetTexture(),    // �{�P�e�N�X�`��
        depthRenderTarget.GetRenderTargetTexture()  // �[�x�e�N�X�`��
    );

    // ���C�������_�����O�^�[�Q�b�g�̊G���t���[���o�b�t�@�ɃR�s�[���邽�߂̃X�v���C�g��������
    // �X�v���C�g�̏������I�u�W�F�N�g���쐬����
    Sprite copyToFrameBufferSprite;
    InitCopyToFrameBufferTargetSprite(
        copyToFrameBufferSprite,                    // �����������X�v���C�g
        mainRenderTarget.GetRenderTargetTexture()   // ���C�������_�����O�^�[�Q�b�g�̃e�N�X�`��
    );

    // �w�i���f����������
    Light light;
    Model model;
    InitBGModel(model, light);

    // ���̃��f����������
    Light sphereLight[50];
    Model sphereModels[50];
    InitSphereModels(sphereModels, sphereLight, ARRAYSIZE(sphereModels));

    //////////////////////////////////////
    // ���������s���R�[�h�������̂͂����܂ŁI�I�I
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    // ��������Q�[�����[�v
    while (DispatchWindowMessage())
    {
        // 1�t���[���̊J�n
        g_engine->BeginFrame();

        //////////////////////////////////////
        // �G��`���R�[�h�������̂͂����܂ŁI�I�I
        //////////////////////////////////////
        MoveCamera();

        // 2���̃����_�����O�^�[�Q�b�g��ݒ肵�āA���f����`�悷��
        // 2���̃����_�����O�^�[�Q�b�g�̃|�C���^�����z����`����
        RenderTarget* rts[] = {
            &mainRenderTarget,
            &depthRenderTarget
        };

        // �����_�����O�^�[�Q�b�g�Ƃ��ė��p�ł���܂ő҂�
        renderContext.WaitUntilToPossibleSetRenderTargets(2, rts);

        // �����_�����O�^�[�Q�b�g��ݒ�
        renderContext.SetRenderTargetsAndViewport(2, rts);

        // �����_�����O�^�[�Q�b�g���N���A
        renderContext.ClearRenderTargetViews(2, rts);

        // ���f�����h���[
        model.Draw(renderContext);
        for (auto& sphereModel : sphereModels) {
            sphereModel.Draw(renderContext);
        }

        // �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
        renderContext.WaitUntilFinishDrawingToRenderTargets(2, rts);

        // step-4 �����A�Ίp���u���[��������

        // step-5 �Z�p�`�u���[��������

        // �{�P�摜�Ɛ[�x�e�N�X�`���𗘗p���āA�{�P�摜��`������ł���
        // ���C�������_�����O�^�[�Q�b�g��ݒ�
        renderContext.WaitUntilToPossibleSetRenderTarget(mainRenderTarget);
        renderContext.SetRenderTargetAndViewport(mainRenderTarget);

        // �X�v���C�g��`��
        combineBokeImageSprite.Draw(renderContext);

        // �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
        renderContext.WaitUntilFinishDrawingToRenderTarget(mainRenderTarget);

        // ���C�������_�����O�^�[�Q�b�g�̊G���t���[���o�b�t�@�[�ɃR�s�[
        renderContext.SetRenderTarget(
            g_graphicsEngine->GetCurrentFrameBuffuerRTV(),
            g_graphicsEngine->GetCurrentFrameBuffuerDSV()
        );

        // �r���[�|�[�g���w�肷��
        D3D12_VIEWPORT viewport;
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = 1280;
        viewport.Height = 720;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        renderContext.SetViewportAndScissor(viewport);
        copyToFrameBufferSprite.Draw(renderContext);

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

    // ���_���C�A�E�g���`����
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
