#pragma once

#include "MyRenderer.h"

namespace myRenderer
{
    class RenderingEngine;
    /// <summary>
    /// �t�H���[�h�����_�����O�p�̃��f���������\���́B
    /// </summary>
    /// <remark>
    /// ModelInitData���p�����Ă��܂��B
    /// �t�H���[�h�����_�����O�̂��߂ɕK�v�ȃf�t�H���g�ݒ���R���X�g���N�^�ōs���Ă���܂��B
    /// </remark>
    struct ModelInitDataFR : public ModelInitData
    {
        ModelInitDataFR()
        {
            m_colorBufferFormat[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
        }
    };
    class ModelRender
    {
    public:
        
        /// <summary>
        /// ������
        /// </summary>
        /// <remark>
        /// �{�֐��𗘗p���ď��������ꂽ���f���́A
        /// �f�B�t�@�[�h�����_�����O�̕`��p�X�ŕ`�悳��܂�
        /// �f�B�t�@�[�h�����_�����O�ł̕`���PBR�V�F�[�_�[�Ń��C�e�B���O���v�Z����܂�
        /// </remark>
        /// <param name="renderingEngine">�����_�����O�G���W���B</param>
        /// <param name="tkmFilePath">tkm�t�@�C���p�X�B</param>
        /// <param name="isShadowReciever">�V���h�E���V�[�o�[�t���O</param>
        void InitDeferredRendering(RenderingEngine& renderingEngine, const char* tkmFilePath, bool isShadowReciever);

        /// <summary>
        /// ������
        /// </summary>
        /// <remark>
        /// �{�֐��𗘗p���ď��������ꂽ���f����
        /// �t�H���[�h�����_�����O�̕`��p�X�ŕ`�悳��܂�
        /// ����ȃV�F�[�f�B���O���s�������ꍇ�́A��������g�p���Ă�������
        /// </remark>
        /// <param name="renderingEngine">�����_�����O�G���W��</param>
        /// <param name="modelInitData"></param>
        void InitForwardRendering(RenderingEngine& renderingEngine, ModelInitDataFR& modelInitData);

        /// <summary>
        /// ���[���h�s����X�V����
        /// </summary>
        /// <param name="pos">���W</param>
        /// <param name="rot">��]�N�H�[�^�j�I��</param>
        /// <param name="scale">�g�嗦</param>
        void UpdateWorldMatrix(Vector3 pos, Quaternion rot, Vector3 scale)
        {
            m_zprepassModel.UpdateWorldMatrix(pos, rot, scale);
            if (m_renderToGBufferModel.IsInited())
            {
                m_renderToGBufferModel.UpdateWorldMatrix(pos, rot, scale);
            }
            if (m_forwardRenderModel.IsInited())
            {
                m_forwardRenderModel.UpdateWorldMatrix(pos, rot, scale);
            }
        }

        /// <summary>
        /// �`��
        /// </summary>
        void Draw();

        /// <summary>
        /// �V���h�E�L���X�^�[�̃t���O��ݒ肷��
        /// </summary>
        void SetShadowCasterFlag(bool flag)
        {
            m_isShadowCaster = flag;
        }

    private:
        /// <summary>
        /// ���ʂ̏���������
        /// </summary>
        /// <param name="renderingEngine">�����_�����O�G���W��</param>
        /// <param name="tkmFilePath">tkm�t�@�C���p�X</param>
        void InitCommon(RenderingEngine& renderingEngine, const char* tkmFilePath);

    private:
        RenderingEngine* m_renderingEngine = nullptr;   //�����_�����O�G���W��
        Model m_zprepassModel;                  // ZPrepass�ŕ`�悳��郂�f��
        Model m_forwardRenderModel;             // �t�H���[�h�����_�����O�̕`��p�X�ŕ`�悳��郂�f��
        Model m_renderToGBufferModel;           // RenderToGBuffer�ŕ`�悳��郂�f��
        Model m_shadowModels[NUM_DEFERRED_LIGHTING_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP];	//�V���h�E�}�b�v�ɕ`�悷�郂�f��
        bool m_isShadowCaster = false;          // �V���h�E�L���X�^�[�t���O
    };
}
