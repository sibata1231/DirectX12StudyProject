#pragma once
namespace myRenderer {
    class Bloom
    {
    public:
        void Init(RenderTarget& mainRenderTarget);
        void Render(RenderContext& rc, RenderTarget& mainRenderTarget);
    private:
        RenderTarget m_luminanceRenderTarget;	//�P�x���o�p�̃����_�����O�^�[�Q�b�g
        Sprite m_luminanceSprite;				//�P�x���o�p�̃X�v���C�g
        GaussianBlur m_gaussianBlur[4];			//�K�E�V�A���u���[
        Sprite m_finalSprite;					//�ŏI�����p�̃X�v���C�g
    };
}

