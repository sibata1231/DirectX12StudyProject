#pragma once

#include <stdint.h>

#pragma pack(2)
/// <summary>
/// Bitmap�t�@�C���̃w�b�_�[�\���́B
/// </summary>
/*struct BITMAPFILEHEADER {
	uint16_t bfType;		//�t�@�C���^�C�v�B�ʏ��BM�B
	uint32_t bfSize;		//�t�@�C���T�C�Y(byte)
	uint16_t bfReserved1;	//�\��̈�B��ɂO�B
	uint16_t bfReserved2;	//�\��̈�B��ɂO�B
	uint32_t bfOffBits;		//�t�@�C���̐擪����摜�f�[�^�܂ł̃I�t�Z�b�g(byte)
};
/// <summary>
/// BITMAP���w�b�_�B
/// </summary>
struct BITMAPINFOHEADER {
	uint32_t biSize;			//���w�b�_�T�C�Y�B
	uint32_t biWidth;			//�摜�̕��B
	uint32_t biHeight;			//�摜�̍����B
	uint16_t biPlanes;			//�v���[�����B
	uint16_t biBitCount;		//�F�r�b�g���B1,4,8,(16),24,32�B
	uint32_t biCompression;		//���k�`���B0,1,2,3
	uint32_t biSizeImage;		//�摜�f�[�^�T�C�Y�B
	uint32_t biXPixPerMeter;	//�����𑜓x�B[dot/m]
	uint32_t biYPixPerMeter;	//�����𑜓x�B[dot/m]
	uint32_t biClrUsed;			//�i�[�p���b�g���B�O�̏ꍇ������B
	uint32_t biCirImportant;	//�d�v�F���B�O�̏ꍇ������B
};
*/
#pragma pack()
/// <summary>
/// Bitmap�N���X�B
/// </summary>
class Bitmap
{
public:
	struct SRgbRow {
		unsigned char r;	//R�����Bunsigned�͕����Ȃ���\���C���q�B
		unsigned char g;	//G�����Bunsigned�͕����Ȃ���\���C���q�B
		unsigned char b;	//b�����Bunsigned�͕����Ȃ���\���C���q�B
	};
	/// <summary>
	/// 1�h�b�g�̉�f��\���\���́B
	/// </summary>
	struct SRgba {
		unsigned char r;	//R�����Bunsigned�͕����Ȃ���\���C���q�B
		unsigned char g;	//G�����Bunsigned�͕����Ȃ���\���C���q�B
		unsigned char b;	//b�����Bunsigned�͕����Ȃ���\���C���q�B
		unsigned char a;	//a�����Bunsigned�͕����Ȃ���\���C���q�B
	};

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	Bitmap();

	/// <summary>
	/// �f�X�g���N�^�B
	/// </summary>
	~Bitmap();

	/// <summary>
	/// Bitmap�����[�h�B
	/// Bitmap�̉𑜓x��512�~512�ł���K�v������܂��B
	/// </summary>
	/// <param name="filePath">�ǂݍ��ރt�@�C���p�X�B</param>
	/// <returns>
	/// �ǂݍ��݂ɐ���������true�A
	/// ���s������false��Ԃ��܂��B
	/// </returns>
	bool Load(const char* filePath);

	/// <summary>
	/// Bitmap�̕ۑ��B
	/// </summary>
	/// <param name="filePath">�ۑ�����t�@�C���p�X�B</param>
	/// <returns>
	/// �ۑ��ɐ���������true�A
	/// ���s������false��Ԃ��B
	/// </returns>
	bool Save(const char* filePath);
	/// <summary>
	/// �C���[�W�̃T�C�Y���擾
	/// </summary>
	/// <returns></returns>
	int GetImageSizeInBytes() const
	{
		return m_imageSizeInBytes;
	}
	/// <summary>
	/// 1�s�N�Z���̃T�C�Y�B
	/// </summary>
	/// <returns></returns>
	int GetPixelSizeInBytes() const
	{
		return 4;
	}
	/// <summary>
	/// �s�N�Z���̐����擾�B
	/// </summary>
	/// <returns></returns>
	int GetNumPixel() const
	{
		return m_numPixel;
	}
	/// <summary>
	/// �摜�f�[�^�̐擪�A�h���X���擾�B
	/// </summary>
	/// <returns></returns>
	SRgba* GetImageAddress() const
	{
		return m_image.get();
	}
	void Copy(void* image)
	{
		memcpy(m_image.get(), image, m_numPixel*4);
	}
	/// <summary>
	/// �������擾�B
	/// </summary>
	/// <returns></returns>
	LONG GetWidth() const
	{
		return m_bitmapInfoHeader.biWidth;
	}
	/// <summary>
	/// �������擾�B
	/// </summary>
	/// <returns></returns>
	LONG GetHeight() const
	{
		return m_bitmapInfoHeader.biHeight;
	}
private:
	
	BITMAPFILEHEADER m_bitmapHeader;		//�r�b�g�}�b�v�t�@�C���̃w�b�_�[�B
											//Load�֐������s����ƁA�ǂݍ��񂾃r�b�g�}�b�v�摜�̃w�b�_�[���R�s�[����܂��B
	BITMAPINFOHEADER m_bitmapInfoHeader;	//�r�b�g�}�b�v�t�@�C���̏��w�b�_�[�B
											//Load�֐������s����ƁA�ǂݍ��񂾃r�b�g�}�b�v�̏��w�b�_�[���R�s�[����܂��B
	std::unique_ptr<SRgba[]> m_image;		//�摜�̃C���[�W�B
	int m_imageSizeInBytes = 0;				//�摜�̃T�C�Y�B
	int m_numPixel = 0;						//�s�N�Z�����B
	std::unique_ptr< SRgbRow[]> m_imageRow;
private:
};

