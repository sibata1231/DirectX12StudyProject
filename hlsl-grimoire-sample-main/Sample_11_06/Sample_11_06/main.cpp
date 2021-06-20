#include "stdafx.h"
#include "system/system.h"
#include "ModelStandard.h"

// �֐��錾
void InitRootSignature(RootSignature& rs);
void MoveCamera();
void InitShadowCaster(Model& model);
void InitShadowReciever(Model& model, Matrix* lvpMatrix, RenderTarget* shadowMap);
void InitLightCamera(Camera& lightCamera);

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

    g_camera3D->SetPosition(0, 100.0f, 350.0f);
    g_camera3D->SetTarget(0, 100.0f, 0);
    RootSignature rs;
    InitRootSignature(rs);

    // �e�`��p�̃��C�g�J�������쐬����
    Camera lightCamera;
    InitLightCamera(lightCamera);

    // �V���h�E�}�b�v��3���쐬����
    float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    // step-1 �V���h�E�}�b�v�̖�����萔�Œ�`����

    // step-2 ���C�g�r���[�v���W�F�N�V�����N���b�v�s��̔z����`����

    // step-3 �V���h�E�}�b�v���������ރ����_�����O�^�[�Q�b�g��3���p�ӂ���

    // step-4 �����G���A�̍ő�[�x�l���`����

    // �e�𗎂Ƃ����f��������������
    Model testShadowModel[NUM_SHADOW_MAP];
    InitShadowCaster(testShadowModel[0]);
    InitShadowCaster(testShadowModel[1]);
    InitShadowCaster(testShadowModel[2]);

    // �ʏ�`��̃e�B�[�|�b�g���f����������
    ModelStandard teapotModel;
    teapotModel.Init("Assets/modelData/testModel.tkm");

    // step-5 �e���󂯂�w�i���f����������

    //////////////////////////////////////
    // ���������s���R�[�h�������̂͂����܂ŁI�I�I
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    // ��������Q�[�����[�v
    while (DispatchWindowMessage())
    {
        // 1�t���[���̊J�n
        g_engine->BeginFrame();

        // �J�����𓮂���
        MoveCamera();

        //////////////////////////////////////
        // ��������G��`���R�[�h���L�q����
        //////////////////////////////////////

        const auto& lvpMatrix = lightCamera.GetViewProjectionMatrix();

        // step-6 �J�����̑O�����A�E�����A����������߂�

        // nearDepth�̓G���A�̍ŏ��[�x�l��\��
        // ��ԋ߂��G���A�̍ŏ��[�x�l�̓J�����̃j�A�N���b�v
        float nearDepth = g_camera3D->GetNear();
        for (int areaNo = 0; areaNo < NUM_SHADOW_MAP; areaNo++)
        {
            // step-7 �G���A�����鎋�����8���_�����߂�

            // step-8 8���_��ϊ����čő�l�A�ŏ��l�����߂�

            // step-9 �N���b�v�s������߂�

            // step-10 ���C�g�r���[�v���W�F�N�V�����s��ɃN���b�v�s�����Z����

            // step-11 �V���h�E�}�b�v�Ƀ����_�����O

            // ���̃G���A�̋ߕ��ʂ܂ł̋�����������
            nearDepth = cascadeAreaTbl[areaNo];
        }

        // �ʏ탌���_�����O
        // �����_�����O�^�[�Q�b�g���t���[���o�b�t�@�[�ɖ߂�
        renderContext.SetRenderTarget(
            g_graphicsEngine->GetCurrentFrameBuffuerRTV(),
            g_graphicsEngine->GetCurrentFrameBuffuerDSV()
        );
        renderContext.SetViewportAndScissor(
            g_graphicsEngine->GetFrameBufferViewport());

        // �e�B�[�|�b�g���f����`��
        teapotModel.Draw(renderContext);

        // �e���󂯂�w�i��`��
        bgModel.Draw(renderContext);

        //////////////////////////////////////
        // �G��`���R�[�h�������̂͂����܂ŁI�I�I
        //////////////////////////////////////

        // 1�t���[���I��
        g_engine->EndFrame();
    }
    return 0;
}

void InitLightCamera(Camera& lightCamera)
{
    // ���C�g�܂ł̃x�N�g��
    Vector3 toLigDir = { 1, 2, 1 };
    toLigDir.Normalize();
    toLigDir *= 5000.0f;    // ���C�g�܂ł̋���

    // �J�����̈ʒu��ݒ�B����̓��C�g�̈ʒu
    lightCamera.SetPosition(toLigDir);

    // �J�����̒����_��ݒ�B���ꂪ���C�g���Ƃ炵�Ă���ꏊ
    lightCamera.SetTarget({ 0, 0, 0 });

    // �������ݒ�B����̓��C�g���^���������Ă���̂ŁAX��������ɂ��Ă���
    lightCamera.SetUp(1, 0, 0);

    lightCamera.SetUpdateProjMatrixFunc(Camera::enUpdateProjMatrixFunc_Ortho);
    lightCamera.SetWidth(5000.0f);
    lightCamera.SetHeight(5000.0f);
    lightCamera.SetNear(1.0f);
    lightCamera.SetFar(10000.0f);

    // ���C�g�r���[�v���W�F�N�V�����s����v�Z���Ă���
    lightCamera.Update();
}

// ���[�g�V�O�l�`���̏�����
void InitRootSignature( RootSignature& rs )
{
    rs.Init(D3D12_FILTER_MIN_MAG_MIP_LINEAR,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP);
}

void MoveCamera()
{
    auto pos = g_camera3D->GetPosition();
    auto target = g_camera3D->GetTarget();
    pos.z -= g_pad[0]->GetLStickYF() * 2.0f;
    target.z -= g_pad[0]->GetLStickYF() * 2.0f;
    pos.y += g_pad[0]->GetRStickYF() * 2.0f;
    target.y += g_pad[0]->GetRStickYF() * 2.0f;
    g_camera3D->SetPosition(pos);
    g_camera3D->SetTarget(target);
}

void InitShadowCaster(Model& model)
{
    // �V���h�E�L���X�^�[���f��������������
    // �V���h�E�L���X�^�[���f�������������邽�߂̏������f�[�^��ݒ肷��
    ModelInitData modelInitData;

    // �V���h�E�}�b�v�`��p�̃V�F�[�_�[���w�肷��
    modelInitData.m_fxFilePath = "Assets/shader/sampleDrawShadowMap.fx";
    modelInitData.m_tkmFilePath = "Assets/modelData/testModel.tkm";

    // �J���[�o�b�t�@�[�̃t�H�[�}�b�g�ɕύX���������̂ŁA��������ύX����
    modelInitData.m_colorBufferFormat[0] = DXGI_FORMAT_R32_FLOAT;

    model.Init(modelInitData);
}

void InitShadowReciever(Model& model, Matrix* lvpMatrix, RenderTarget* shadowMap)
{
    // �e���󂯂�w�i���f����������
    ModelInitData bgModelInitData;

    // �V���h�E���V�[�o�[(�e�����Ƃ���郂�f��)�p�̃V�F�[�_�[���w�肷��
    bgModelInitData.m_fxFilePath = "Assets/shader/sampleShadowReciever.fx";

    // �V���h�E�}�b�v�́A�K�E�V�A���u���[�łڂ��������̂𗘗p����
    bgModelInitData.m_expandShaderResoruceView[0] = &shadowMap[0].GetRenderTargetTexture();
    bgModelInitData.m_expandShaderResoruceView[1] = &shadowMap[1].GetRenderTargetTexture();
    bgModelInitData.m_expandShaderResoruceView[2] = &shadowMap[2].GetRenderTargetTexture();

    // �e�p�̃p�����[�^���g���萔�o�b�t�@�[�ɐݒ肷��
    bgModelInitData.m_expandConstantBuffer = (void*)lvpMatrix;
    bgModelInitData.m_expandConstantBufferSize = sizeof(*lvpMatrix) * 3;
    bgModelInitData.m_tkmFilePath = "Assets/modelData/bg/bg.tkm";

    model.Init(bgModelInitData);
}
