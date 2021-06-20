#include "stdafx.h"
#include "system/system.h"
#include <random>
#include "util/stopwatch.h"

//�֐��錾
void InitRootSignature(RootSignature& rs);
void InitStandardIOConsole();

// �f�B���N�V�������C�g�\����
struct alignas(16) DirectionalLight
{
    Vector3  color;     // ���C�g�̃J���[
    float pad0;
    Vector3  direction; // ���C�g�̕���
};

// �|�C���g���C�g�\����
struct alignas(16) PointLight
{
    Vector3 position;   // ���W
    float pad0;
    Vector3 color;      // ���C�g�̃J���[
    float range;        // ���C�g�̉e����^����͈�
};

const int NUM_POINT_LIGHT = 1000;   // �|�C���g���C�g�̐�
const int NUM_DIRECTION_LIGHT = 4;  // �f�B���N�V�������C�g�̐�

// ���C�g�\����
struct Light
{
    DirectionalLight directionLights[ NUM_DIRECTION_LIGHT]; // �f�B���N�V�������C�g
    PointLight pointLights[NUM_POINT_LIGHT];                // �|�C���g���C�g
    Matrix mViewProjInv;    // �r���[�v���W�F�N�V�����s��̋t�s��
    Vector3 eyePos;         // ���_
    float specPow;          // �X�y�L�����̍i��
};

///////////////////////////////////////////////////////////////////
//  �E�B���h�E�v���O�����̃��C���֐�
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    // �Q�[���̏�����
    InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));

    // �W�����o�̓R���\�[���̏�����
    InitStandardIOConsole();

    //////////////////////////////////////
    // �������珉�������s���R�[�h���L�q����
    //////////////////////////////////////
    std::random_device seed_gen;
    std::mt19937 random(seed_gen());

    g_camera3D->SetPosition({ 0.0f, 200.0, 400.0f });
    g_camera3D->Update();

    // ���[�g�V�O�l�`�����쐬
    RootSignature rootSignature;
    InitRootSignature(rootSignature);

    Light light;

    // ���C�g�̋��ʃp�����[�^��ݒ�
    light.eyePos = g_camera3D->GetPosition();
    light.specPow = 5.0f;
    light.mViewProjInv.Inverse(g_camera3D->GetViewProjectionMatrix());

    // �f�B���N�V�������C�g��������
    light.directionLights[0].direction.Set(1.0f, 0.0f, 0.0f);
    light.directionLights[0].color.Set(0.5f, 0.5f, 0.5f);

    light.directionLights[1].direction.Set(-1.0f, 0.0f, 0.0f);
    light.directionLights[1].color.Set(0.5f, 0.0f, 0.0f);

    light.directionLights[2].direction.Set(0.0f, 0.0f, 1.0f);
    light.directionLights[2].color.Set(0.0f, 0.5f, 0.0f);

    light.directionLights[3].direction.Set(0.0f, -1.0f, 0.0f);
    light.directionLights[3].color.Set(0.0f, 0.0f, 0.5f);

    // �|�C���g���C�g��������
    for (auto& pt : light.pointLights)
    {
        pt.position.x = static_cast<float>(random() % 1000) - 500.0f;
        pt.position.y = 20.0f;  // ������20�Œ�
        pt.position.z = static_cast<float>(random() % 1000) - 500.0f;
        pt.range = 50.0f;       // �e���͈͂�50�ŌŒ肵�Ă���
        pt.color.x = static_cast<float>(random() % 255) / 255.0f;
        pt.color.y = static_cast<float>(random() % 255) / 255.0f;
        pt.color.z = static_cast<float>(random() % 255) / 255.0f;
    }

    // �e�B�[�|�b�g���f����������
    ModelInitData teapotModelInitData;
    teapotModelInitData.m_tkmFilePath = "Assets/modelData/teapot.tkm";
    teapotModelInitData.m_fxFilePath = "Assets/shader/renderGBuffer.fx";
    teapotModelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    teapotModelInitData.m_colorBufferFormat[1] = DXGI_FORMAT_R16G16B16A16_FLOAT;
    teapotModelInitData.m_colorBufferFormat[2] = DXGI_FORMAT_R32_FLOAT;
    Model teapotModel;
    teapotModel.Init(teapotModelInitData);

    // �w�i�̃��f����������
    ModelInitData bgModelInitData;

    // ���[�U�[�g���f�[�^�Ƃ��ă|�C���g���C�g�̃��X�g��n��
    bgModelInitData.m_tkmFilePath = "Assets/modelData/bg.tkm";
    bgModelInitData.m_fxFilePath = "Assets/shader/renderGBuffer.fx";
    bgModelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    bgModelInitData.m_colorBufferFormat[1] = DXGI_FORMAT_R16G16B16A16_FLOAT;
    bgModelInitData.m_colorBufferFormat[2] = DXGI_FORMAT_R32_FLOAT;
    Model bgModel;
    bgModel.Init(bgModelInitData);

    // G-Buffer���쐬
    // �A���x�h�J���[�o�͗p�̃����_�����O�^�[�Q�b�g
    RenderTarget albedoRT;
    albedoRT.Create(
        FRAME_BUFFER_W,
        FRAME_BUFFER_H,
        1,
        1,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_D32_FLOAT);

    // �@���o�͗p�̃����_�����O�^�[�Q�b�g
    RenderTarget normalRT;
    normalRT.Create(
        FRAME_BUFFER_W,
        FRAME_BUFFER_H,
        1,
        1,
        DXGI_FORMAT_R16G16B16A16_FLOAT,
        DXGI_FORMAT_UNKNOWN);

    // step-1 �ˉe��Ԃł�Z�l���o�͂��邽�߂�G-Buffer���쐬����
    RenderTarget depthRT;
    depthRT.Create(
        FRAME_BUFFER_W,
        FRAME_BUFFER_H,
        1,
        1,
        DXGI_FORMAT_R32_FLOAT,
        DXGI_FORMAT_UNKNOWN);

    // �����_�����O�^�[�Q�b�g��G-Buffer�ɕύX���ď�������
    RenderTarget* rts[] = {
        &albedoRT,      // 0�Ԗڂ̃����_�����O�^�[�Q�b�g
        &normalRT,      // 1�Ԗڂ̃����_�����O�^�[�Q�b�g
        // step-2 RenderGBuffer�̃p�X�̃����_�����O�^�[�Q�b�g��depthRT��ǉ�����
        & depthRT		//2�Ԗڂ̃����_�����O�^�[�Q�b�g
    };

    // �|�X�g�G�t�F�N�g�I�Ƀf�B�t�@�[�h���C�e�B���O���s�����߂̃X�v���C�g��������
    SpriteInitData spriteInitData;

    // ��ʑS�̂Ƀ����_�����O����̂ŕ��ƍ����̓t���[���o�b�t�@�̕��ƍ����Ɠ���
    spriteInitData.m_width = FRAME_BUFFER_W;
    spriteInitData.m_height = FRAME_BUFFER_H;

    // �f�B�t�@�[�h���C�e�B���O�Ŏg�p����e�N�X�`����ݒ�
    spriteInitData.m_textures[0] = &albedoRT.GetRenderTargetTexture();
    spriteInitData.m_textures[1] = &normalRT.GetRenderTargetTexture();

    // step-3 �f�B�t�@�[�h���C�e�B���O�Ŏg�p����e�N�X�`����ǉ�����
    spriteInitData.m_textures[2] = &depthRT.GetRenderTargetTexture();

    spriteInitData.m_fxFilePath = "Assets/shader/defferedLighting.fx";
    spriteInitData.m_expandConstantBuffer = &light;
    spriteInitData.m_expandConstantBufferSize = sizeof(light);

    // �������f�[�^���g���ăX�v���C�g���쐬
    Sprite defferdLightingSpr;
    defferdLightingSpr.Init( spriteInitData );

    //////////////////////////////////////
    // ���������s���R�[�h�������̂͂����܂ŁI�I�I
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    Vector4 pos = { 20.0f, 10.0f, 30.0f, 1.0f };
    Matrix mViewProj = g_camera3D->GetViewProjectionMatrix();
    mViewProj.Apply(pos);
    mViewProj.Inverse(mViewProj);
    pos.x /= pos.w;
    pos.y /= pos.w;
    pos.z /= pos.w;
    pos.w = 1.0f;
    mViewProj.Apply(pos);
    pos.x /= pos.w;
    pos.y /= pos.w;
    pos.z /= pos.w;

    Stopwatch sw;

    //  ��������Q�[�����[�v
    while (DispatchWindowMessage())
    {
        sw.Start();

        // �����_�����O�J�n
        g_engine->BeginFrame();

        //////////////////////////////////////
        // ��������G��`���R�[�h���L�q����
        //////////////////////////////////////
        // ���C�g����
        Quaternion qRot;
        qRot.SetRotationDegY(1.0f);
        for (auto& pt : light.pointLights)
        {
            qRot.Apply(pt.position);
        }

        // �܂��A�����_�����O�^�[�Q�b�g�Ƃ��Đݒ�ł���悤�ɂȂ�܂ő҂�
        renderContext.WaitUntilToPossibleSetRenderTargets(ARRAYSIZE(rts), rts);

        // �����_�����O�^�[�Q�b�g��ݒ�
        renderContext.SetRenderTargets(ARRAYSIZE(rts), rts);

        // �����_�����O�^�[�Q�b�g���N���A
        renderContext.ClearRenderTargetViews(ARRAYSIZE(rts), rts);

        teapotModel.Draw(renderContext);
        bgModel.Draw(renderContext);

        // �����_�����O�^�[�Q�b�g�ւ̏������ݑ҂�
        renderContext.WaitUntilFinishDrawingToRenderTargets(ARRAYSIZE(rts), rts);

        // �����_�����O����t���[���o�b�t�@�ɖ߂��ăX�v���C�g�������_�����O����
        g_graphicsEngine->ChangeRenderTargetToFrameBuffer(renderContext);

        // G-Buffer�̓��e�����ɂ��ăX�v���C�g�������_�����O
        defferdLightingSpr.Draw(renderContext);

        /////////////////////////////////////////
        //�G��`���R�[�h�������̂͂����܂ŁI�I�I
        //////////////////////////////////////
        // �����_�����O�I��
        g_engine->EndFrame();
        sw.Stop();
        printf("fps = %0.2f\n", 1.0f / sw.GetElapsed() );
    }

    ::FreeConsole();
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
// / �W�����o�̓R���\�[����������
/// </summary>
void InitStandardIOConsole()
{
    ::AllocConsole();               // �R�}���h�v�����v�g���\�������
    freopen("CON", "w", stdout);    // �W���o�͂̊��蓖��

    auto fhandle = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT rc;

    rc.Top = 0;
    rc.Left = 0;
    rc.Bottom = 30;
    rc.Right = 30;
    ::SetConsoleWindowInfo(fhandle, TRUE, &rc);
}