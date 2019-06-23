#include <stdio.h>
#include "interface/IApplication.h"
#include "GraphicsManager.h"
#include "AssetLoadManager.h"
#include "BehaviourManager.h"
#include "SceneManager.h"
#include "InputManager.h"
// #include "MemoryManager.h"
#include "Timer.h"

using namespace Phantom;
using namespace std;

namespace Phantom {
	extern IApplication*    g_pApp;
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
#ifndef  OS_MACOS
	g_pApp->CreateMainWindow();
#endif
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
	if ((ret = g_pGraphicsManager->Init()) != 0) {
		printf("Graphics Manager Init failed, will exit now.");
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

	Timer timer;
	float timeCount = 0.0f;
	unsigned int  frames = 0;
	while (!g_pApp->IsQuit()) {
		g_pApp->Tick();
		//float now = timer.ElapsedMillis();
        // g_pMemoryManager->Tick();
		g_pAssetLoader->Tick();
		g_pSceneManager->Tick();
        g_pGraphicsManager->Tick();
		g_pBehaviourManager->Tick();
		g_pInputManager->Tick();

		frames++;
		if (timer.Elapsed() - timeCount > 1.0f)
		{
			timeCount += 1.0f;
			string str = std::to_string(frames);
			g_pGraphicsManager->DrawString(GUI::FrameGuiIdx,20, 400, "fps:" + str);
		/* ����N��Ĭ�Ͽ����Զ�ͬ���������ֶ��������ã������Կ����ã������60֡�� A��Ĭ�Ϲرա�*/
			frames = 0;
		}
	}

    g_pGraphicsManager->Shutdown();
    // g_pMemoryManager->Shutdown();
	g_pApp->Shutdown();
	g_pAssetLoader->Shutdown();
	delete g_pGraphicsManager;
	delete g_pAssetLoader;
	delete g_pInputManager;
	delete g_pBehaviourManager;
	delete g_pApp;
	return 0;
}
