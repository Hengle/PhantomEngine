#pragma once 
#include "mat4.h"
#include "vector.h"
#include "SceneGeometryNode.h"
namespace Phantom {

typedef maths::vec4 float4;
typedef maths::vec3 float3;
typedef maths::vec2 float2;
typedef maths::mat4x4 mat4x4f;
	//ÿ��Ⱦ���γ���
	struct ConstantsPerBatch 
	{ 
		maths::mat4x4 modelMatrix;
	};
	const size_t kSizeOfFrameConstantBuffer = 256;
	const size_t kSizeOfBatchConstantBuffer = 256;
	//��Ⱦ��ÿ֡�ڹ̶�����
	struct ConstantsPerFrame  
	{
		mat4x4f  	viewMatrix;						// 64 bytes
		mat4x4f  	projectionMatrix;				// 64 bytes
		float4   	camPos;							// 16 bytes
	};

	//��Ⱦ����������
	struct ContextPerDrawBatch:ConstantsPerBatch {
		uint32_t batchIndex;
		std::shared_ptr<SceneGeometryNode> node;
		int32_t diffuseMap = -1;
		virtual ~ContextPerDrawBatch() = default;
	};

	//��Ⱦÿ֡��������
	struct ContextPerFrame : ConstantsPerFrame {
	};

	//ÿ֡�����ݼ���
	struct Frame {
		ContextPerFrame frameContext;
		std::vector<std::shared_ptr<ContextPerDrawBatch>> batchContexts;
	};
}