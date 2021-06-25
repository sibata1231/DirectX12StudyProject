#include "stdafx.h"
#include "system/system.h"

// ���_�\����
struct SimpleVertex
{
    float x, y, z; // ���_���W
};

// �֐��錾
void InitRootSignature(RootSignature& rs);
void InitPipelineState(PipelineState& pipelineState, RootSignature& rs, Shader& vs, Shader& ps);

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

    // 1. ���[�g�V�O�l�`�����쐬
    RootSignature rootSignature;
    InitRootSignature(rootSignature);

    // 2. �V�F�[�_�[�����[�h
    Shader vs, ps;
    vs.LoadVS("Assets/shader/sample.after_3.fx", "VSMain");
    ps.LoadPS("Assets/shader/sample.fx", "PSMain");

    // 3. �p�C�v���C���X�e�[�g���쐬
    PipelineState pipelineState;
    InitPipelineState(pipelineState, rootSignature, vs, ps);

    // 4. �O�p�`�̒��_�o�b�t�@���쐬
    // ���_�z����`
    SimpleVertex vertices[] = {
        { -0.5f, -0.5f, 0.0f },
        { 0.0f, 0.5f, 0.0f },
        { 0.5f, -0.5f, 0.0f },
    };

    VertexBuffer triangleVB;
    triangleVB.Init(sizeof(vertices), sizeof(vertices[0]));
    triangleVB.Copy(vertices);

    // 5. �O�p�`�̃C���f�b�N�X�o�b�t�@���쐬
    // �C���f�b�N�X�z��
    uint16_t indices[] = {
        0,1,2
    };
    IndexBuffer triangleIB;
    triangleIB.Init(sizeof(indices), 2);
    triangleIB.Copy(indices);

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

        // 1. ���[�g�V�O�l�`����ݒ�
        renderContext.SetRootSignature(rootSignature);

        // 2. �p�C�v���C���X�e�[�g��ݒ�
        renderContext.SetPipelineState(pipelineState);

        // 3. �v���~�e�B�u�̃g�|���W�[��ݒ�
        renderContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // 4. ���_�o�b�t�@��ݒ�
        renderContext.SetVertexBuffer(triangleVB);

        // 5. �C���f�b�N�X�o�b�t�@��ݒ�
        renderContext.SetIndexBuffer(triangleIB);

        // 6. �h���[�R�[��
        renderContext.DrawIndexed(3);

        /////////////////////////////////////////
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
// �p�C�v���C���X�e�[�g�̏�����
void InitPipelineState(PipelineState& pipelineState, RootSignature& rs, Shader& vs, Shader& ps)
{

    // ���_���C�A�E�g���`����
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    // �p�C�v���C���X�e�[�g���쐬
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { 0 };
    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.pRootSignature = rs.Get();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(vs.GetCompiledBlob());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(ps.GetCompiledBlob());
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
    pipelineState.Init(psoDesc);
}
