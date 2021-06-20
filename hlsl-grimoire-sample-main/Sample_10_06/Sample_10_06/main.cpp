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
    DirectionalLight directionalLight[NUM_DIRECTIONAL_LIGHT]; // �f�B���N�V�������C�g
    Vector3 eyePos;         // �J�����̈ʒu
    float specPow;          // �X�y�L�����̍i��
    Vector3 ambinetLight;   // ����
};

const int NUM_WEIGHTS = 8;

/// <summary>
/// �u���[�p�̃p�����[�^
/// </summary>
struct SBlurParam
{
    float weights[NUM_WEIGHTS];
};

// �֐��錾
void InitRootSignature(RootSignature& rs);
void InitModel(Model& plModel);

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

    // 32�r�b�g�̕��������_�̃J���[�o�b�t�@�[��ێ��������C�������_�����O�^�[�Q�b�g���쐬����
    RenderTarget mainRenderTarget;
    mainRenderTarget.Create(
        1280,
        720,
        1,
        1,
        // �y���ځz�J���[�o�b�t�@�[�̃t�H�[�}�b�g��32bit���������_�ɂ��Ă���
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_D32_FLOAT
    );

    // �������̃��C�g��p�ӂ���
    Light light;

    // �������߂ɐݒ肷��
    light.directionalLight[0].color.x = 10.8f;
    light.directionalLight[0].color.y = 10.8f;
    light.directionalLight[0].color.z = 10.8f;

    light.directionalLight[0].direction.x = 0.0f;
    light.directionalLight[0].direction.y = 0.0f;
    light.directionalLight[0].direction.z = -1.0f;
    light.directionalLight[0].direction.Normalize();

    light.ambinetLight.x = 0.5f;
    light.ambinetLight.y = 0.5f;
    light.ambinetLight.z = 0.5f;
    light.eyePos = g_camera3D->GetPosition();

    // ���f���̏���������ݒ肷��
    ModelInitData plModelInitData;

    // tkm�t�@�C�����w�肷��
    plModelInitData.m_tkmFilePath = "Assets/modelData/sample.tkm";

    // �V�F�[�_�[�t�@�C�����w�肷��
    plModelInitData.m_fxFilePath = "Assets/shader/sample3D.fx";

    // ���[�U�[�g���̒萔�o�b�t�@�[�ɑ���f�[�^���w�肷��
    plModelInitData.m_expandConstantBuffer = &light;

    // ���[�U�[�g���̒萔�o�b�t�@�[�ɑ���f�[�^�̃T�C�Y���w�肷��
    plModelInitData.m_expandConstantBufferSize = sizeof(light);

    // �����_�����O����J���[�o�b�t�@�[�̃t�H�[�}�b�g���w�肷��
    plModelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;

    // �ݒ肵���������������ƂɃ��f��������������
    Model plModel;
    plModel.Init(plModelInitData);

    // �P�x���o�p�̃����_�����O�^�[�Q�b�g���쐬
    RenderTarget luminnceRenderTarget;

    // �𑜓x�A�~�b�v�}�b�v���x��
    luminnceRenderTarget.Create(
        1280,       // �𑜓x�̓��C�������_�����O�^�[�Q�b�g�Ɠ���
        720,        // �𑜓x�̓��C�������_�����O�^�[�Q�b�g�Ɠ���
        1,
        1,
        // �y���ځz�J���[�o�b�t�@�[�̃t�H�[�}�b�g��32bit���������_�ɂ��Ă���
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_D32_FLOAT
    );

    // �P�x���o�p�̃X�v���C�g��������
    // �����������쐬����
    SpriteInitData luminanceSpriteInitData;

    // �P�x���o�p�̃V�F�[�_�[�̃t�@�C���p�X���w�肷��
    luminanceSpriteInitData.m_fxFilePath = "Assets/shader/samplePostEffect.fx";

    // ���_�V�F�[�_�[�̃G���g���[�|�C���g���w�肷��
    luminanceSpriteInitData.m_vsEntryPointFunc = "VSMain";

    // �s�N�Z���V�F�[�_�[�̃G���g���[�|�C���g���w�肷��
    luminanceSpriteInitData.m_psEntryPoinFunc = "PSSamplingLuminance";

    // �X�v���C�g�̕��ƍ�����luminnceRenderTarget�Ɠ���
    luminanceSpriteInitData.m_width = 1280;
    luminanceSpriteInitData.m_height = 720;

    // �e�N�X�`���̓��C�������_�����O�^�[�Q�b�g�̃J���[�o�b�t�@�[
    luminanceSpriteInitData.m_textures[0] = &mainRenderTarget.GetRenderTargetTexture();

    // �`�����ރ����_�����O�^�[�Q�b�g�̃t�H�[�}�b�g���w�肷��
    luminanceSpriteInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;

    // �쐬�����������������ƂɃX�v���C�g������������
    Sprite luminanceSprite;
    luminanceSprite.Init( luminanceSpriteInitData );

    // step-1 �K�E�V�A���u���[��������

    // step-2 �{�P�摜���������ď������ނ��߂̃X�v���C�g��������

    // mainRenderTarget�̃e�N�X�`�����t���[���o�b�t�@�[�ɓ\��t���邽�߂̃X�v���C�g������������
    // �X�v���C�g�̏������I�u�W�F�N�g���쐬����
    SpriteInitData spriteInitData;

    // �e�N�X�`����mainRenderTarget�̃J���[�o�b�t�@�[
    spriteInitData.m_textures[0] = &mainRenderTarget.GetRenderTargetTexture();
    spriteInitData.m_width = 1280;
    spriteInitData.m_height = 720;

    // ���m�N���p�̃V�F�[�_�[���w�肷��
    spriteInitData.m_fxFilePath = "Assets/shader/sample2D.fx";

    // �������I�u�W�F�N�g���g���āA�X�v���C�g������������
    Sprite copyToFrameBufferSprite;
    copyToFrameBufferSprite.Init(spriteInitData);

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
        // ��������G��`���R�[�h���L�q����
        //////////////////////////////////////

        // �����_�����O�^�[�Q�b�g��mainRenderTarget�ɕύX����
        // �����_�����O�^�[�Q�b�g�Ƃ��ė��p�ł���܂ő҂�
        renderContext.WaitUntilToPossibleSetRenderTarget(mainRenderTarget);
        // �����_�����O�^�[�Q�b�g��ݒ�
        renderContext.SetRenderTargetAndViewport(mainRenderTarget);
        // �����_�����O�^�[�Q�b�g���N���A
        renderContext.ClearRenderTargetView(mainRenderTarget);

        // mainRenderTarget�Ɋe�탂�f����`�悷��
        plModel.Draw(renderContext);

        // �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
        renderContext.WaitUntilFinishDrawingToRenderTarget(mainRenderTarget);

        // �P�x���o
        // �P�x���o�p�̃����_�����O�^�[�Q�b�g�ɕύX
        renderContext.WaitUntilToPossibleSetRenderTarget(luminnceRenderTarget);

        // �����_�����O�^�[�Q�b�g��ݒ�
        renderContext.SetRenderTargetAndViewport(luminnceRenderTarget);

        // �����_�����O�^�[�Q�b�g���N���A
        renderContext.ClearRenderTargetView(luminnceRenderTarget);

        // �P�x���o���s��
        luminanceSprite.Draw(renderContext);

        // �����_�����O�^�[�Q�b�g�ւ̏������ݏI���҂�
        renderContext.WaitUntilFinishDrawingToRenderTarget(luminnceRenderTarget);

        // step-3 �K�E�V�A���u���[��4����s����

        // step-4 �{�P�摜���������ă��C�������_�����O�^�[�Q�b�g�ɉ��Z����

        // ���C�������_�����O�^�[�Q�b�g�̊G���t���[���o�b�t�@�[�ɃR�s�[
        renderContext.SetRenderTarget(
            g_graphicsEngine->GetCurrentFrameBuffuerRTV(),
            g_graphicsEngine->GetCurrentFrameBuffuerDSV()
        );
        copyToFrameBufferSprite.Draw(renderContext);

        // ���C�g�̋�����ύX����
        light.directionalLight[0].color.x += g_pad[0]->GetLStickXF() * 0.5f;
        light.directionalLight[0].color.y += g_pad[0]->GetLStickXF() * 0.5f;
        light.directionalLight[0].color.z += g_pad[0]->GetLStickXF() * 0.5f;
        //////////////////////////////////////
        //�G��`���R�[�h�������̂͂����܂ŁI�I�I
        //////////////////////////////////////

        // 1�t���[���I��
        g_engine->EndFrame();
    }
    return 0;
}

// ���[�g�V�O�l�`���̏�����
void InitRootSignature( RootSignature& rs )
{
    rs.Init(D3D12_FILTER_MIN_MAG_MIP_LINEAR,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP);
}
