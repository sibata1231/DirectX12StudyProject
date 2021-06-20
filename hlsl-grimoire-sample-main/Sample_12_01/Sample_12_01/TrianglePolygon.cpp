#include "stdafx.h"
#include "TrianglePolygon.h"

void TrianglePolygon::Init(RootSignature& rs)
{
	//�V�F�[�_�[�����[�h�B
	LoadShaders();
	//�p�C�v���C���X�e�[�g���쐬�B
	InitPipelineState(rs);
	//���_�o�b�t�@���쐬�B
	InitVertexBuffer();
	//�C���f�b�N�X�o�b�t�@���쐬�B
	InitIndexBuffer();
}
void TrianglePolygon::Draw(RenderContext& rc)
{
	//�p�C�v���C���X�e�[�g��ݒ�B
	rc.SetPipelineState(m_pipelineState);
	//�v���~�e�B�u�̃g�|���W�[��ݒ�B
	rc.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//���_�o�b�t�@��ݒ�B
	rc.SetVertexBuffer(m_vertexBuffer);
	//�C���f�b�N�X�o�b�t�@��ݒ�B
	rc.SetIndexBuffer(m_indexBuffer);
	//�h���[�R�[��
	rc.DrawIndexed(3);
}
void TrianglePolygon::LoadShaders()
{
	m_vertexShader.LoadVS("Assets/shader/sample.fx", "VSMain");
	m_pixelShader.LoadPS("Assets/shader/sample.fx", "PSMain");
}
void TrianglePolygon::InitPipelineState(RootSignature& rs)
{
	// ���_���C�A�E�g���`����B
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	//�p�C�v���C���X�e�[�g���쐬�B
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { 0 };
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = rs.Get();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vertexShader.GetCompiledBlob());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_pixelShader.GetCompiledBlob());
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_pipelineState.Init(psoDesc);
}
void TrianglePolygon::InitVertexBuffer()
{
	SimpleVertex vertices[] = {
		{
			{-0.5f, -0.5f, 0.0f},
			{ 1.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f }
		},
		{
			{ 0.0f, 0.5f, 0.0f },
			{ 0.0f, 1.0f, 0.0f },
			{ 0.5f, 1.0f }
		},
		{
			{ 0.5f, -0.5f, 0.0f },
			{ 0.0f, 0.0f, 1.0f },
			{1.0f, 0.0f}
		}
	};

	m_vertexBuffer.Init(sizeof(vertices), sizeof(vertices[0]));
	m_vertexBuffer.Copy(vertices);
}
void TrianglePolygon::InitIndexBuffer()
{
	uint16_t indices[] = {
		0,1,2
	};
	
	m_indexBuffer.Init(sizeof(indices), 2);
	m_indexBuffer.Copy(indices);

}