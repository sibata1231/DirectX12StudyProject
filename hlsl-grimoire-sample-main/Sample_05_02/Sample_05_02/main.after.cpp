#include "stdafx.h"
#include "system/system.h"

/// <summary>
/// ���C�g�\����
/// </summary>
struct Light
{
    // �f�B���N�V�������C�g�p�̃����o
    Vector3 dirDirection;   // ���C�g�̕���
    float pad0;
    Vector3 dirColor;       // ���C�g�̃J���[
    float pad1;

    // ���C�g�\���̂Ƀ|�C���g���C�g�p�̃����o�ϐ���ǉ�
    Vector3 ptPosition;     // �ʒu
    float pad2;             // �p�f�B���O
    Vector3 ptColor;        // �J���[
    float ptRange;          // �e���͈�

    // step-1 ���C�g�\���̂ɃX�|�b�g���C�g�p�̃����o�ϐ���ǉ�
    Vector3 spPosition;     // �ʒu
    float pad3;             // �p�f�B���O
    Vector3 spColor;        // �J���[
    float spRange;          // �e���͈�
    Vector3 spDirection;    // �ˏo����
    float spAngle;          // �ˏo�p�x

    Vector3 eyePos;         // ���_�̈ʒu
    float pad4;

    Vector3 ambientLight;   // �A���r�G���g���C�g
};

//////////////////////////////////////
// �֐��錾
//////////////////////////////////////
void InitModel(Model& bgModel, Model& teapotModel, Model& lightModel, Light& light);
void InitDirectionLight(Light& light);
void InitPointLight(Light& light);
void InitAmbientLight(Light& light);

///////////////////////////////////////////////////////////////////
// �E�B���h�E�v���O�����̃��C���֐�
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    // �Q�[���̏�����
    InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));

    g_camera3D->SetPosition({ 0.0f, 50.0f, 200.0f });
    g_camera3D->SetTarget({ 0.0f, 50.0f, 0.0f });

    //////////////////////////////////////
    // �������珉�������s���R�[�h���L�q����
    //////////////////////////////////////
    // ���C�g�̃f�[�^���쐬����
    Light light;
    // �f�B���N�V�������C�g������������
    InitDirectionLight(light);
    // �|�C���g���C�g������������
    InitPointLight(light);
    // �A���r�G���g���C�g������������
    InitAmbientLight(light);

    // step-2 �X�|�b�g���C�g�̃f�[�^������������
    // �������W��X = 0�AY = 50�AZ = 0�ɂ���
    light.spPosition.x = 0.0f;
    light.spPosition.y = 50.0f;
    light.spPosition.z = 0.0f;

    // ���C�g�̃J���[��ݒ�BR = 10�AG = 10�AB = 10�ɂ���
    light.spColor.x = 10.0f;
    light.spColor.y = 10.0f;
    light.spColor.z = 10.0f;

    // ���������͎΂߉��ɂ���
    light.spDirection.x = 1.0f;
    light.spDirection.y = -1.0f;
    light.spDirection.z = 1.0f;

    // �����f�[�^�Ȃ̂ŁA�傫����1�ɂ���K�v������̂Ő��K������
    light.spDirection.Normalize();

    // �ˏo�͈͂�300
    light.spRange = 300.0f;

    // �ˏo�p�x��10�x
    light.spAngle = Math::DegToRad(25.0f);

    // ���f��������������
    // ���f�������������邽�߂̏����\�z����
    Model lightModel, bgModel, teapotModel;
    InitModel(bgModel, teapotModel, lightModel , light);

    //////////////////////////////////////
    // ���������s���R�[�h�������̂͂����܂ŁI�I�I
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    // ��������Q�[�����[�v
    while (DispatchWindowMessage())
    {
        //�����_�����O�J�n
        g_engine->BeginFrame();

        //////////////////////////////////////
        // ��������G��`���R�[�h���L�q����
        //////////////////////////////////////

        // step-3 �R���g���[���[���X�e�B�b�N�ŃX�|�b�g���C�g���ړ�������
        // ���̃A�i���O�X�e�B�b�N�œ�����
        light.spPosition.x -= g_pad[0]->GetLStickXF();
        if ( g_pad[0]->IsPress(enButtonB) )
        {
            // B�{�^�����ꏏ�ɉ�����Ă�����Y�������ɓ�����
            light.spPosition.y += g_pad[0]->GetLStickYF();
        }
        else
        {
            // Z�������ɓ�����
            light.spPosition.z -= g_pad[0]->GetLStickYF();
        }

        // step-4 �R���g���[���[�E�X�e�B�b�N�ŃX�|�b�g���C�g����]������
        // Y������̉�]�N�H�[�^�j�I�����v�Z����
        Quaternion qRotY;
        qRotY.SetRotationY(g_pad[0]->GetRStickXF() * 0.01f);

        // �v�Z�����N�H�[�^�j�I���Ń��C�g�̕�������
        qRotY.Apply(light.spDirection);

        // X������̉�]�N�H�[�^�j�I�����v�Z����
        Vector3 rotAxis;
        rotAxis.Cross(g_vec3AxisY, light.spDirection);
        Quaternion qRotX;
        qRotX.SetRotation(rotAxis, g_pad[0]->GetRStickYF() * 0.01f);

        // �v�Z�����N�H�[�^�j�I���Ń��C�g�̕�������
        qRotX.Apply(light.spDirection);

        // �X�|�b�g���C�g���f���̉�]�N�H�[�^�j�I�������߂�
        Quaternion qRot;
        qRot.SetRotation({ 0.0f, 0.0f, -1.0f }, light.spDirection);

        // �X�|�b�g���C�g���f���̃��[���h�s����X�V����
        lightModel.UpdateWorldMatrix(light.spPosition, qRot, g_vec3One);

        // �w�i���f�����h���[
        bgModel.Draw(renderContext);

        // �d�����f�����h���[
        lightModel.Draw(renderContext);

        //////////////////////////////////////
        // �G��`���R�[�h�������̂͂����܂ŁI�I�I
        //////////////////////////////////////
        // �����_�����O�I��
        g_engine->EndFrame();
    }
    return 0;
}

/// <summary>
/// ���f����������
/// </summary>
/// <param name="bgModel"></param>
/// <param name="teapotModel"></param>
/// <param name="lightModel"></param>
/// <param name="light"></param>
void InitModel(Model& bgModel, Model& teapotModel, Model& lightModel, Light& light)
{
    ModelInitData bgModelInitData;
    bgModelInitData.m_tkmFilePath = "Assets/modelData/bg.tkm";
    // �g�p����V�F�[�_�[�t�@�C���p�X��ݒ肷��
    bgModelInitData.m_fxFilePath = "Assets/shader/sample.fx";

    // �f�B���N�V�������C�g�̏���萔�o�b�t�@�Ƃ��ăf�B�X�N���v�^�q�[�v�ɓo�^���邽�߂�
    // ���f���̏��������Ƃ��ēn��
    bgModelInitData.m_expandConstantBuffer = &light;
    bgModelInitData.m_expandConstantBufferSize = sizeof(light);

    // �����������g���ă��f��������������
    bgModel.Init(bgModelInitData);

    ModelInitData teapotModelInitData;
    teapotModelInitData.m_tkmFilePath = "Assets/modelData/teapot.tkm";

    // �g�p����V�F�[�_�[�t�@�C���p�X��ݒ肷��
    teapotModelInitData.m_fxFilePath = "Assets/shader/sample.fx";

    // �f�B���N�V�������C�g�̏���萔�o�b�t�@�Ƃ��ăf�B�X�N���v�^�q�[�v�ɓo�^���邽�߂�
    // ���f���̏��������Ƃ��ēn��
    teapotModelInitData.m_expandConstantBuffer = &light;
    teapotModelInitData.m_expandConstantBufferSize = sizeof(light);

    // �����������g���ă��f��������������
    teapotModel.Init(teapotModelInitData);

    teapotModel.UpdateWorldMatrix(
        { 0.0f, 20.0f, 0.0f },
        g_quatIdentity,
        g_vec3One
    );

    ModelInitData lightModelInitData;
    lightModelInitData.m_tkmFilePath = "Assets/modelData/light.tkm";

    // �g�p����V�F�[�_�[�t�@�C���p�X��ݒ肷��
    lightModelInitData.m_fxFilePath = "Assets/shader/other/light.fx";
    lightModelInitData.m_expandConstantBuffer = &light;
    lightModelInitData.m_expandConstantBufferSize = sizeof(light);

    // �����������g���ă��f��������������
    lightModel.Init(lightModelInitData);
}

void InitDirectionLight(Light& light)
{
    // ���C�g�͉E�����瓖�����Ă���
    light.dirDirection.x = 1.0f;
    light.dirDirection.y = -1.0f;
    light.dirDirection.z = -1.0f;
    light.dirDirection.Normalize();

    // ���C�g�̃J���[�͔�
    light.dirColor.x = 0.5f;
    light.dirColor.y = 0.5f;
    light.dirColor.z = 0.5f;

    // ���_�̈ʒu��ݒ肷��
    light.eyePos = g_camera3D->GetPosition();
}

void InitPointLight(Light& light)
{
    // �|�C���g���C�g�̍��W��ݒ肷��
    light.ptPosition.x = 0.0f;
    light.ptPosition.y = 50.0f;
    light.ptPosition.z = 50.0f;

    // �|�C���g���C�g�̃J���[��ݒ肷��
    light.ptColor.x = 0.0f;
    light.ptColor.y = 0.0f;
    light.ptColor.z = 0.0f;

    // �|�C���g���C�g�̉e���͈͂�ݒ肷��
    light.ptRange = 100.0f;
}

void InitAmbientLight(Light& light)
{
    // �A���r�G���g���C�g
    light.ambientLight.x = 0.3f;
    light.ambientLight.y = 0.3f;
    light.ambientLight.z = 0.3f;
}
