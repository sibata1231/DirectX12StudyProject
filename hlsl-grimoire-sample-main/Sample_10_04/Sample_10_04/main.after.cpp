#include "stdafx.h"
#include "system/system.h"

const int NUM_DIRECTIONAL_LIGHT = 4; // �f�B���N�V�������C�g�̐�

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
    DirectionalLight directionalLight[NUM_DIRECTIONAL_LIGHT];   // �f�B���N�V�������C�g
    Vector3 eyePos;                 // �J�����̈ʒu
    float specPow;                  // �X�y�L�����̍i��
    Vector3 ambinetLight;           // ����
};

// �֐��錾
void InitRootSignature(RootSignature& rs);
void CalcWeightsTableFromGaussian(float* weights, int numWeights, float sigma);

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

    RootSignature rs;
    InitRootSignature(rs);

    // step-1 �Q�[���V�[����`�悷�郌���_�����O�^�[�Q�b�g���쐬
    RenderTarget mainRenderTarget;
    mainRenderTarget.Create(
        1280,
        720,
        1,
        1,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_D32_FLOAT
    );

    // �w�i���f����������
    ModelInitData bgModelInitData;
    bgModelInitData.m_tkmFilePath = "Assets/modelData/bg/bg.tkm";
    bgModelInitData.m_fxFilePath = "Assets/shader/sample3D.fx";

    // �w�i���f����������
    Model bgModel;
    bgModel.Init(bgModelInitData);

    // �v���C���[���f����������
    // ���f���̏���������ݒ肷��
    ModelInitData plModelInitData;

    // tkm�t�@�C�����w�肷��
    plModelInitData.m_tkmFilePath = "Assets/modelData/sample.tkm";

    // �V�F�[�_�[�t�@�C�����w�肷��
    plModelInitData.m_fxFilePath = "Assets/shader/sample3D.fx";

    // �ݒ肵���������������ƂɃ��f��������������
    Model plModel;
    plModel.Init(plModelInitData);

    // step-2 �K�E�X�u���[�p�̏d�݃e�[�u�����v�Z����
    const int NUM_WEIGHTS = 8;

    // �e�[�u���̃T�C�Y�͂W
    float weights[NUM_WEIGHTS];

    // �d�݃e�[�u�����v�Z����
    CalcWeightsTableFromGaussian(
        weights,        // �d�݂̊i�[��
        NUM_WEIGHTS,    // �d�݃e�[�u���̃T�C�Y
        8.0f            // �{�P��B���̐��l���傫���Ȃ�ƃ{�P�������Ȃ�
    );

    // step-3 ���u���[�p�̃����_�����O�^�[�Q�b�g���쐬
    RenderTarget xBlurRenderTarget;
    xBlurRenderTarget.Create(
        640,    // �����̉𑜓x��mainRenderTarget�̕��̔����ɂ���
        720,    // ������mainRenderTarget�̍����Ɠ���
        1,
        1,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_D32_FLOAT
    );

    // step-4 ���u���[�p�̃X�v���C�g��������
    // ����������ݒ肷��
    SpriteInitData xBlurSpriteInitData;
    xBlurSpriteInitData.m_fxFilePath = "Assets/shader/samplePostEffect.fx";
    xBlurSpriteInitData.m_vsEntryPointFunc = "VSXBlur";
    xBlurSpriteInitData.m_psEntryPoinFunc = "PSBlur";

    // �X�v���C�g�̉𑜓x��xBlurRenderTarget�Ɠ���
    xBlurSpriteInitData.m_width = 640;
    xBlurSpriteInitData.m_height = 720;

    // �e�N�X�`����mainRenderTarget�̃J���[�o�b�t�@�[
    xBlurSpriteInitData.m_textures[0] = &mainRenderTarget.GetRenderTargetTexture();

    // ���[�U�[�g���̒萔�o�b�t�@�[�ɏd�݃e�[�u����ݒ肷��
    xBlurSpriteInitData.m_expandConstantBuffer = &weights;
    xBlurSpriteInitData.m_expandConstantBufferSize = sizeof(weights);

    // �������������Ƃɉ��u���[�p�̃X�v���C�g������������
    Sprite xBlurSprite;
    xBlurSprite.Init(xBlurSpriteInitData);

    // step-5 �c�u���[�p�̃����_�����O�^�[�Q�b�g���쐬
    RenderTarget yBlurRenderTarget;
    yBlurRenderTarget.Create(
        640,        // �����̉𑜓x��xBlurRenderTarget�̕��Ɠ���
        360,        // �c���̉𑜓x��xBlurRenderTarget�̍����̔���
        1,
        1,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_D32_FLOAT
    );

    // step-6 �c�u���[�p�̃X�v���C�g��������
    // ����������ݒ肷��
    SpriteInitData yBlurSpriteInitData;
    yBlurSpriteInitData.m_fxFilePath = "Assets/shader/samplePostEffect.fx";
    yBlurSpriteInitData.m_vsEntryPointFunc = "VSYBlur";
    yBlurSpriteInitData.m_psEntryPoinFunc = "PSBlur";

    // �X�v���C�g�̕��ƍ�����yBlurRenderTarget�Ɠ���
    yBlurSpriteInitData.m_width = 640;
    yBlurSpriteInitData.m_height = 360;

    // �e�N�X�`����xBlurRenderTarget�̃J���[�o�b�t�@�[
    yBlurSpriteInitData.m_textures[0] = &xBlurRenderTarget.GetRenderTargetTexture();

    // ���[�U�[�g���̒萔�o�b�t�@�[�ɏd�݃e�[�u����ݒ肷��
    yBlurSpriteInitData.m_expandConstantBuffer = &weights;
    yBlurSpriteInitData.m_expandConstantBufferSize = sizeof(weights);

    // �������������Ƃɏc�u���[�p�̃X�v���C�g������������
    Sprite yBlurSprite;
    yBlurSprite.Init(yBlurSpriteInitData);

    // step-7 �e�N�X�`����\��t���邽�߂̃X�v���C�g������������
    // �X�v���C�g�̏������I�u�W�F�N�g���쐬����
    SpriteInitData spriteInitData;

    // �e�N�X�`����yBlurRenderTarget�̃J���[�o�b�t�@�[
    spriteInitData.m_textures[0] = &yBlurRenderTarget.GetRenderTargetTexture();

    // �����_�����O�悪�t���[���o�b�t�@�[�Ȃ̂ŁA�𑜓x�̓t���[���o�b�t�@�[�Ɠ���
    spriteInitData.m_width = 1280;
    spriteInitData.m_height = 720;

    // �{�P�摜�����̂܂ܓ\��t���邾���Ȃ̂ŁA�ʏ��2D�`��̃V�F�[�_�[���w�肷��
    spriteInitData.m_fxFilePath = "Assets/shader/sample2D.fx";

    // �������I�u�W�F�N�g���g���āA�X�v���C�g������������
    Sprite copyToFrameBufferSprite;
    copyToFrameBufferSprite.Init(spriteInitData);

    //////////////////////////////////////
    // ���������s���R�[�h�������̂͂����܂ŁI�I�I
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    //  ��������Q�[�����[�v
    while (DispatchWindowMessage())
    {
        // 1�t���[���̊J�n
        g_engine->BeginFrame();

        //////////////////////////////////////
        // ��������G��`���R�[�h���L�q����
        //////////////////////////////////////

        // step-8 �����_�����O�^�[�Q�b�g��mainRenderTarget�ɕύX����

        // �����_�����O�^�[�Q�b�g�Ƃ��ė��p�ł���܂ő҂�
        renderContext.WaitUntilToPossibleSetRenderTarget(mainRenderTarget);

        // �����_�����O�^�[�Q�b�g��ݒ�
        renderContext.SetRenderTargetAndViewport(mainRenderTarget);

        // �����_�����O�^�[�Q�b�g���N���A
        renderContext.ClearRenderTargetView(mainRenderTarget);

        // step-9 mainRenderTarget�Ɋe�탂�f����`�悷��
        plModel.Draw(renderContext);
        bgModel.Draw(renderContext);

        // �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
        renderContext.WaitUntilFinishDrawingToRenderTarget(mainRenderTarget);

        // step-10 mainRenderTarget�ɕ`�悳�ꂽ�摜�ɉ��u���[��������
        // ���u���[�p�̃����_�����O�^�[�Q�b�g�ɕύX
        // �����_�����O�^�[�Q�b�g�Ƃ��ė��p�ł���܂ő҂�
        renderContext.WaitUntilToPossibleSetRenderTarget(xBlurRenderTarget);

        // �����_�����O�^�[�Q�b�g��ݒ�
        renderContext.SetRenderTargetAndViewport(xBlurRenderTarget);

        // �����_�����O�^�[�Q�b�g���N���A
        renderContext.ClearRenderTargetView(xBlurRenderTarget);

        // 2D��`��
        xBlurSprite.Draw(renderContext);

        // �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
        renderContext.WaitUntilFinishDrawingToRenderTarget(xBlurRenderTarget);

        // step-11 �c�u���[���s��
        // �c�u���[�p�̃����_�����O�^�[�Q�b�g�ɕύX
        // �����_�����O�^�[�Q�b�g�Ƃ��ė��p�ł���܂ő҂�
        renderContext.WaitUntilToPossibleSetRenderTarget(yBlurRenderTarget);

        // �����_�����O�^�[�Q�b�g��ݒ�
        renderContext.SetRenderTargetAndViewport(yBlurRenderTarget);

        // �����_�����O�^�[�Q�b�g���N���A
        renderContext.ClearRenderTargetView(yBlurRenderTarget);

        // 2D��`��
        yBlurSprite.Draw(renderContext);

        // �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
        renderContext.WaitUntilFinishDrawingToRenderTarget(yBlurRenderTarget);

        // step-12 ���C�������_�����O�^�[�Q�b�g�̊G���t���[���o�b�t�@�[�ɃR�s�[
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

/// <summary>
/// �K�E�V�A���֐��𗘗p���ďd�݃e�[�u�����v�Z����
/// </summary>
/// <param name="weightsTbl">�d�݃e�[�u���̋L�^��</param>
/// <param name="sizeOfWeightsTbl">�d�݃e�[�u���̃T�C�Y</param>
/// <param name="sigma">���U��B���̐��l���傫���Ȃ�ƕ��U��������Ȃ�</param>
void CalcWeightsTableFromGaussian(float* weightsTbl, int sizeOfWeightsTbl, float sigma)
{
    // �d�݂̍��v���L�^����ϐ����`����
    float total = 0;

    // ��������K�E�X�֐���p���ďd�݂��v�Z���Ă���
    // ���[�v�ϐ���x����e�N�Z������̋���
    for (int x = 0; x < sizeOfWeightsTbl; x++)
    {
        weightsTbl[x] = expf(-0.5f * (float)(x * x) / sigma);
        total += 2.0f * weightsTbl[x];
    }

    // �d�݂̍��v�ŏ��Z���邱�ƂŁA�d�݂̍��v��1�ɂ��Ă���
    for (int i = 0; i < sizeOfWeightsTbl; i++)
    {
        weightsTbl[i] /= total;
    }
}
