#include "stdafx.h"
#include "Bitmap.h"
#include <fstream>

using namespace std;


/// <summary>
/// �R���X�g���N�^
/// </summary>
Bitmap::Bitmap()
{
}

/// <summary>
/// �f�X�g���N�^�B
/// </summary>
Bitmap::~Bitmap()
{
}

/// <summary>
/// Bitmap�����[�h�B
/// Bitmap�̉𑜓x��512�~512�ł���K�v������܂��B
/// </summary>
/// <param name="filePath">�ǂݍ��ރt�@�C���p�X�B</param>
/// <returns>
/// �ǂݍ��݂ɐ���������true�A
/// ���s������false��Ԃ��܂��B
/// </returns>
bool Bitmap::Load(const char* filePath)
{
	//�t�@�C���X�g���[�����쐬�B
	ifstream fs(filePath, std::ios::binary);

	if (fs.fail() == true) {
		//�t�@�C���̓ǂݍ��݂Ɏ��s�����B
		return false;
	}
	//�t�@�C���T�C�Y���v�Z����B
	fs.seekg(0, fstream::end);		//�t�@�C���̏I�[�Ɉړ�����B
	uint32_t endPos = fs.tellg();	//���݂̃t�@�C���̈ʒu���擾�B
	fs.clear();
	fs.seekg(0, fstream::beg);		//�t�@�C���̐擪�ɖ߂ǂ�B
	uint32_t begPos = fs.tellg();	//���݂̃t�@�C���̈ʒu���擾�B
	uint32_t fileSize = endPos - begPos;	//����-�K���Ńt�@�C���T�C�Y���v�Z�B

	//�ǂݍ��ނ��B
	char* buf = new char[fileSize];
	fs.read(buf, fileSize);

	//�t�@�C���w�b�_�[�̃A�h���X�����B
	BITMAPFILEHEADER* header = (BITMAPFILEHEADER*)buf;
	//�t�@�C�����w�b�_�[�̃A�h���X�����B
	BITMAPINFOHEADER* infoHeader = (BITMAPINFOHEADER*)(buf + sizeof(BITMAPFILEHEADER));
	//�ق�Ƃ͐F�X�`�F�b�N������̂����A
	//�t�H�[�}�b�g��24bit�J���[�ŁA�𑜓x��512�~512�Ō��ߑł��������I
	//�K���ɃC���[�W�ɃW�����v�B
	char* pImage = buf + header->bfOffBits;

	//�r�b�g�}�b�v�̏��������o�ϐ��ɃR�s�[�B
	memcpy(&m_bitmapHeader, header, sizeof(m_bitmapHeader));
	memcpy(&m_bitmapInfoHeader, infoHeader, sizeof(m_bitmapInfoHeader));
	//RGB24�r�b�g�����l�����ĂȂ���B
	m_numPixel = m_bitmapInfoHeader.biWidth * m_bitmapInfoHeader.biHeight;
	m_imageSizeInBytes = m_numPixel * sizeof(SRgbRow);
	m_imageRow = std::make_unique<SRgbRow[]>(m_numPixel);
	memcpy(m_imageRow.get(), pImage, m_imageSizeInBytes);

	m_image = std::make_unique<SRgba[]>(m_numPixel);
	for (int i = 0; i < m_numPixel; i++) {
		m_image[i].r = m_imageRow[i].r;
		m_image[i].g = m_imageRow[i].g;
		m_image[i].b = m_imageRow[i].b;
		m_image[i].a = 255;
	}
	


	//�R�s�[���I������̂ł�������Ȃ��̂ŁA�������폜�B
	//�������B
	delete[] buf;
	//�t�@�C�������B
	fs.close();

	//�ǂݍ��݂����������̂�true��Ԃ��B
	return true;
}
/// <summary>
/// Bitmap�̕ۑ��B
/// </summary>
/// <param name="filePath">�ۑ�����t�@�C���p�X�B</param>
/// <returns>
/// �ۑ��ɐ���������true�A
/// ���s������false��Ԃ��B
/// </returns>
bool Bitmap::Save(const char* filePath)
{
	ofstream fs(filePath, std::ios::binary);

	if (fs.fail() == true) {
		//�t�@�C���̃I�[�v���Ɏ��s�����B
		return false;
	}

	//�������߁\�B
	fs.write((char*)&m_bitmapHeader, sizeof(m_bitmapHeader));
	fs.write((char*)&m_bitmapInfoHeader, sizeof(m_bitmapInfoHeader));
	std::unique_ptr< SRgbRow[]> imageRow = std::make_unique<SRgbRow[]>(m_numPixel);
	for (int i = 0; i < m_numPixel; i++) {
		imageRow[i].r = m_image[i].r;
		imageRow[i].g = m_image[i].g;
		imageRow[i].b = m_image[i].b;
	}

	fs.write((char*)imageRow.get(), m_imageSizeInBytes);

	fs.close();
	//�ۑ������������̂ŁAtrue��Ԃ��B
	return true;
}
