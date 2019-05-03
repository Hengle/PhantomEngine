#include "Scene.h"
#include "SceneObjectVertexArray.h"
#include "SceneOjbectTypeDef.h"
#include "SceneBaseNode.h"
#include "OpengexParser.h"
#include "BehaviourManager.h"

using namespace Phantom;
using namespace std;


Phantom::Scene::Scene(const char * name):SceneRootGraph(new SceneBaseNode(name))
{
	camera = make_shared<CameraNode>();
	m_pDefaultMat = std::make_shared<SceneObjectMaterial>("default");
}

Phantom::Scene::~Scene()
{
}

const shared_ptr<SceneObjectMaterial> Scene::GetFirstMaterial() const
{
	return (Materials.empty() ? nullptr : Materials.cbegin()->second);
}

//�Ҳ�������Ĭ�ϲ��ʡ�
const std::shared_ptr<SceneObjectMaterial> Phantom::Scene::GetMaterial(const std::string & matName) const
{
	unordered_map<string, shared_ptr<SceneObjectMaterial>> ::const_iterator  iter = Materials.find(matName);
	if (iter != Materials.end())
	{
		return iter->second;
	}
	else
	{
		return m_pDefaultMat;
	}
}

void Phantom::Scene::LoadTextures(void)
{
	for (auto mat : Materials)
	{
		if (auto ptr = mat.second)
		{
			ptr->LoadTextures();
		}
	}
}
