#include "GraphicsManager.h"
#include <iostream>
#include "InputManager.h"
#include "SceneManager.h"
using namespace std;
using namespace Phantom::maths;
float PI = 3.1415926f;

int Phantom::GraphicsManager::Init()
{
	Inited = true;
	return 0;
}

void Phantom::GraphicsManager::Shutdown()
{
}

void Phantom::GraphicsManager::Tick()
{
	UpdateConstants();
	Draw();
}
void Phantom::GraphicsManager::CalculateCameraMatrix() {
	shared_ptr<CameraNode> camera = g_pSceneManager->GetSceneForRendering().camera;
	camera->CalculateVPMatrix();
}

void Phantom::GraphicsManager::UpdateConstants() {
	bindShader();

	auto& frame = m_Frame;

	for (auto& pDbc : frame.batchContexts)
	{
		pDbc->modelMatrix = *pDbc->node->GetCalculatedTransform();
	}
	//������������ת������
	//#todo
	CalculateCameraMatrix();

	SetPerFrameConstants(frame.frameContext);
	SetPerFrameLight(frame.light);
	SetPerBatchConstants(frame.batchContexts);
}
