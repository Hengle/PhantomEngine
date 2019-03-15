#include "OpenGLApplication.h"
#include "OpenGL/OpenGLGraphicsManager.h"
#include "AssetLoadManager.h"
#include "SceneManager.h"
#include "SceneBaseObject.h"
#include <unordered_map>

namespace Phantom {
	//���ó��������ռ��ڹ�������ȫ�ֱ���
    GfxConfiguration config(8, 8, 8, 8, 24, 8, 0, 960, 540, "Game  (Windows OpenGL)");
    IApplication* g_pApp                = static_cast<IApplication*>(new OpenGLApplication(config));
    GraphicsManager* g_pGraphicsManager = static_cast<GraphicsManager*>(new OpenGLGraphicsManager);
    AssetLoadManager*     g_pAssetLoader     = static_cast<AssetLoadManager*>(new AssetLoadManager);
	SceneManager*     g_pSceneManager     = static_cast<SceneManager*>(new SceneManager);

	std::unordered_map<std::string, std::shared_ptr<SceneBaseObject>> g_SceneObjects;
}

