#include "OpenGLApplication.h"
#include "OpenGL/OpenGLGraphicsManager.h"
#include "AssetLoadManager.h"
#include "SceneManager.h"
#include "SceneBaseObject.h"
#include <unordered_map>
#include "GfxConfiguration.h"
#include "IApplication.h"
#include "BehaviourManager.h"
#include "InputManager.h"
namespace Phantom {
	//���ó��������ռ��ڹ�������ȫ�ֱ���
    GfxConfiguration config(8, 8, 8, 8, 24, 8, 0, 1024, 576, "Phantom Game  (Windows OpenGL)");
    IApplication* g_pApp                = static_cast<IApplication*>(new OpenGLApplication(config));
    GraphicsManager* g_pGraphicsManager = static_cast<GraphicsManager*>(new OpenGLGraphicsManager);
    AssetLoadManager*     g_pAssetLoader     = static_cast<AssetLoadManager*>(new AssetLoadManager);
	SceneManager*     g_pSceneManager     = static_cast<SceneManager*>(new SceneManager);

	InputManager*    g_pInputManager = static_cast<InputManager*>(new InputManager);

}

