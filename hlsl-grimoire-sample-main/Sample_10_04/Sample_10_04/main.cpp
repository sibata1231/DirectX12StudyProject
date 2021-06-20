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

    // step-3 ���u���[�p�̃����_�����O�^�[�Q�b�g���쐬

    // step-4 ���u���[�p�̃X�v���C�g��������

    // step-5 �c�u���[�p�̃����_�����O�^�[�Q�b�g���쐬

    // step-6 �c�u���[�p�̃X�v���C�g��������

    // step-7 �e�N�X�`����\��t���邽�߂̃X�v���C�g������������

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

        // step-9 mainRenderTarget�Ɋe�탂�f����`�悷��

        // step-10 mainRenderTarget�ɕ`�悳�ꂽ�摜�ɉ��u���[��������

        // step-11 �c�u���[���s��

        // step-12 ���C�������_�����O�^�[�Q�b�g�̊G���t���[���o�b�t�@�ɃR�s�[

        //////////////////////////////////////
        // �G��`���R�[�h�������̂͂����܂ŁI�I�I
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
