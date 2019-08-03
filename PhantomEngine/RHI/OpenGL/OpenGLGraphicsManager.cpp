﻿#include "glad/glad.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include "OpenGLGraphicsManager.h"
#include "common/AssetLoadManager.h"
#include "common/SceneManager.h"
#include "common/GfxConfiguration.h"
#include "interface/IApplication.h"
#include "scene/Scene.h"
#include <unordered_map>
#include "scene/SceneObjectGeometry.h"
#include "scene/SceneObjectVertexArray.h"
#include "scene/SceneObjectIndexArray.h"
#include "graphics/GfxStruct.h"




using namespace std;
using namespace Phantom::maths;

extern struct gladGLversionStruct GLVersion;

namespace Phantom {

	extern AssetLoadManager* g_pAssetLoader;
	extern SceneManager* g_pSceneManager;
	extern IApplication* g_pApp;


	const char VS_SHADER_SOURCE_FILE[] = "Resources/shaders/vert_light.shader";
	const char PS_SHADER_SOURCE_FILE[] = "Resources/shaders/frag_light.shader";

	const char SKYBOX_VS_SHADER_SOURCE_FILE[] = "Resources/shaders/skybox_vert.shader";
	const char SKYBOX_PS_SHADER_SOURCE_FILE[] = "Resources/shaders/skybox_frag.shader";

	const char SHADOWMAP_VS_SHADER_SOURCE_FILE[] = "Resources/shaders/shadowMap_vert.shader";
	const char SHADOWMAP_PS_SHADER_SOURCE_FILE[] = "Resources/shaders/shadowMap_frag.shader";

	const char TEXT_VS_SHADER_SOURCE_FILE[] = "Resources/shaders/text_vt.shader";
	const char TEXT_PS_SHADER_SOURCE_FILE[] = "Resources/shaders/text_ps.shader";

	float cubeVertices[] = {
		// positions          // texture Coords
			-50.0f, -50.0f, -50.0f,  0.0f, 0.0f,
		 50.0f, -50.0f, -50.0f,  1.0f, 0.0f,
		 50.0f,  50.0f, -50.0f,  1.0f, 1.0f,
		 50.0f,  50.0f, -50.0f,  1.0f, 1.0f,
		-50.0f,  50.0f, -50.0f,  0.0f, 1.0f,
		-50.0f, -50.0f, -50.0f,  0.0f, 0.0f,

		-50.0f, -50.0f,  50.0f,  0.0f, 0.0f,
		 50.0f, -50.0f,  50.0f,  1.0f, 0.0f,
		 50.0f,  50.0f,  50.0f,  1.0f, 1.0f,
		 50.0f,  50.0f,  50.0f,  1.0f, 1.0f,
		-50.0f,  50.0f,  50.0f,  0.0f, 1.0f,
		-50.0f, -50.0f,  50.0f,  0.0f, 0.0f,

		-50.0f,  50.0f,  50.0f,  1.0f, 0.0f,
		-50.0f,  50.0f, -50.0f,  1.0f, 1.0f,
		-50.0f, -50.0f, -50.0f,  0.0f, 1.0f,
		-50.0f, -50.0f, -50.0f,  0.0f, 1.0f,
		-50.0f, -50.0f,  50.0f,  0.0f, 0.0f,
		-50.0f,  50.0f,  50.0f,  1.0f, 0.0f,

		 50.0f,  50.0f,  50.0f,  1.0f, 0.0f,
		 50.0f,  50.0f, -50.0f,  1.0f, 1.0f,
		 50.0f, -50.0f, -50.0f,  0.0f, 1.0f,
		 50.0f, -50.0f, -50.0f,  0.0f, 1.0f,
		 50.0f, -50.0f,  50.0f,  0.0f, 0.0f,
		 50.0f,  50.0f,  50.0f,  1.0f, 0.0f,

		-50.0f, -50.0f, -50.0f,  0.0f, 1.0f,
		 50.0f, -50.0f, -50.0f,  1.0f, 1.0f,
		 50.0f, -50.0f,  50.0f,  1.0f, 0.0f,
		 50.0f, -50.0f,  50.0f,  1.0f, 0.0f,
		-50.0f, -50.0f,  50.0f,  0.0f, 0.0f,
		-50.0f, -50.0f, -50.0f,  0.0f, 1.0f,

		-50.0f,  50.0f, -50.0f,  0.0f, 1.0f,
		 50.0f,  50.0f, -50.0f,  1.0f, 1.0f,
		 50.0f,  50.0f,  50.0f,  1.0f, 0.0f,
		 50.0f,  50.0f,  50.0f,  1.0f, 0.0f,
		-50.0f,  50.0f,  50.0f,  0.0f, 0.0f,
		-50.0f,  50.0f, -50.0f,  0.0f, 1.0f
	};

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

			//--- mesh ---- ²ð½â---------
			const auto vertexPropertiesCount = pMesh->GetVertexPropertiesCount();

			GLuint vertexArrayId;
			// Allocate an OpenGL vertex array object.
			glGenVertexArrays(1, &vertexArrayId);
			// Bind the vertex array object to store all the buffers and vertex attributes we create here.
			glBindVertexArray(vertexArrayId);

			GLuint vertexBufferId;
			GLuint positionBufferId; //todo 临时记录pos ，做cpu蒙皮
			// Generate an ID for the vertex buffer.
			for (GLuint i = 0; i < vertexPropertiesCount; i++)
			{
				const SceneObjectVertexArray & vProArr = pMesh->GetVertexPropertyArray(i);
				const auto vProArrSize = vProArr.GetDataSize();
				const auto vProArrData = vProArr.GetData();

				glGenBuffers(1, &vertexBufferId);
				if (i == 0) { positionBufferId = vertexBufferId; }

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
				//#todo buff id ÐèÒªÍ³¼Æ¹ÜÀí£¬ÒÔ±¸ÊÍ·ÅÇÐ»»³¡¾°ÊÍ·Å
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
				/*const*/ auto material_index = index_array.GetMaterialIndex();
				material_index = 0;//²»Ì«¶®3dmaxµÄ²ÄÖÊ¹ÜÀí£¬ÏÈÄ¬ÈÏ²ÉÓÃGeomatry¹ÒµÄµÚÒ»¸ö²ÄÖÊ¡£

				const auto& matName = pGeometryNode->GetMaterialRef(material_index);
				const auto material = scene.GetMaterial(matName);
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
				contextPerBatch->posBuffId = positionBufferId;
				m_Frame.batchContexts.push_back(contextPerBatch);
			}
			glBindVertexArray(0);
		}
		return true;
	}

	int OpenGLGraphicsManager::Init()
	{
		GraphicsManager::Init();
		int result;
		result = gladLoadGL();//ÔÚOpenGL RHIÏÂ³õÊ¼»¯glad £¬×¢Òâ¸÷Æ½Ì¨ÒýÓÃglad/(_wgl).c²»Í¬£¬ÔÝÔÚcmakeÖÐÉèÖÃ
		if (!result) {
			cerr << "OpenGL load failed!" << endl;
			result = -1;
		}
		result = 0;
		cout << "OpenGL Version " << GLVersion.major << "." << GLVersion.minor << " loaded" << endl;

		if (GLAD_GL_VERSION_3_0) {
			// Set the depth buffer to be entirely cleared to 1.0 values.
			glClearDepth(1.0f);

			// Enable depth testing.
			glEnable(GL_DEPTH_TEST);

			// Set the polygon winding to front facing for the right handed system.
			//Ä¬ÈÏÖµÊÇGL_CCW£¬Ëü´ú±íÄæÊ±Õë£¬GL_CW´ú±íË³Ê±ÕëË³Ðò¡£
			glFrontFace(GL_CCW);

			/*glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);*/

		}
		InitializeShader();
		initializeTextVao();

		glGenBuffers(1, &m_uboBatchId);
		glGenBuffers(1, &m_uboFrameId);
		glGenBuffers(1, &m_lightId);
		glGenFramebuffers(1, &m_shadowMapFboId);
		return result;
	}

	void OpenGLGraphicsManager::Shutdown()
	{
		PurgeCurScene();
		m_textures.clear();
		glDeleteBuffers(1, &m_uboBatchId);
		glDeleteBuffers(1, &m_uboFrameId);
		glDeleteBuffers(1, &m_lightId);
		glDeleteFramebuffers(1, &m_shadowMapFboId);
		GraphicsManager::Shutdown();
	}

	void OpenGLGraphicsManager::Tick()
	{
		Clear();
		GraphicsManager::Tick();
	}

	void OpenGLGraphicsManager::Clear()
	{
		for (GLenum err; (err = glGetError()) != GL_NO_ERROR;)
		{
			printf("gl error = %d", err);
		}
		// Set the color to clear the screen to.
		glClearColor(0.5f, 1.0f, 1.0f, 1.0f);
		// Clear the screen and depth buffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLGraphicsManager::Draw()
	{
		//shadow map pass
		RenderShadowMap();

		// Render the model using the color shader.
		BindShaderByType(Common_Shader);
		
		SetShadowMap();
		
		RenderBatches();

		RenderDebugInfo();

		DrawSkyBox();
		m_pShader->unbind();
		//glFlush();
	}

	void OpenGLGraphicsManager::DrawGUI()
	{
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		m_TextShader->bind();
		//正交投影 -#todo 挪到init or camera中，无需反复创建
		maths::mat4x4  m_pro;
		GfxConfiguration& conf = g_pApp->GetConfiguration();
		m_pro.orthographic(0.0f, conf.screenWidth, 0.0f, conf.screenHeight, 0.01f, 10000.0f);
		m_TextShader->setUniformMat4("projection", m_pro);



		glBindVertexArray(m_TextVaoId);
		m_TextShader->setUniform1i("text", 3);
		glActiveTexture(GL_TEXTURE3);

		glBindTexture(GL_TEXTURE_2D, m_TextTextureId);
		std::unordered_map<char, GUI::GuiDisplayUnit>::iterator itr;
		for (itr = m_GuiUnitMap.begin(); itr != m_GuiUnitMap.end(); itr++)
		{
			auto unit = itr->second;
			std::string::const_iterator c;
			maths::vec3 color = itr->first == 0 ? maths::vec3(0.5, 0.8f, 0.2f) : maths::vec3(0.3, 0.8f, 0.6f);
			m_TextShader->setUniform3f("textColor", color);
			for (c = unit.content.begin(); c != unit.content.end(); c++)
			{
				TextCore::Character ch = fontEngine.m_Characters[*c];
				float scale = 1.0f;
				GLfloat xpos = unit.posX + ch.Bearing.x * scale;
				GLfloat ypos = unit.posY - (ch.Size.y - ch.Bearing.y) * scale;
				GLfloat w = ch.Size.x * scale;
				GLfloat h = ch.Size.y * scale;

				GLfloat xOffsetUv = ch.offset.x / TextCore::K_TEXTURE_SIZE;
				GLfloat yOffsetUv = ch.offset.y / TextCore::K_TEXTURE_SIZE;
				GLfloat xPlusWidthUv = (ch.offset.x + w) / TextCore::K_TEXTURE_SIZE;
				GLfloat yPlusHeightUv = (ch.offset.y + h) / TextCore::K_TEXTURE_SIZE;

				GLfloat vertices[6][4] = {
				  { xpos,     ypos + h,   xOffsetUv, yOffsetUv },
					{ xpos,     ypos,       xOffsetUv, yPlusHeightUv },
					{ xpos + w, ypos,       xPlusWidthUv, yPlusHeightUv },

					{ xpos,     ypos + h,   xOffsetUv, yOffsetUv },
					{ xpos + w, ypos,       xPlusWidthUv, yPlusHeightUv},
					{ xpos + w, ypos + h,   xPlusWidthUv, yOffsetUv }
				};


				glBindBuffer(GL_ARRAY_BUFFER, m_TextVboId);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				unit.posX += (ch.Advance >> 6)*scale;
			}
		}

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	bool OpenGLGraphicsManager::initializeSkyBox()
	{

		//==============================================
		static float skyboxVertices[] = {
			// positions          
		  -1.0f,  1.0f, -1.0f,
		  -1.0f, -1.0f, -1.0f,
		   1.0f, -1.0f, -1.0f,
		   1.0f, -1.0f, -1.0f,
		   1.0f,  1.0f, -1.0f,
		  -1.0f,  1.0f, -1.0f,

		  -1.0f, -1.0f,  1.0f,
		  -1.0f, -1.0f, -1.0f,
		  -1.0f,  1.0f, -1.0f,
		  -1.0f,  1.0f, -1.0f,
		  -1.0f,  1.0f,  1.0f,
		  -1.0f, -1.0f,  1.0f,

		   1.0f, -1.0f, -1.0f,
		   1.0f, -1.0f,  1.0f,
		   1.0f,  1.0f,  1.0f,
		   1.0f,  1.0f,  1.0f,
		   1.0f,  1.0f, -1.0f,
		   1.0f, -1.0f, -1.0f,

		  -1.0f, -1.0f,  1.0f,
		  -1.0f,  1.0f,  1.0f,
		   1.0f,  1.0f,  1.0f,
		   1.0f,  1.0f,  1.0f,
		   1.0f, -1.0f,  1.0f,
		  -1.0f, -1.0f,  1.0f,

		  -1.0f,  1.0f, -1.0f,
		   1.0f,  1.0f, -1.0f,
		   1.0f,  1.0f,  1.0f,
		   1.0f,  1.0f,  1.0f,
		  -1.0f,  1.0f,  1.0f,
		  -1.0f,  1.0f, -1.0f,

		  -1.0f, -1.0f, -1.0f,
		  -1.0f, -1.0f,  1.0f,
		   1.0f, -1.0f, -1.0f,
		   1.0f, -1.0f, -1.0f,
		  -1.0f, -1.0f,  1.0f,
		   1.0f, -1.0f,  1.0f
		};

		//为得到不扭曲的天空盒，需考虑大点。
		for (int i = 0; i < sizeof(skyboxVertices) / 4; i++)
		{
			skyboxVertices[i] = 256.0f * skyboxVertices[i];
		}


		unsigned int skyboxVAO, skyboxVBO;
		glGenVertexArrays(1, &skyboxVAO);
		glGenBuffers(1, &skyboxVBO);
		glBindVertexArray(skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		GLenum target;
		target = GL_TEXTURE_CUBE_MAP;
		uint32_t texture_id;
		glGenTextures(1, &texture_id);
		glBindTexture(target, texture_id);
		vector<std::string> names = { "Textures/right.jpg","Textures/left.jpg","Textures/top.jpg",
								"Textures/bottom.jpg" ,"Textures/front.jpg","Textures/back.jpg" };
		for (int picIdx = 0; picIdx < 6; picIdx++)
		{
			Buffer buf = g_pAssetLoader->SyncOpenAndReadBinary(names[picIdx].c_str());
			JpegParser parser;
			auto imgptr = std::make_shared<Image>(parser.Parse(buf));
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + picIdx, 0, GL_RGB, imgptr->Width, imgptr->Height,
				0, GL_RGB, GL_UNSIGNED_BYTE, imgptr->data);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		for (GLenum err; (err = glGetError()) != GL_NO_ERROR;)
		{
			printf("gl error = %d", err);
		}

		m_textures["SkyBox"] = texture_id;
		m_Frame.frameContext.skybox = texture_id;
		m_skyboxShader->bind();
		m_skyboxShader->setUniform1i("skybox", 2); //2号纹理  /如果是0的话这里可以忽略


		m_skyboxContext.vao = skyboxVAO;
		m_skyboxContext.mode = GL_TRIANGLES;
		m_skyboxContext.type = GL_UNSIGNED_BYTE;
		m_skyboxContext.indexCount = 36;



		return true;
	}

	//need to go further ; pack all glyphs into one texture and record uvs.
	bool OpenGLGraphicsManager::initializeTextVao()
	{
		// Disable byte-alignment restriction
		glGenTextures(1, &m_TextTextureId);
		glBindTexture(GL_TEXTURE_2D, m_TextTextureId);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			TextCore::K_TEXTURE_SIZE,
			TextCore::K_TEXTURE_SIZE,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			0
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		int xStart = 0;
		int yStart = 0;

		for (int i = 0; i < 128; i++) {
			if (xStart + TextCore::K_FONT_SIZE > TextCore::K_TEXTURE_SIZE)
			{
				xStart = 0;
				yStart += TextCore::K_FONT_SIZE;
			}
			fontEngine.RenderGlyphToTextureData(i);
			TextCore::Character &c = fontEngine.m_Characters.at(i);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexSubImage2D(
				GL_TEXTURE_2D,
				0,
				xStart,
				yStart,
				c.Size.x,
				c.Size.y,
				GL_RED,
				GL_UNSIGNED_BYTE,
				c.buffer
			);
			c.offset.x = xStart;
			c.offset.y = yStart;
			xStart += c.Size.x + 1;
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		// Configure VAO/VBO for texture quads
		glGenVertexArrays(1, &m_TextVaoId);
		glGenBuffers(1, &m_TextVboId);
		glBindVertexArray(m_TextVaoId);
		glBindBuffer(GL_ARRAY_BUFFER, m_TextVboId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		return true;
	}

	void OpenGLGraphicsManager::DrawSkyBox()
	{

		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to depth buffer's content
		m_skyboxShader->bind();
		int32_t texture_id = m_Frame.frameContext.skybox;

		//m_skyboxShader->setUniformMat4("projection", m_Frame.frameContext.projectionMatrix);
		//m_skyboxShader->setUniformMat4("view", m_Frame.frameContext.viewMatrix);
		uint32_t blockIndex = glGetUniformBlockIndex(m_skyboxShader->m_ShaderId, "ConstantsPerFrame");
		if (blockIndex != GL_INVALID_INDEX)
		{
			glUniformBlockBinding(m_skyboxShader->m_ShaderId, blockIndex, ConstantsPerFrameBind);
			glBindBufferBase(GL_UNIFORM_BUFFER, ConstantsPerFrameBind, m_uboFrameId);
		}

		glBindVertexArray(m_skyboxContext.vao);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
		//glDrawElements(m_skyboxContext.mode, m_skyboxContext.indexCount, m_skyboxContext.type, 0x00);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
	}

	void OpenGLGraphicsManager::resize(int32_t width, int32_t height)
	{
		glViewport(0, 0, width, height);
		GfxConfiguration& conf = g_pApp->GetConfiguration();
		conf.screenWidth = width;
		conf.screenHeight = height;
	}

	
	void OpenGLGraphicsManager::EnterScene(const Scene & scene)
	{
		GraphicsManager::EnterScene(scene);
		
		InitializeBuffers();
		initializeSkyBox();
	

	}

	void OpenGLGraphicsManager::PurgeCurScene()
	{
		auto& batchs = m_Frame.batchContexts;
		for (auto& dbc : batchs) {
			glDeleteVertexArrays(1, &dynamic_pointer_cast<OpenGLContextPerDrawBatch>(dbc)->vao);
		}

		for (auto& te : m_textures)
		{
			glDeleteTextures(1, &te.second);
		}
		m_textures.clear();

		GraphicsManager::PurgeCurScene();
	}

	void OpenGLGraphicsManager::BindShaderByType(Shader_Type st)
	{
		switch (st)
		{
		case Common_Shader:
			m_currentShader = m_pShader;
			break;
		case SkyBox_Shader:
			m_currentShader = m_skyboxShader;
			break;
		case ShadowMap_Shader:
			m_currentShader = m_pShadowMapShader;
			break;
		default:
			std::cout << "this shader is not supported!" << std::endl;
			break;
		}
		shared_ptr<OpenGLShader> curShader = m_currentShader.lock();
		curShader->bind();
	}


	void OpenGLGraphicsManager::RenderBatches()
	{
		shared_ptr<OpenGLShader> curShader = m_currentShader.lock();
		//Ê¹ÓÃubo »ñÈ¡Ò»Ö¡ÆÚ¼äµÄ³£Á¿
		uint32_t blockIndex = glGetUniformBlockIndex(curShader->m_ShaderId, "ConstantsPerFrame");//Shader.PropertyToID
		if (blockIndex != GL_INVALID_INDEX)
		{
			glUniformBlockBinding(curShader->m_ShaderId, blockIndex, ConstantsPerFrameBind);
			glBindBufferBase(GL_UNIFORM_BUFFER, ConstantsPerFrameBind, m_uboFrameId);
		}

		//light 
		uint32_t lightBlockIdx = glGetUniformBlockIndex(curShader->m_ShaderId, "Light");
		if (lightBlockIdx != GL_INVALID_INDEX)
		{
			glUniformBlockBinding(curShader->m_ShaderId, lightBlockIdx, FrameLightBind);
			glBindBufferBase(GL_UNIFORM_BUFFER, FrameLightBind, m_lightId);
		}

		uint32_t bIndex = glGetUniformBlockIndex(curShader->m_ShaderId, "ConstantsPerBatch");
		for (auto& pDbc : m_Frame.batchContexts)
		{
			const OpenGLContextPerDrawBatch& dbc = dynamic_cast<const OpenGLContextPerDrawBatch&>(*pDbc);
			//°ó¶¨Ã¿Åú´ÎäÖÈ¾Ê±µÄ³£Á¿
			glBindBufferRange(GL_UNIFORM_BUFFER, bIndex, m_uboBatchId,
				dbc.batchIndex * kSizeOfBatchConstantBuffer, kSizeOfBatchConstantBuffer);

			uint32_t diffuseIndex = glGetUniformLocation(curShader->m_ShaderId, "diffuseColor");
			if (dbc.diffuseMap >= 0 && diffuseIndex != GL_INVALID_INDEX) {
				m_pShader->setUniform1i("diffuseColor", 0);  //material.SetTexture.
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, dbc.diffuseMap);
			}

			glBindVertexArray(dbc.vao);
			glDrawElements(dbc.mode, dbc.indexCount, dbc.type, 0x00);
		}
		glBindVertexArray(0);
	}

	

	void OpenGLGraphicsManager::RenderShadowMap()
	{
		BindShaderByType(ShadowMap_Shader);

		BeginShadowMap();

		RenderBatches();

		EndShadowMap();
	}

	static const int kSizeOfShadowMap = 1024;
	void OpenGLGraphicsManager::BeginShadowMap()
	{
		//clear shadow map
		if (m_Frame.frameContext.shadowMap != -1)
		{
			GLuint id = (GLuint)m_Frame.frameContext.shadowMap;
			glDeleteTextures(1, &id);
			m_Frame.frameContext.shadowMap = -1;
		}
		GLuint shadowMap;
		glGenTextures(1, &shadowMap);
		glActiveTexture(GL_TEXTURE0 + shadowMap);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, kSizeOfShadowMap, kSizeOfShadowMap, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

		m_Frame.frameContext.shadowMap = shadowMap;

		glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFboId);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMap, 0);
		// Always check that our framebuffer is ok
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << " error !!!" << std::endl;
		}
		glDrawBuffers(0, nullptr); // No color buffer is drawn to.
		glDepthMask(GL_TRUE);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, kSizeOfShadowMap, kSizeOfShadowMap);


		// We don't use bias in the shader, but instead we draw back faces,
		// which are already separated from the front faces by a small distance
		// (if your geometry is made this way)
		glCullFace(GL_FRONT); // Cull front-facing triangles -> draw only back-facing triangles
	}

	void OpenGLGraphicsManager::EndShadowMap()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//glDeleteFramebuffers(1, &m_shadowMapFboId); //参考  glGenFramebuffers

		const GfxConfiguration& conf = g_pApp->GetConfiguration();
		glViewport(0, 0, conf.screenWidth, conf.screenHeight);

		glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
	}

	void OpenGLGraphicsManager::SetShadowMap()
	{
		GLuint textureId = m_Frame.frameContext.shadowMap;
		/*glActiveTexture(GL_TEXTURE0 +  textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);*/
		m_pShader->setUniform1i("shadowMap", textureId);
	}

	bool OpenGLGraphicsManager::InitializeShader()
	{
		m_pShader = make_shared<OpenGLShader>(VS_SHADER_SOURCE_FILE, PS_SHADER_SOURCE_FILE);
		m_skyboxShader = make_shared<OpenGLShader>(SKYBOX_VS_SHADER_SOURCE_FILE, SKYBOX_PS_SHADER_SOURCE_FILE);
		m_pShadowMapShader = make_shared<OpenGLShader>(SHADOWMAP_VS_SHADER_SOURCE_FILE, SHADOWMAP_PS_SHADER_SOURCE_FILE);
		m_TextShader = make_shared<OpenGLShader>(TEXT_VS_SHADER_SOURCE_FILE, TEXT_PS_SHADER_SOURCE_FILE);
		m_currentShader = m_pShader;
		return true;
	}

	void OpenGLGraphicsManager::SetPerFrameLight()
	{


		glBindBuffer(GL_UNIFORM_BUFFER, m_lightId);
		glBufferData(GL_UNIFORM_BUFFER, kSizeOfLightBuffer, &m_Frame.light, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}



	void OpenGLGraphicsManager::SetPerFrameConstants(const ContextPerFrame& context)
	{
		ConstantsPerFrame constants = static_cast<ConstantsPerFrame>(context);   //MaterialPropertyBlock
		glBindBuffer(GL_UNIFORM_BUFFER, m_uboFrameId);
		glBufferData(GL_UNIFORM_BUFFER, kSizeOfFrameConstantBuffer, &constants, GL_DYNAMIC_DRAW);// 256 ¶ÔÆë  £¬ gpu¿é¶ÁÈ¡ todo
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	static void * pBuffer = new uint8_t[1024 * 1024];//todo 挪到引擎core中
	// GraphicsManager中处理Cpu-skin ， 将处理完的顶点数据传入各GL进行处理？？？？//todo
	void OpenGLGraphicsManager::ProcessCpuSkin(const ConstantsPerBatch& batch)
	{
		const OpenGLContextPerDrawBatch& drawBatch = static_cast<const OpenGLContextPerDrawBatch&>(batch);
		std::shared_ptr<SceneGeometryNode> geoNode = drawBatch.node;

		auto & scene = g_pSceneManager->GetSceneForRendering();
		std::unordered_map<std::string, std::shared_ptr<SceneObjectGeometry>> geoObjects = scene.GeometryOjbects;
		const std::string & oKey = geoNode->GetSceneObjectRef();
		auto pGeometry = geoObjects[oKey];
		const auto& pMesh = pGeometry->GetMesh().lock();
		auto skin = pMesh->GetSkin().lock();
		if (!skin) return;

		glBindVertexArray(drawBatch.vao);
		const auto vertexPropertiesCount = pMesh->GetVertexPropertiesCount();

		auto skeleton = skin->GetSkeleton().lock();
		auto boneCountArr = skin->GetBoneCountArray().lock();
		auto boneIndexArr = skin->GetBoneIndexArray().lock();
		auto boneWeightArr = skin->GetBoneWeightArray().lock();
		auto skinRootTransform = skin->GetSkinMatrix();

		auto sceneBoneNodes = scene.BoneNodes;
		auto vBoneRefArr = skeleton->GetBoneRefArr().lock()->GetBoneNodeRefArr();

		GLuint vertexBufferId;
		// Generate an ID for the vertex buffer.  
		for (GLuint i = 0; i < vertexPropertiesCount; i++)
		{
			const SceneObjectVertexArray & vProArr = pMesh->GetVertexPropertyArray(i);
			const auto vProArrSize = vProArr.GetDataSize();
			const auto vProArrData = vProArr.GetData();

			memcpy(pBuffer, vProArrData, vProArrSize);

#pragma region Skining
			const int verNum = boneCountArr->GetVertexCount();
			int influenceCount = 0;
			const unsigned short* bca = boneCountArr->GetData();
			const unsigned short* bia = boneIndexArr->GetData();
			const float*          bwa = boneWeightArr->GetData();
			for (int i = 0; i < verNum; i++)  //for every vertex  
			{
				influenceCount = *(bca + i);
				float x = *((float*)pBuffer + 0);
				float y = *((float*)pBuffer + 1);
				float z = *((float*)pBuffer + 2);
				//P-bind  where bind P is the bind-pose position of the vertex (having an implicit w coordinate of one)
				vec4 v(x, y, z, 1);
				v = skinRootTransform * v;
				vec4 tv;
				vec4 tt;
				//todo pre-calculate all bone's transform.
			// 可以参考这个写一个boneStack ： http://www.wazim.com/Collada_Tutorial_2.htm
			// 进行整体旋转的话需要对整体进行父节点包装
				/*auto chain = boneNode->GetTreeChain();
				*/

				for (int j = 0; j < influenceCount; j++)
				{
					int idx = *(bia++);
					float weight = *(bwa++);

					auto boneRef = vBoneRefArr[idx];
					const std::string &boneName = boneRef->GetName();
					auto boneNode = sceneBoneNodes.find(boneName)->second.lock();
					auto finalMat = boneNode->m_RuntimeWithBindPoseMat;

					tt =
						*(finalMat)*
						v;
					tt *= weight;
					tv += tt;
					assert(weight <= 1.0);
				}

				mat4x4 rotation; //todo
				rotation = rotation.rotation(270, vec3(1.0f, 0.0f, 0.0f));
				tv = rotation * tv;

				*((float*)pBuffer + 0) = tv.x;
				*((float*)pBuffer + 1) = tv.y;
				*((float*)pBuffer + 2) = tv.z;

				pBuffer = (float*)pBuffer + 3;
			}

			pBuffer = (float*)pBuffer - (verNum * 3);
#pragma endregion



			glBindBuffer(GL_ARRAY_BUFFER, drawBatch.posBuffId);
			glBufferSubData(GL_ARRAY_BUFFER, 0, vProArrSize, pBuffer);
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, 3, GL_FLOAT, false, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			break;//仅仅处理顶点位置蒙皮先 //todo normal *****
		}
		glBindVertexArray(0);
	}

	void OpenGLGraphicsManager::SetPerBatchConstants(const std::vector<std::shared_ptr<ContextPerDrawBatch>>& batches)
	{

		uint8_t * lpBuffer = new uint8_t[kSizeOfBatchConstantBuffer* batches.size()];
		for (auto & pBatch : batches)
		{
			const ConstantsPerBatch& constants = static_cast<ConstantsPerBatch&>(*pBatch);
			ProcessCpuSkin(constants);
			memcpy(lpBuffer + pBatch->batchIndex * kSizeOfBatchConstantBuffer, &constants, kSizeOfBatchConstantBuffer);
		}

		glBindBuffer(GL_UNIFORM_BUFFER, m_uboBatchId);
		glBufferData(GL_UNIFORM_BUFFER, kSizeOfBatchConstantBuffer*batches.size(), lpBuffer, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		delete[] lpBuffer;
		lpBuffer = nullptr;
	}
	void OpenGLGraphicsManager::DrawLine(const maths::vec3 & from, const maths::vec3 & to, const maths::vec3 & color)
	{
		const int LEN = 2;
		GLfloat* _vertices = new GLfloat[3 * LEN];

		{
			_vertices[0] = from.x;
			_vertices[1] = from.y;
			_vertices[2] = from.z;
			_vertices[3] = to.x;
			_vertices[4] = to.y;
			_vertices[5] = to.z;
		}

		uint32_t vao;
		glGenVertexArrays(1, &vao);
		// Bind the vertex array object to store all the buffers and vertex attributes we create here.
		glBindVertexArray(vao);
		uint32_t buffer_id;
		// Generate an ID for the vertex buffer.
		glGenBuffers(1, &buffer_id);
		// Bind the vertex buffer and load the vertex (position and color) data into the vertex buffer.
		glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * LEN, _vertices, GL_STATIC_DRAW);
		delete[] _vertices;
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

		DebugDrawBatchContext& dbc = *(new DebugDrawBatchContext); //leak?
		dbc.vao = vao;
		dbc.mode = GL_LINES;
		dbc.indexCount = static_cast<int32_t>(LEN);
		dbc.color = color;
		dbc.posBuffId = buffer_id;

		m_DebugDrawBatchContext.push_back(std::move(dbc));
	}
	void OpenGLGraphicsManager::RenderDebugInfo()
	{
		if (m_DebugDrawBatchContext.size() < 1) return;
		BindShaderByType(Shader_Type::Common_Shader);
		std::shared_ptr<OpenGLShader> curShader = m_currentShader.lock();

		//只要在Geometry后面渲染就不必改变Uniform state。//todo 做一套DEBUG独有的shader和buffer。
		//ConstantsPerFrame constants = static_cast<ConstantsPerFrame>(m_Frame.frameContext);   //MaterialPropertyBlock
		//glBindBuffer(GL_UNIFORM_BUFFER, m_uboFrameId);
		//glBufferData(GL_UNIFORM_BUFFER, kSizeOfFrameConstantBuffer, &constants, GL_DYNAMIC_DRAW);// 256 ¶ÔÆë  £¬ gpu¿é¶ÁÈ¡ todo

		//create a standard world matrix
		uint8_t * lpBuffer = new uint8_t[kSizeOfBatchConstantBuffer];
		ConstantsPerBatch cfb;
		memcpy(lpBuffer , &cfb, kSizeOfBatchConstantBuffer);

		uint32_t bIndex = glGetUniformBlockIndex(curShader->m_ShaderId, "ConstantsPerBatch");
		glBindBuffer(GL_UNIFORM_BUFFER, m_uboBatchId);
		glBindBufferRange(GL_UNIFORM_BUFFER, bIndex, m_uboBatchId,0, kSizeOfBatchConstantBuffer);
	
		for (auto& pDbc : m_DebugDrawBatchContext)
		{
			const OpenGLContextPerDrawBatch& dbc = dynamic_cast<const OpenGLContextPerDrawBatch&>(pDbc);
			glBindVertexArray(dbc.vao);
			glDrawArrays(dbc.mode,0, dbc.indexCount);
		}
		delete[] lpBuffer;
		lpBuffer = nullptr;
	}
}
