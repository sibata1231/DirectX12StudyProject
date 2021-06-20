#pragma once

#include "CascadeShadowMapMatrix.h"

namespace myRenderer
{
    namespace shadow
    {
        /// <summary>
        /// �V���h�E�}�b�v�ւ̕`�揈��
        /// </summary>
        class ShadowMapRender
        {
        public:
            /// <summary>
            /// ������
            /// </summary>
            void Init();
            /// <summary>
        /// �V���h�E�}�b�v�ɕ`�悷�郂�f����ǉ�
        /// </summary>
        /// <param name="model0">�ߌi�p�̃V���h�E�}�b�v�ɕ`�悷�郂�f��</param>
        /// <param name="model1">���i�p�̃V���h�E�}�b�v1�ɕ`�悷�郂�f��</param>
        /// <param name="model2">���i�p�̃V���h�E�}�b�v2�ɕ`�悷�郂�f��</param>
            void Add3DModel(
                Model& model0,
                Model& model1,
                Model& model2)
            {
                m_modelsArray[0].push_back(&model0);
                m_modelsArray[1].push_back(&model1);
                m_modelsArray[2].push_back(&model2);
            }
            /// <summary>
            /// �`��
            /// </summary>
            /// <param name="rc">�����_�����O�R���e�L�X�g</param>
            /// <param name="lightDirection">���C�g�̕���</param>
            void Render(RenderContext& rc, Vector3& lightDirection);
            /// <summary>
            /// �V���h�E�}�b�v���擾
            /// </summary>
            /// <param name="areaNo">�G���A�ԍ�</param>
            /// <returns></returns>
            Texture& GetShadowMap(int areaNo)
            {
                return m_shadowMaps[areaNo].GetRenderTargetTexture();
            }
            /// <summary>
            /// ���C�g�r���[�v���W�F�N�V�����s����擾
            /// </summary>
            /// <returns></returns>
            const Matrix& GetLVPMatrix(int areaNo) const
            {
                return m_cascadeShadowMapMatrix.GetLightViewProjectionCropMatrix(areaNo);
            }
        private:
            CascadeShadowMapMatrix m_cascadeShadowMapMatrix;	//�J�X�P�[�h�V���h�E�}�b�v�̍s��������I�u�W�F�N�g
            RenderTarget m_shadowMaps[NUM_SHADOW_MAP];			//�V���h�E�}�b�v
            std::vector<Model*> m_modelsArray[NUM_SHADOW_MAP];	//�V���h�E�}�b�v�ɕ`�悷�郂�f���̔z��

        };
    }
}

