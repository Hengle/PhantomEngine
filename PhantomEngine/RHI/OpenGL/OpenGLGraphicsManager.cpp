#include "glad/glad.h"
#include <iostream>
#include <fstream>
#include "OpenGLGraphicsManager.h"
#include "AssetLoadManager.h"
#include "SceneManager.h"
#include "GfxConfiguration.h"
#include "IApplication.h"
#include "Scene.h"
#include <unordered_map>
#include "SceneObjectGeometry.h"
#include "SceneObjectVertexArray.h"
#include "SceneObjectIndexArray.h"
#include "GfxStruct.h"

const char VS_SHADER_SOURCE_FILE[] = "Resources/shaders/vert_light.shader";
const char PS_SHADER_SOURCE_FILE[] = "Resources/shaders/frag_light.shader";

using namespace std;
using namespace Phantom::maths;

extern struct gladGLversionStruct GLVersion;

namespace Phantom {

	extern AssetLoadManager* g_pAssetLoader;
	extern SceneManager* g_pSceneManager;
	extern IApplication* g_pApp;

	bool OpenGLGraphicsManager::InitializeBuffers()
	{
		uint32_t batchCounter = 0;
		auto& scene = g_pSceneManager->GetSceneForRendering();
		std::unordered_map<std::string, std::shared_ptr<SceneObjectGeometry>> geoOjbects = scene.GeometryOjbects;
		for (const auto& iter : scene.GeometryNodes)
		{
			const auto& pGeometryNode = iter.second.lock();
			if (!pGeometryNode /*&& pGeometryNode->Visible()*/)
			{
				continue;
			}
			const std::string& gKey = pGeometryNode->GetSceneObjectRef();
			auto i = geoOjbects.find(gKey);
			if (i == geoOjbects.end())
			{
				continue;
			}
			const auto&pGeometry = i->second;
			const auto& pMesh = pGeometry->GetMesh().lock();
			
			//--- mesh ---- ���---------
			const auto vertexPropertiesCount = pMesh->GetVertexPropertiesCount();

			GLuint vertexArrayId;
			// Allocate an OpenGL vertex array object.
			glGenVertexArrays(1, &vertexArrayId);
			// Bind the vertex array object to store all the buffers and vertex attributes we create here.
			glBindVertexArray(vertexArrayId);

			GLuint vertexBufferId;

			// Generate an ID for the vertex buffer.
			for (GLuint i = 0; i < vertexPropertiesCount; i++)
			{
				if (i > 1) break;
				const SceneObjectVertexArray & vProArr = pMesh->GetVertexPropertyArray(i);
				const auto vProArrSize = vProArr.GetDataSize();
				const auto vProArrData = vProArr.GetData();

				glGenBuffers(1, &vertexBufferId);
				glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
				glBufferData(GL_ARRAY_BUFFER, vProArrSize, vProArrData, GL_STATIC_DRAW);

				glEnableVertexAttribArray(i);
				switch (vProArr.GetDataType()) {
				case VertexDataType::kVertexDataTypeFloat1:
					glVertexAttribPointer(i, 1, GL_FLOAT, false, 0, 0);
					break;
				case VertexDataType::kVertexDataTypeFloat2:
					glVertexAttribPointer(i, 2, GL_FLOAT, false, 0, 0);
					break;
				case VertexDataType::kVertexDataTypeFloat3:
					glVertexAttribPointer(i, 3, GL_FLOAT, false, 0, 0);
					break;
				case VertexDataType::kVertexDataTypeFloat4:
					glVertexAttribPointer(i, 4, GL_FLOAT, false, 0, 0);
					break;
#if !defined(OS_ANDROID) && !defined(OS_WEBASSEMBLY)
				case VertexDataType::kVertexDataTypeDouble1:
					glVertexAttribPointer(i, 1, GL_DOUBLE, false, 0, 0);
					break;
				case VertexDataType::kVertexDataTypeDouble2:
					glVertexAttribPointer(i, 2, GL_DOUBLE, false, 0, 0);
					break;
				case VertexDataType::kVertexDataTypeDouble3:
					glVertexAttribPointer(i, 3, GL_DOUBLE, false, 0, 0);
					break;
				case VertexDataType::kVertexDataTypeDouble4:
					glVertexAttribPointer(i, 4, GL_DOUBLE, false, 0, 0);
					break;
#endif
				default:
					assert(0);
				}
				//#todo buff id ��Ҫͳ�ƹ����Ա��ͷ��л������ͷ�
			}

			const auto indexGroupCount = pMesh->GetIndexGroupCount();

			uint32_t  mode;
			switch (pMesh->GetPrimitiveType())
			{
			case PrimitiveType::kPrimitiveTypePointList:
				mode = GL_POINTS;
				break;
			case PrimitiveType::kPrimitiveTypeLineList:
				mode = GL_LINES;
				break;
			case PrimitiveType::kPrimitiveTypeLineStrip:
				mode = GL_LINE_STRIP;
				break;
			case PrimitiveType::kPrimitiveTypeTriList:
				mode = GL_TRIANGLES;
				break;
			case PrimitiveType::kPrimitiveTypeTriStrip:
				mode = GL_TRIANGLE_STRIP;
				break;
			case PrimitiveType::kPrimitiveTypeTriFan:
				mode = GL_TRIANGLE_FAN;
				break;
			default:
				// ignore
				break;
			}
			GLuint indexBufferId = 0;
			int32_t indexCount = 0;
			for (GLuint i = 0; i < indexGroupCount; i++)
			{
				// Generate an ID for the index buffer.
				glGenBuffers(1, &indexBufferId);

				const SceneObjectIndexArray& index_array = pMesh->GetIndexArray(i);
				const auto index_array_size = index_array.GetDataSize();
				const auto index_array_data = index_array.GetData();

				// Bind the index buffer and load the index data into it.
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_array_size, index_array_data, GL_STATIC_DRAW);
				// Set the number of indices in the index array.
				indexCount = static_cast<int32_t>(index_array.GetIndexCount());
				uint32_t type;
				switch (index_array.GetIndexType())
				{
				case IndexDataType::kIndexDataTypeInt8:
					type = GL_UNSIGNED_BYTE;
					break;
				case IndexDataType::kIndexDataTypeInt16:
					type = GL_UNSIGNED_SHORT;
					break;
				case IndexDataType::kIndexDataTypeInt32:
					type = GL_UNSIGNED_INT;
					break;
				default:
					// not supported by OpenGL
					//cerr << "Error: Unsupported Index Type " << index_array << endl;
					//cerr << "Mesh: " << *pMesh << endl;
					//cerr << "Geometry: " << *pGeometry << endl;
					continue;
				}
				auto contextPerBatch = make_shared<OpenGLContextPerDrawBatch>();

				//materials
				size_t material_index = index_array.GetMaterialIndex();
				std::string material_key = pGeometryNode->GetMaterialRef(material_index);
				auto material = scene.GetMaterial(material_key);
				//const std::string & name = "material7";
				//auto material = scene.GetMaterial(name);
				if (material) {
					const auto & color = material->GetBaseColor();
					if (color.ValueMap) {
						const auto& texture = color.ValueMap->GetTextureImage();
						const auto& textureName = color.ValueMap->GetName();
						GLuint textureId;
						auto iter = m_textures.find(textureName);
						if (iter == m_textures.end())
						{
							glGenTextures(1, &textureId);
							glBindTexture(GL_TEXTURE_2D, textureId);
							glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, texture->Width, texture->Height,
								0, GL_RGB, GL_UNSIGNED_BYTE, texture->data);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
							glGenerateMipmap(GL_TEXTURE_2D);

							glBindTexture(GL_TEXTURE_2D, 0);
							m_textures[textureName] = textureId;
						}
						else {
							textureId = iter->second;
						}
						contextPerBatch->diffuseMap = static_cast<int32_t>(textureId);
					}
				
				}

				contextPerBatch->batchIndex = batchCounter++;
				contextPerBatch->vao = vertexArrayId;
				contextPerBatch->mode = mode;
				contextPerBatch->type = type;
				contextPerBatch->indexCount = indexCount;
				contextPerBatch->node = pGeometryNode;
				m_Frame.batchContexts.push_back(contextPerBatch);
			}
			glBindVertexArray(0);
		}
		return true;
	}



	int OpenGLGraphicsManager::Init()
	{
		
		int result;
		result = gladLoadGL();//��OpenGL RHI�³�ʼ��glad ��ע���ƽ̨����glad/(_wgl).c��ͬ������cmake������
		if (!result) {
			cerr << "OpenGL load failed!" << endl;
			result = -1;
		}
		else {
			result = 0;
			cout << "OpenGL Version " << GLVersion.major << "." << GLVersion.minor << " loaded" << endl;

			if (GLAD_GL_VERSION_3_0) {
				// Set the depth buffer to be entirely cleared to 1.0 values.
				glClearDepth(1.0f);

				// Enable depth testing.
				glEnable(GL_DEPTH_TEST);

				// Set the polygon winding to front facing for the right handed system.
				//Ĭ��ֵ��GL_CCW����������ʱ�룬GL_CW����˳ʱ��˳��
				glFrontFace(GL_CCW);

				// Enable back face culling.
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);

				
			}

			InitializeShader(VS_SHADER_SOURCE_FILE, PS_SHADER_SOURCE_FILE);
			InitializeBuffers();
		}
		GraphicsManager::Init();
		glGenBuffers(1, &m_uboBatchId);
		glGenBuffers(1, &m_uboFrameId);
		glGenBuffers(1, &m_lightId);
		return result;
	}

	void OpenGLGraphicsManager::Shutdown()
	{
		//#todo

		// Disable the two vertex array attributes.
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Release the vertex buffer.
		/*glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &m_vertexBufferId);*/

		m_textures.clear();

		delete m_pShader;
	}

	void OpenGLGraphicsManager::Tick()
	{
		Clear();
		GraphicsManager::Tick();
	}

	void OpenGLGraphicsManager::Clear()
	{
		// Set the color to clear the screen to.
		glClearColor(0.5f, 1.0f, 1.0f, 1.0f);
		// Clear the screen and depth buffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLGraphicsManager::Draw()
	{
	

		// Render the model using the color shader.
		RenderBuffers();
		m_pShader->unbind();
		glFlush();
	}

	void OpenGLGraphicsManager::resize(int32_t width, int32_t height)
	{
		glViewport(0, 0, width, height);
	}

	void OpenGLGraphicsManager::bindShader()
	{
		m_pShader->bind();
	}

	void OpenGLGraphicsManager::RenderBuffers()
	{
		//ʹ��ubo ��ȡһ֡�ڼ�ĳ���
		uint32_t blockIndex = glGetUniformBlockIndex(m_pShader->m_ShaderId, "ConstantsPerFrame");
		if (blockIndex != GL_INVALID_INDEX)
		{
			glUniformBlockBinding(m_pShader->m_ShaderId, blockIndex, ConstantsPerFrameBind);
			glBindBufferBase(GL_UNIFORM_BUFFER, ConstantsPerFrameBind, m_uboFrameId);
		}
		
		//light 
		uint32_t lightBlockIdx = glGetUniformBlockIndex(m_pShader->m_ShaderId, "Light");
		if (lightBlockIdx != GL_INVALID_INDEX)
		{
			glUniformBlockBinding(m_pShader->m_ShaderId, lightBlockIdx, FrameLightBind);
			glBindBufferBase(GL_UNIFORM_BUFFER, FrameLightBind, m_lightId);
		}
		
		
		uint32_t bIndex = glGetUniformBlockIndex(m_pShader->m_ShaderId, "ConstantsPerBatch");
		for (auto& pDbc : m_Frame.batchContexts)
		{
			const OpenGLContextPerDrawBatch& dbc = dynamic_cast<const OpenGLContextPerDrawBatch&>(*pDbc);
			//��ÿ������Ⱦʱ�ĳ���
			glBindBufferRange(GL_UNIFORM_BUFFER, bIndex, m_uboBatchId,
				dbc.batchIndex * kSizeOfBatchConstantBuffer, kSizeOfBatchConstantBuffer);
			//������
			m_pShader->setUniform1i("diffuseColor", 0);


			glActiveTexture(GL_TEXTURE0);
			if (dbc.diffuseMap > 0) {
				glBindTexture(GL_TEXTURE_2D, dbc.diffuseMap);
			}
			else {
				glBindTexture(GL_TEXTURE_2D, 0);
			}



			glBindVertexArray(dbc.vao);
			glDrawElements(dbc.mode, dbc.indexCount, dbc.type, 0x00);
		}
		glBindVertexArray(0);
	}

	

	bool OpenGLGraphicsManager::InitializeShader(const char* vsFilename, const char* fsFilename)
	{
		m_pShader = new OpenGLShader(VS_SHADER_SOURCE_FILE, PS_SHADER_SOURCE_FILE);
		return true;
	}

	void OpenGLGraphicsManager::SetPerFrameLight(const Light & light)
	{
		m_Frame.light.lightPos = vec4(1000.0f, 0.0f, 0.0f,0.0f);
		m_Frame.light.lightDir = vec4(1.0f, 0.0f, 0.0f,0.0f);
		m_Frame.light.lightColor = vec4(255.0f, 255.0f, 0.0f,255.0f);
		glBindBuffer(GL_UNIFORM_BUFFER, m_lightId);
		glBufferData(GL_UNIFORM_BUFFER, kSizeOfLigtBuffer, &light, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void OpenGLGraphicsManager::SetPerFrameConstants(const ContextPerFrame& context)
	{
		shared_ptr<CameraNode> camera = g_pSceneManager->GetSceneForRendering().camera;
		m_Frame.frameContext.viewMatrix = camera->m_viewMatrix;
		m_Frame.frameContext.projectionMatrix = camera->m_projectionMatrix;

        
		ConstantsPerFrame constants = static_cast<ConstantsPerFrame>(context);
		glBindBuffer(GL_UNIFORM_BUFFER, m_uboFrameId);
		glBufferData(GL_UNIFORM_BUFFER, kSizeOfFrameConstantBuffer, &constants, GL_DYNAMIC_DRAW);// 256 ����  �� gpu���ȡ todo
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void OpenGLGraphicsManager::SetPerBatchConstants(const std::vector<std::shared_ptr<ContextPerDrawBatch>>& batches)
	{
		uint8_t * pBuffer = new uint8_t[kSizeOfBatchConstantBuffer* batches.size()];
		for (auto & pBatch : batches)
		{
			const ConstantsPerBatch& constants = static_cast<ConstantsPerBatch&>(*pBatch);
			memcpy(pBuffer + pBatch->batchIndex * kSizeOfBatchConstantBuffer, &constants, kSizeOfBatchConstantBuffer);
		}
		
		glBindBuffer(GL_UNIFORM_BUFFER, m_uboBatchId);
		glBufferData(GL_UNIFORM_BUFFER, kSizeOfBatchConstantBuffer*batches.size(), pBuffer, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		delete[] pBuffer;
		pBuffer = nullptr;
	}

}
