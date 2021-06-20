/*!
 *@brief �Z�p�`�u���[
 */

// �u���[��������e�N�X�`���̕�
static const float BLUR_TEX_W = 1280.0f;

// �u���[��������e�N�X�`���̍���
static const float BLUR_TEX_H = 720.0f;

// �u���[���a�B���̐��l��傫������ƘZ�p�`�{�P���傫���Ȃ�
static const float BLUR_RADIUS = 8.0f;

struct VSInput
{
    float4 pos : POSITION;
    float2 uv  : TEXCOORD0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

cbuffer cb : register(b0)
{
    float4x4 mvp;       // MVP�s��
    float4 mulColor;    // ��Z�J���[
};

// step-6  �����A�Ίp���u���[�̏o�͍\���̂��`

/*!
 *@brief ���_�V�F�[�_�[
 */
PSInput VSMain(VSInput In)
{
    PSInput psIn;
    psIn.pos = mul(mvp, In.pos);
    psIn.uv = In.uv;
    return psIn;
}

Texture2D<float4> srcTexture : register(t0); // �u���[��������O�̃I���W�i���e�N�X�`��

// �T���v���[�X�e�[�g
sampler g_sampler : register(s0);

/*!
 *@brief �����A�΂߃u���[�̃s�N�Z���V�F�[�_�[
 */
PSOutput PSVerticalDiagonalBlur(PSInput pIn)
{
    PSOutput psOut = (PSOutput)0;

    // �u���[��������e�N�X�`���̃J���[���擾
    float4 srcColor = srcTexture.Sample(
        g_sampler, pIn.uv );

    // step-7 �u���[���a�iBLUR_RADIUS�j����u���[�X�e�b�v�̒��������߂�

    // step-8 ����������UV�I�t�Z�b�g���v�Z

    // step-9 ���������ɃJ���[���T���v�����O���ĕ��ς���

    // step-10 �Ίp��������UV�I�t�Z�b�g���v�Z

    // step-11 �Ίp�������ɃJ���[���T���v�����O���ĕ��ω�����

    return psOut;
}

Texture2D<float4> blurTexture_0 : register(t0); // �u���[�e�N�X�`��_0�B1�p�X�ڂō쐬���ꂽ�e�N�X�`��
Texture2D<float4> blurTexture_1 : register(t1); // �u���[�e�N�X�`��_1�B1�p�X�ڂō쐬���ꂽ�e�N�X�`��

/*!
 *@brief �Z�p�`�쐬�u���[
 */
float4 PSRhomboidBlur(PSInput pIn) : SV_Target0
{
    // �u���[�X�e�b�v�̒��������߂�
    float blurStepLen = BLUR_RADIUS / 4.0f;

    // step-12 ���΂߉������ւ�UV�I�t�Z�b�g���v�Z����

    // step-13 ���΂߉������ɃJ���[���T���v�����O����

    // step-14 �E�΂߉������ւ�UV�I�t�Z�b�g���v�Z����

    // step-15 �E�΂߉������ɃJ���[���T���v�����O����

    // step-16 ���ω�

    return color;
}
