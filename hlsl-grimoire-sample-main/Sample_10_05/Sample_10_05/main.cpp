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
    Vector3 eyePos;                 // �J�����̈ʒu
    float specPow;                  // �X�y�L�����̍i��
    Vector3 ambinetLight;           // ����
};

const int NUM_WEIGHTS = 8;
/// <summary>
/// �u���[�p�̃p�����[�^�[
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
    //  �������珉�������s���R�[�h���L�q����
    //////////////////////////////////////

    RootSignature rs;
    InitRootSignature(rs);

    // step-1 ���C�������_�����O�^�[�Q�b�g���쐬����

    // step-2 �������̃��C�g��p�ӂ���

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

    // step-3 �P�x���o�p�̃����_�����O�^�[�Q�b�g���쐬

    // step-4 �P�x���o�p�̃X�v���C�g��������

    // step-5 �K�E�V�A���u���[��������

    // step-6 �{�P�摜�����Z��������X�v���C�g��������

    // step-7 �e�N�X�`����\��t���邽�߂̃X�v���C�g������������

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

        // step-8 �����_�����O�^�[�Q�b�g��mainRenderTarget�ɕύX����

        // step-9 mainRenderTarget�Ɋe�탂�f����`�悷��

        // step-10 �P�x���o

        // step-11 �K�E�V�A���u���[�����s����

        // step-12 �{�P�摜�����C�������_�����O�^�[�Q�b�g�ɉ��Z����

        // step-13 ���C�������_�����O�^�[�Q�b�g�̊G���t���[���o�b�t�@�[�ɃR�s�[

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
