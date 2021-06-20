#pragma once

/// <summary>
/// Standard�V�F�[�_�[�𗘗p�������f���\������
/// </summary>
class ModelStandard
{
public:
	enum { NUM_DIRECTIONAL_LIGHT = 4 };	//�f�B���N�V�������C�g�̐��B

	/// <summary>
	/// �f�B���N�V�������C�g�B
	/// </summary>
	struct DirectionalLight {
		Vector3 direction;	//���C�g�̕����B
		float pad0;			//�p�f�B���O�B
		Vector4 color;		//���C�g�̃J���[�B
	};
	/// <summary>
	/// ���C�g�\���́B
	/// </summary>
	struct Light {
		DirectionalLight directionalLight[NUM_DIRECTIONAL_LIGHT];	//�f�B���N�V�������C�g�B
		Vector3 eyePos;					//�J�����̈ʒu�B
		float specPow;					//�X�y�L�����̍i��B
		Vector3 ambinetLight;			//�����B
	};
public: 
	/// <summary>
	/// �������B
	/// </summary>
	/// <param name="tkmFilePath">tkm�t�@�C���̃t�@�C���p�X�B</param>
	void Init(const char* tkmFilePath);
	/// <summary>
	/// ���t���[���Ăяo���K�v������X�V�����B
	/// </summary>
	/// <param name="pos">���W</param>
	/// <param name="rot">��]</param>
	/// <param name="scale">�g�嗦</param>
	void Update(Vector3 pos, Quaternion rot, Vector3 scale)
	{
		m_light.eyePos = g_camera3D->GetPosition();
		m_model.UpdateWorldMatrix(pos, rot, scale);
	}
	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="rc">�����_�����O�R���e�L�X�g</param>
	void Draw(RenderContext& rc)
	{
		m_model.Draw(rc);
	}
private:
	Model m_model;	//���f���B
	Light m_light;	//���C�g�B
};

