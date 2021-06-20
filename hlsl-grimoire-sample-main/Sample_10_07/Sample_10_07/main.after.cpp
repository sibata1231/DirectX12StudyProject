#include "stdafx.h"
#include "system/system.h"
#include "sub.h"

const int NUM_DIRECTIONAL_LIGHT = 4;

/// <summary>
/// �f�B���N�V�������C�g
/// </summary>
struct DirectionalLight
{
    Vector3 direction;  // ���C�g�̕���
    float pad0;         // �p�f�B���O
    Vector4 color;      // ���C�g�̃J���[
};

/// <summary>
/// ���C�g�\����
/// </summary>
struct Light
{
    DirectionalLight directionalLight[NUM_DIRECTIONAL_LIGHT]; // �f�B���N�V�������C�g
    Vector3 eyePos;         // �J�����̈ʒu
    float specPow;          // �X�y�L�����̍i��
    Vector3 ambinetLight;   // ����
};

// �֐��錾
void InitRootSignature(RootSignature& rs);
void InitPipelineState(PipelineState& pipelineState, RootSignature& rs, Shader& vs, Shader& ps);
void InitModel(Model& model, Light& light);

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

    Light light;

    // step-1 ���C�������_�����O�^�[�Q�b�g�Ɛ[�x�����_�����O�^�[�Q�b�g���쐬
    // �V�[���̃J���[��`�����ރ��C�������_�����O�^�[�Q�b�g���쐬
    RenderTarget mainRenderTarget;
    mainRenderTarget.Create(
        1280,
        720,
        1,
        1,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_D32_FLOAT
    );

    //�V�[���̃J������Ԃł�Z�l���������ރ����_�����O�^�[�Q�b�g���쐬
    RenderTarget depthRenderTarget;
    depthRenderTarget.Create(
        1280,
        720,
        1,
        1,
        DXGI_FORMAT_R32_FLOAT,
        DXGI_FORMAT_UNKNOWN
    );

    // step-2 �V�[���e�N�X�`�����ڂ������߂̃K�E�V�A���u���[�I�u�W�F�N�g��������
    GaussianBlur blur;
    blur.Init(&mainRenderTarget.GetRenderTargetTexture());

    // step-3 �{�P�摜�����p�̃X�v���C�g������������
    SpriteInitData combineBokeImageSpriteInitData;

    // �g�p����e�N�X�`����2��
    combineBokeImageSpriteInitData.m_textures[0] = &blur.GetBokeTexture();
    combineBokeImageSpriteInitData.m_textures[1] = &depthRenderTarget.GetRenderTargetTexture();
    combineBokeImageSpriteInitData.m_width = 1280;
    combineBokeImageSpriteInitData.m_height = 720;
    combineBokeImageSpriteInitData.m_fxFilePath = "Assets/shader/samplePostEffect.fx";
    combineBokeImageSpriteInitData.m_colorBufferFormat = {DXGI_FORMAT_R32G32B32A32_FLOAT};

    // �����𗘗p���ă{�P�摜���A���t�@�u�����f�B���O����̂ŁA�������������[�h�ɂ���
    combineBokeImageSpriteInitData.m_alphaBlendMode = AlphaBlendMode_Trans;

    // �������I�u�W�F�N�g�𗘗p���ăX�v���C�g������������
    Sprite combineBokeImageSprite;
    combineBokeImageSprite.Init(combineBokeImageSpriteInitData);

    // ���C�������_�����O�^�[�Q�b�g�̊G���t���[���o�b�t�@�ɃR�s�[���邽�߂̃X�v���C�g��������
    // �X�v���C�g�̏������I�u�W�F�N�g���쐬����
    SpriteInitData spriteInitData;

    // �e�N�X�`����yBlurRenderTarget�̃J���[�o�b�t�@�[
    spriteInitData.m_textures[0] = &mainRenderTarget.GetRenderTargetTexture();

    // �����_�����O�悪�t���[���o�b�t�@�[�Ȃ̂ŁA�𑜓x�̓t���[���o�b�t�@�[�Ɠ���
    spriteInitData.m_width = 1280;
    spriteInitData.m_height = 720;

    // �{�P�摜�����̂܂ܓ\��t���邾���Ȃ̂ŁA�ʏ��2D�`��̃V�F�[�_�[���w�肷��
    spriteInitData.m_fxFilePath = "Assets/shader/preset/sample2D.fx";

    // �������I�u�W�F�N�g���g���āA�X�v���C�g������������
    Sprite copyToFrameBufferSprite;
    copyToFrameBufferSprite.Init(spriteInitData);

    //���f����������
    Model model;
    InitModel(model, light);

    //////////////////////////////////////
    // ���������s���R�[�h�������̂͂����܂ŁI�I�I
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    // ��������Q�[�����[�v
    while (DispatchWindowMessage())
    {
        // 1�t���[���̊J�n
        g_engine->BeginFrame();

        //�J�����𓮂���
        MoveCamera();

        // step-4 2���̃����_�����O�^�[�Q�b�g��ݒ肵�āA���f����`�悷��
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

        // �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
        renderContext.WaitUntilFinishDrawingToRenderTargets(2, rts);

        // step-5 ���C�������_�����O�^�[�Q�b�g�̃{�P�摜���쐬
        blur.ExecuteOnGPU(renderContext, 5);

        // step-6 �{�P�摜�Ɛ[�x�e�N�X�`���𗘗p���āA�{�P�摜��`������ł���
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

        //////////////////////////////////////
        // �G��`���R�[�h�������̂͂����܂ŁI�I�I
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

void InitModel(Model& model, Light& light)
{
    // �������߂ɐݒ肷��
    light.directionalLight[0].color.x = 2.0f;
    light.directionalLight[0].color.y = 2.0f;
    light.directionalLight[0].color.z = 2.0f;

    light.directionalLight[0].direction.x = 0.0f;
    light.directionalLight[0].direction.y = 0.0f;
    light.directionalLight[0].direction.z = -1.0f;
    light.directionalLight[0].direction.Normalize();

    light.ambinetLight.x = 0.5f;
    light.ambinetLight.y = 0.5f;
    light.ambinetLight.z = 0.5f;

    light.eyePos = g_camera3D->GetPosition();

    ModelInitData modelInitData;
    modelInitData.m_tkmFilePath = "Assets/modelData/bg/bg.tkm";
    modelInitData.m_fxFilePath = "Assets/shader/sample3D.fx";
    modelInitData.m_expandConstantBuffer = &light;
    modelInitData.m_expandConstantBufferSize = sizeof(light);
    modelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
    modelInitData.m_colorBufferFormat[1] = DXGI_FORMAT_R32_FLOAT;

    model.Init(modelInitData);
}
