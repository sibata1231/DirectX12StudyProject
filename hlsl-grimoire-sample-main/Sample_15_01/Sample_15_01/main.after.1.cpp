#include "stdafx.h"
#include "system/system.h"
#include "sub.h"


struct OutputData
{
    float avarageScore; // ���ϓ_
    float maxScore;     // �ō����_
    float minScore;     // �ŏ����_

    // step-3 �o�͍\���̂Ƀ����o�[��ǉ�����
    int totalScore;     // ���v�_
};

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

    // �R���s���[�g�V�F�[�_�̃��[�h
    Shader cs;
    cs.LoadCS("Assets/shader/sample.fx", "CSMain");

    RootSignature rs;
    InitRootSignature(rs, cs);

    PipelineState pipelineState;
    InitPipelineState(rs, pipelineState, cs);

    // ���̓f�[�^���󂯎��o�b�t�@�[���쐬
    int inputData[] = {
        20, 30, 40
    };

    StructuredBuffer inputSB;
    inputSB.Init(sizeof(int), 3, inputData);

    // �o�̓f�[�^���󂯎��o�b�t�@�[���쐬
    RWStructuredBuffer outputSb;
    outputSb.Init(sizeof(OutputData), 1, nullptr);

    // ���̓f�[�^�Əo�̓f�[�^���f�B�X�N���v�^�q�[�v�ɓo�^����
    DescriptorHeap ds;
    ds.RegistShaderResource(0, inputSB);
    ds.RegistUnorderAccessResource(0, outputSb);
    ds.Commit();
    //////////////////////////////////////
    // ���������s���R�[�h�������̂͂����܂ŁI�I�I
    //////////////////////////////////////
    auto& renderContext = g_graphicsEngine->GetRenderContext();

    // ��������Q�[�����[�v
    while (DispatchWindowMessage())
    {
        // �t���[���J�n
        g_engine->BeginFrame();
        //////////////////////////////////////
        // ���͂��鐬�уf�[�^�������_���ɐ�������
        //////////////////////////////////////
        for (int i = 0; i < 3; i++)
        {
            inputData[i] = rand() % 101;
        }

        inputSB.Update(inputData);

        //////////////////////////////////////
        // ��������DirectCompute�ւ̃f�B�X�p�b�`����
        //////////////////////////////////////
        renderContext.SetComputeRootSignature(rs);
        renderContext.SetPipelineState(pipelineState);
        renderContext.SetComputeDescriptorHeap(ds);
        renderContext.Dispatch(1, 1, 1);

        // �t���[���I��
        g_engine->EndFrame();

        // ���ϓ_�A�ō����_�A�Œᓾ�_��\������
        char text[256];
        OutputData* outputData = (OutputData*)outputSb.GetResourceOnCPU();
        // step-4 ���v�_��\������
        sprintf(
            text,
            "�P�l�� = %d\n" \
            "�Q�l�� = %d\n" \
            "�R�l�� = %d\n" \
            "���ϓ_ = %0.2f\n" \
            "�ō����_=%0.2f\n" \
            "�Œᓾ�_=%0.2f\n" \
            "���v�_=%d\n", // �����ǉ�
            inputData[0],
            inputData[1],
            inputData[2],
            outputData->avarageScore,
            outputData->maxScore,
            outputData->minScore,
            outputData->totalScore // ������ǉ�
        );
        MessageBoxA(nullptr, text, "���є��\", MB_OK);
    }
    return 0;
}

