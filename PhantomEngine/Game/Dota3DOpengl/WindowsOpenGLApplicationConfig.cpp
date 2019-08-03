#include "OpenGLApplication.h"
#include "OpenGL/OpenGLGraphicsManager.h"
#include "common/AssetLoadManager.h"
#include "common/SceneManager.h"
#include "scene/SceneBaseObject.h"
#include <unordered_map>
#include "common/GfxConfiguration.h"
#include "interface/IApplication.h"
#include "common/BehaviourManager.h"
#include "common/InputManager.h"
#include "common/DebugManager.h"
namespace Phantom {
	//���ó��������ռ��ڹ�������ȫ�ֱ���
    GfxConfiguration config(8, 8, 8, 8, 24, 8, 0, 1024, 576, "Phantom Game  (Windows OpenGL)");
    IApplication* g_pApp                = static_cast<IApplication*>(new OpenGLApplication(config));
    GraphicsManager* g_pGraphicsManager = static_cast<GraphicsManager*>(new OpenGLGraphicsManager);
    AssetLoadManager*     g_pAssetLoader     = static_cast<AssetLoadManager*>(new AssetLoadManager);
	SceneManager*     g_pSceneManager     = static_cast<SceneManager*>(new SceneManager);

	InputManager*    g_pInputManager = static_cast<InputManager*>(new InputManager);
	DebugManager*    g_pDebugManager = static_cast<DebugManager*>(new DebugManager);

}

