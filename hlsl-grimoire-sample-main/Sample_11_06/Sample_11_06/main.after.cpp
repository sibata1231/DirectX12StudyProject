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
    const int NUM_SHADOW_MAP = 3;

    // step-2 ���C�g�r���[�v���W�F�N�V�����N���b�v�s��̔z����`����
    Matrix lvpcMatrix[NUM_SHADOW_MAP];

    // step-3 �V���h�E�}�b�v���������ރ����_�����O�^�[�Q�b�g���R���p�ӂ���
    RenderTarget shadowMaps[NUM_SHADOW_MAP];

    // �߉e�p�̃V���h�E�}�b�v
    shadowMaps[0].Create(
        2048,
        2048,
        1,
        1,
        DXGI_FORMAT_R32_FLOAT,
        DXGI_FORMAT_D32_FLOAT,
        clearColor
    );

    // ���e�p�̃V���h�E�}�b�v
    shadowMaps[1].Create(
        1024,
        1024,
        1,
        1,
        DXGI_FORMAT_R32_FLOAT,
        DXGI_FORMAT_D32_FLOAT,
        clearColor
    );

    // ���e�p�̃V���h�E�}�b�v
    shadowMaps[2].Create(
        512,
        512,
        1,
        1,
        DXGI_FORMAT_R32_FLOAT,
        DXGI_FORMAT_D32_FLOAT,
        clearColor
    );

    // step-4 �����G���A�̍ő�[�x�l���`����
    float cascadeAreaTbl[NUM_SHADOW_MAP] = {
        500,                    // �߉e���f���ő�[�x�l
        2000,                   // ���e���f���ő�[�x�l
        g_camera3D->GetFar(),   // ���e���f���ő�[�x�l�B�ő�[�x�̓J������Far�N���b�v
    };

    // �e�𗎂Ƃ����f��������������
    Model testShadowModel[NUM_SHADOW_MAP];
    InitShadowCaster(testShadowModel[0]);
    InitShadowCaster(testShadowModel[1]);
    InitShadowCaster(testShadowModel[2]);

    // �ʏ�`��̃e�B�[�|�b�g���f����������
    ModelStandard teapotModel;
    teapotModel.Init("Assets/modelData/testModel.tkm");

    // step-5 �e���󂯂�w�i���f����������
    ModelInitData bgModelInitData;

    // �V���h�E���V�[�o�[�i�e�����Ƃ���郂�f���j�p�̃V�F�[�_�[���w�肷��
    bgModelInitData.m_fxFilePath = "Assets/shader/sampleShadowReciever.fx";

    // �y���ځz�V���h�E�}�b�v���g��SRV�ɐݒ肷��
    bgModelInitData.m_expandShaderResoruceView[0] = &shadowMaps[0].GetRenderTargetTexture();
    bgModelInitData.m_expandShaderResoruceView[1] = &shadowMaps[1].GetRenderTargetTexture();
    bgModelInitData.m_expandShaderResoruceView[2] = &shadowMaps[2].GetRenderTargetTexture();

    // �y���ځz���C�g�r���[�v���W�F�N�V�����N���b�v�s����g���萔�o�b�t�@�[�ɐݒ肷��
    bgModelInitData.m_expandConstantBuffer = (void*)lvpcMatrix;
    bgModelInitData.m_expandConstantBufferSize = sizeof(lvpcMatrix);
    bgModelInitData.m_tkmFilePath = "Assets/modelData/bg/bg.tkm";

    Model bgModel;
    bgModel.Init(bgModelInitData);

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
        // �O�����ƉE�����͂��łɌv�Z�ς݂Ȃ̂ŁA��������������Ă���
        const auto& cameraForward = g_camera3D->GetForward();
        const auto& cameraRight = g_camera3D->GetRight();

        // �J�����̏�����͑O�����ƉE�����̊O�ςŋ��߂�
        Vector3 cameraUp;
        cameraUp.Cross( cameraForward, cameraRight );

        // nearDepth�̓G���A�̍ŏ��[�x�l��\��
        // ��ԋ߂��G���A�̍ŏ��[�x�l�̓J�����̃j�A�N���b�v
        float nearDepth = g_camera3D->GetNear();
        for (int areaNo = 0; areaNo < NUM_SHADOW_MAP; areaNo++)
        {
            // step-7 �G���A�����鎋����̂W���_�����߂�
            // �G���A�̋ߕ��ʂ̒��S����̏�ʁA���ʂ܂ł̋��������߂�
            float nearY = tanf(g_camera3D->GetViewAngle() * 0.5f) * nearDepth;

            // �G���A�̋ߕ��ʂ̒��S����̉E�ʁA���ʂ܂ł̋��������߂�
            float nearX = nearY * g_camera3D->GetAspect();

            // �G���A�̉����ʂ̒��S����̏�ʁA���ʂ܂ł̋��������߂�
            float farY = tanf(g_camera3D->GetViewAngle()*0.5f) * cascadeAreaTbl[areaNo];

            // �G���A�̉����ʂ̒��S����̉E�ʁA���ʂ܂ł̋��������߂�
            float farX = farY * g_camera3D->GetAspect();

            // �G���A�̋ߕ��ʂ̒��S���W�����߂�
            Vector3 nearPos = g_camera3D->GetPosition() + cameraForward * nearDepth;

            // �G���A�̉����ʂ̒��S���W�����߂�
            Vector3 farPos = g_camera3D->GetPosition() + cameraForward * cascadeAreaTbl[areaNo];

            // 8���_�����߂�
            Vector3 vertex[8];

            // �ߕ��ʂ̉E��̒��_
            vertex[0] += nearPos + cameraUp *  nearY + cameraRight *  nearX;

            // �ߕ��ʂ̍���̒��_
            vertex[1] += nearPos + cameraUp *  nearY + cameraRight * -nearX;

            // �ߕ��ʂ̉E���̒��_
            vertex[2] += nearPos + cameraUp * -nearY + cameraRight *  nearX;

            // �ߕ��ʂ̍����̒��_
            vertex[3] += nearPos + cameraUp * -nearY + cameraRight * -nearX;

            // �����ʂ̉E��̒��_
            vertex[4] += farPos + cameraUp *  farY + cameraRight *  farX;

            // �����ʂ̍���̒��_
            vertex[5] += farPos + cameraUp *  farY + cameraRight * -farX;

            // �����ʂ̉E���̒��_
            vertex[6] += farPos + cameraUp * -farY + cameraRight *  farX;

            // �����ʂ̍����̒��_
            vertex[7] += farPos + cameraUp * -farY + cameraRight * -farX;

            // step-8 8���_��ϊ����čő�l�A�ŏ��l�����߂�
            Vector3 vMax, vMin;
            vMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
            vMin = {  FLT_MAX,  FLT_MAX,  FLT_MAX };
            for (auto& v : vertex)
            {
                lvpMatrix.Apply(v);
                vMax.Max(v);
                vMin.Min(v);
            }

            // step-9 �N���b�v�s������߂�
            float xScale = 2.0f / (vMax.x - vMin.x);
            float yScale = 2.0f / (vMax.y - vMin.y);
            float xOffset = (vMax.x + vMin.x) * -0.5f * xScale;
            float yOffset = (vMax.y + vMin.y) * -0.5f * yScale;
            Matrix clopMatrix;
            clopMatrix.m[0][0] = xScale;
            clopMatrix.m[1][1] = yScale;
            clopMatrix.m[3][0] = xOffset;
            clopMatrix.m[3][1] = yOffset;

            // step-10 ���C�g�r���[�v���W�F�N�V�����s��ɃN���b�v�s�����Z����
            lvpcMatrix[areaNo] = lvpMatrix * clopMatrix;

            // step-11 �V���h�E�}�b�v�Ƀ����_�����O
            // �����_�����O�^�[�Q�b�g���V���h�E�}�b�v�ɕύX����
            renderContext.WaitUntilToPossibleSetRenderTarget(shadowMaps[areaNo]);
            renderContext.SetRenderTargetAndViewport(shadowMaps[areaNo]);
            renderContext.ClearRenderTargetView(shadowMaps[areaNo]);

            // �e���f����`��
            testShadowModel[areaNo].Draw(renderContext, g_matIdentity, lvpcMatrix[areaNo]);

            // �������݊����҂�
            renderContext.WaitUntilFinishDrawingToRenderTarget(shadowMaps[areaNo]);

            // ���̃G���A�̋ߕ��ʂ܂ł̋�����������
            nearDepth = cascadeAreaTbl[areaNo];
        }

        // �ʏ탌���_�����O
        // �����_�����O�^�[�Q�b�g���t���[���o�b�t�@�[�ɖ߂�
        renderContext.SetRenderTarget(
            g_graphicsEngine->GetCurrentFrameBuffuerRTV(),
            g_graphicsEngine->GetCurrentFrameBuffuerDSV()
        );
        renderContext.SetViewportAndScissor(g_graphicsEngine->GetFrameBufferViewport());

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
    toLigDir *= 5000.0f; // ���C�g�܂ł̋���

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

    // �V���h�E���V�[�o�[�i�e�����Ƃ���郂�f���j�p�̃V�F�[�_�[���w�肷��
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
