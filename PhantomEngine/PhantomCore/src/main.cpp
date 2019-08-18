#include <stdio.h>
#include <chrono>
#include <thread>
#include "interface/IApplication.h"
#include "common/GraphicsManager.h"
#include "common/AssetLoadManager.h"
#include "common/BehaviourManager.h"
#include "common/SceneManager.h"
#include "common/InputManager.h"
// #include "MemoryManager.h"
#include "common/AnimationManager.h"
#include "utils/Timer.h"
#include "common/DebugManager.h"

using namespace Phantom;
using namespace std;

namespace Phantom {
	extern IApplication*    g_pApp;
    extern DebugManager* g_pDebugManager;
	extern AssetLoadManager* g_pAssetLoader;
    //extern MemoryManager*   g_pMemoryManager;
    extern GraphicsManager* g_pGraphicsManager;
    extern SceneManager* g_pSceneManager;
    extern BehaviourManager* g_pBehaviourManager;
    extern InputManager* g_pInputManager;
}


int main(int argc, char** argv) {


	int ret;

	if ((ret = g_pApp->Init()) != 0) {
		printf("App Init failed, will exit now.");
		return ret;
	}
	// create the main window
	g_pApp->CreateMainWindow();
    
	// if ((ret = g_pMemoryManager->Init()) != 0) {
	// 	printf("Memory Manager Init failed, will exit now.");
	// 	return ret;
	// }
	if ((ret = g_pAssetLoader->Init()) != 0) {
		cerr << "Failed. err =  g_pAssetLoader initialize failed , will exit now." << ret;
		return ret;
	}
	if ((ret = g_pSceneManager->Init()) != 0) {
		cerr << "Failed. err =  g_pSceneManager initialize failed , will exit now." << ret;
		return ret;
	}
	
	if ((ret = g_pBehaviourManager->Init()) != 0) {
		printf("g_pBehaviourManager  Init failed, will exit now.");
		return ret;
	}
	if ((ret = g_pInputManager->Init()) != 0) {
		printf("g_pInputManager  Init failed, will exit now.");
		return ret;
	}

	if ((ret = AnimationManager::GetInstance().Init()) != 0) {
		printf("AnimationManager  Init failed, will exit now.");
		return ret;
	}
	if ((ret = g_pGraphicsManager->Init()) != 0) {
		printf("Graphics Manager Init failed, will exit now.");
		return ret;
	}

#ifdef _DEBUG
	if ((ret = g_pDebugManager->Init()) != 0) {
		cerr << "g_pDebugManager Failed. err =" << ret;
		return ret;
	}
#endif
	g_pSceneManager->LoadScene();


	Timer timer;
	float secondCount = 0.0f;
	float lastFrameTime = 0.0f;
	unsigned int  frames = 0;
	while (!g_pApp->IsQuit()) {
		float now = timer.ElapsedMillis();
		float deltaTime = now - lastFrameTime;
		lastFrameTime = now;

		g_pApp->Tick();
		// g_pMemoryManager->Tick(deltaTime);
		g_pAssetLoader->Tick();
		AnimationManager::GetInstance().Tick(deltaTime);
		g_pSceneManager->Tick();
		g_pBehaviourManager->Tick();
		g_pInputManager->Tick();
#ifndef OS_MACOS
        g_pGraphicsManager->Tick(); //todo macos tick on metalView
#endif

#ifdef _DEBUG
		g_pDebugManager->Tick();
#endif
		frames++;
		if (timer.Elapsed() - secondCount > 1.0f)
		{
			secondCount += 1.0f;
			string str = std::to_string(frames);
			g_pGraphicsManager->DrawString(GUI::FrameGuiIdx,20, 400, "fps:" + str);
//            printf("%d\n", frames);
		/* ����N��Ĭ�Ͽ����Զ�ͬ���������ֶ��������ã������Կ����ã������60֡�� A��Ĭ�Ϲرա�*/
			frames = 0;
		}
//        std::this_thread::sleep_for(std::chrono::milliseconds(int(1000/60 - deltaTime)));
	}
#ifdef _DEBUG
	g_pDebugManager->Shutdown();
#endif
    // g_pMemoryManager->Shutdown();
	AnimationManager::GetInstance().Shutdown();
	g_pInputManager->Shutdown();
	g_pBehaviourManager->Shutdown();
    g_pGraphicsManager->Shutdown();
	g_pSceneManager->Shutdown();
	g_pAssetLoader->Shutdown();
	g_pApp->Shutdown();
	delete g_pInputManager;
	delete g_pBehaviourManager;
	delete g_pGraphicsManager;
	delete g_pSceneManager;
	delete g_pAssetLoader;
	delete g_pApp;
	return 0;
}
