﻿#include "opengexparser.h"
#include "maths/PhMaths.h"
#include "scene/SceneHeaders.h"
#include "maths/algorithm/Curve.h"
#include "maths/algorithm/Linear.h"
#include "maths/algorithm/Bezier.h"

using namespace Phantom;
using namespace maths;
void Phantom::OpengexParser::ConvertOddlStructureToSceneNode(const ODDL::Structure & structure, std::shared_ptr<SceneBaseNode>& base_node, Scene& scene)
{
	std::shared_ptr <SceneBaseNode> node;
	switch (structure.GetStructureType()) {
	case OGEX::kStructureNode:
	{
		node = std::make_shared<SceneBaseNode>(structure.GetStructureName());
	}
	break;
	case OGEX::kStructureLightNode:
	{
		auto _node = std::make_shared<SceneLightNode>(structure.GetStructureName());

		//**red**  前面少写了& ， 析构失败。 。。。。。
		const OGEX::LightNodeStructure&/*必须&*/ _structure = dynamic_cast<const OGEX::LightNodeStructure&>(structure);

		_node->SetCastShadow(_structure.GetShadowFlag());

		// ref scene objects
		std::string _key = _structure.GetObjectStructure()->GetStructureName();
		_node->AddSceneObjectRef(_key);

		scene.LightNodes.emplace(_key, _node);

		node = _node;
	}
	break;
	case OGEX::kStructureCameraNode:
	{
		auto _node = std::make_shared<CameraNode>(structure.GetStructureName());
		const OGEX::CameraNodeStructure& _structure = dynamic_cast<const OGEX::CameraNodeStructure&>(structure);

		// ref scene objects
		std::string _key = _structure.GetObjectStructure()->GetStructureName();
		//_node->AddSceneObjectRef(_key);

		scene.camera = _node;

		node = _node;
	}
	break;
	case OGEX::kStructureMaterial:
	{
		const OGEX::MaterialStructure& _structure = dynamic_cast<const OGEX::MaterialStructure&>(structure);
		const char* materialName = _structure.GetMaterialName();
		std::string _key = _structure.GetStructureName();
		auto material = std::make_shared<SceneObjectMaterial>();
		material->SetName(materialName);


		const ODDL::Structure* _sub_structure = _structure.GetFirstCoreSubnode();
		while (_sub_structure) {
			std::string attrib, textureName;
			vec4  color;
			float param;
			switch (_sub_structure->GetStructureType())
			{
			case OGEX::kStructureColor:
			{
				attrib = dynamic_cast<const OGEX::ColorStructure*>(_sub_structure)->GetAttribString();
				color = dynamic_cast<const OGEX::ColorStructure*>(_sub_structure)->GetColor();
				material->SetColor(attrib, color);
			}
			break;
			case OGEX::kStructureParam:
			{
				attrib = dynamic_cast<const OGEX::ParamStructure*>(_sub_structure)->GetAttribString();
				param = dynamic_cast<const OGEX::ParamStructure*>(_sub_structure)->GetParam();
				material->SetParam(attrib, param);
			}
			break;
			case OGEX::kStructureTexture:
			{
				attrib = dynamic_cast<const OGEX::TextureStructure*>(_sub_structure)->GetAttribString();
				textureName = dynamic_cast<const OGEX::TextureStructure*>(_sub_structure)->GetTextureName();
				material->SetTexture(attrib, textureName);
			}
			break;
			default:
				;
			};

			_sub_structure = _sub_structure->Next();
		}
		scene.Materials[_key] = material;
	}
	return;//leaf
	case OGEX::kStructureTransform:
	{
		int32_t index, count;
		const OGEX::TransformStructure& _structure = dynamic_cast<const OGEX::TransformStructure&>(structure);
		bool object_flag = _structure.GetObjectFlag();
		mat4x4 matrix;
		std::shared_ptr<SceneObjectTransform> transform;

		auto _key = _structure.GetStructureName();
		count = _structure.GetTransformCount();
		for (index = 0; index < count; index++) {
			const float* data = _structure.GetTransform(index);
			matrix = data;

			transform = std::make_shared<SceneObjectTransform>(matrix, object_flag);
			base_node->AppendTransform(_key, std::move(transform));
		}
	}
	return;

	case OGEX::kStructureGeometryNode:
	{
		std::string _key = structure.GetStructureName();
		auto _node = std::make_shared<SceneGeometryNode>(_key);
		const OGEX::GeometryNodeStructure& _structure = dynamic_cast<const OGEX::GeometryNodeStructure&>(structure);
		_node->SetVisibility(_structure.GetVisibleFlag());
		_node->SetIfCastShadow(_structure.GetShadowFlag());
		_node->SetIfMotionBlur(_structure.GetMotionBlurFlag());

		// ref scene objects
		_key = _structure.GetObjectStructure()->GetStructureName();
		_node->AddSceneObjectRef(_key);

		// ref materials
		/* parser sample
		MaterialRef (index = 0) {ref {$material20}}
		MaterialRef (index = 1) {ref {$material5}}
		MaterialRef (index = 2) {ref {$material7}}
		MaterialRef (index = 3) {ref {$material16}}
		*/
		auto materials = _structure.GetMaterialStructureArray();
		auto materials_count = materials.GetElementCount();
		for (auto i = 0; i < materials_count; i++)
		{
			auto material = materials[i];
			_key = material->GetStructureName();
			_node->AddMaterialRef(_key);
		}
		std::string name = _structure.GetNodeName();
		scene.GeometryNodes.emplace(name, _node);
		node = _node;
	}
	break;
	case OGEX::kStructureLightObject:
	{
		const OGEX::LightObjectStructure& _structure = dynamic_cast<const OGEX::LightObjectStructure&>(structure);
		std::string _key = _structure.GetStructureName();
		std::shared_ptr<SceneObjectLight> light;

		light = std::make_shared<SceneObjectDirectLight>();

		scene.Lights[_key] = light;
	}
	return;
	case OGEX::kStructureGeometryObject:
	{
		const OGEX::GeometryObjectStructure& _structure =
			dynamic_cast<const OGEX::GeometryObjectStructure&>(structure);
		std::string _key = _structure.GetStructureName();
		auto _object = std::make_shared<SceneObjectGeometry>();
		scene.GeometryOjbects.emplace(_key, _object);

		_object->SetVisibility(_structure.GetVisibleFlag());
		_object->SetIfCastShadow(_structure.GetShadowFlag());
		_object->SetIfMotionBlur(_structure.GetMotionBlurFlag());

		//---------------------------mesh begin -------------------------
		const ODDL::Map<OGEX::MeshStructure> *_meshs = _structure.GetMeshMap();
		int32_t _count = _meshs->GetElementCount();
		for (int32_t i = 0; i < _count; i++)
		{
			const OGEX::MeshStructure* _mesh = (*_meshs)[i];
			std::shared_ptr<SceneObjectMesh> mesh(new SceneObjectMesh());
			const std::string _primitive_type = static_cast<const char*>(_mesh->GetMeshPrimitive());
			if (_primitive_type == "points") {
				mesh->SetPrimitiveType(PrimitiveType::kPrimitiveTypePointList);
			}
			else if (_primitive_type == "lines") {
				mesh->SetPrimitiveType(PrimitiveType::kPrimitiveTypeLineList);
			}
			else if (_primitive_type == "line_strip") {
				mesh->SetPrimitiveType(PrimitiveType::kPrimitiveTypeLineStrip);
			}
			else if (_primitive_type == "triangles") {
				mesh->SetPrimitiveType(PrimitiveType::kPrimitiveTypeTriList);
			}
			else if (_primitive_type == "triangle_strip") {
				mesh->SetPrimitiveType(PrimitiveType::kPrimitiveTypeTriStrip);
			}
			else if (_primitive_type == "quads") {
				mesh->SetPrimitiveType(PrimitiveType::kPrimitiveTypeQuadList);
			}
			else {
				// not supported
				mesh.reset();
			}
			if (mesh)
			{
				const ODDL::Structure* sub_structure = _mesh->GetFirstSubnode();
				while (sub_structure)
				{
					switch (sub_structure->GetStructureType())
					{

					case OGEX::kStructureVertexArray:
					{
						const OGEX::VertexArrayStructure* _v = dynamic_cast<const OGEX::VertexArrayStructure*>(sub_structure);
						const char* attr = _v->GetArrayAttrib();
						auto morph_index = _v->GetMorphIndex();

						const ODDL::Structure* _data_structure = _v->GetFirstCoreSubnode();
						const ODDL::DataStructure<FloatDataType>* dataStructure = dynamic_cast<const ODDL::DataStructure<FloatDataType>*>(_data_structure);

						auto arraySize = dataStructure->GetArraySize();
						auto elementCount = dataStructure->GetDataElementCount();
						const void* _data = &dataStructure->GetDataElement(0);
						void* data = new float[elementCount];
						size_t buf_size = sizeof(float) * elementCount;
						memcpy(data, _data, buf_size);
						VertexDataType vertexDataType;
						switch (arraySize) {
						case 1:
							vertexDataType = VertexDataType::kVertexDataTypeFloat1;
							break;
						case 2:
							vertexDataType = VertexDataType::kVertexDataTypeFloat2;
							break;
						case 3:
							vertexDataType = VertexDataType::kVertexDataTypeFloat3;
							break;
						case 4:
							vertexDataType = VertexDataType::kVertexDataTypeFloat4;
							break;
						default:
							continue;
						}
						SceneObjectVertexArray& _v_array = *new SceneObjectVertexArray(
							attr,
							morph_index,
							vertexDataType,
							data,
							elementCount);
						mesh->AddVertexArray(std::move(_v_array));
					}
					break;
					case OGEX::kStructureIndexArray:
					{
						const OGEX::IndexArrayStructure* _i = dynamic_cast<const OGEX::IndexArrayStructure*>(sub_structure);
						auto material_index = _i->GetMaterialIndex();
						auto restart_index = _i->GetRestartIndex();
						const ODDL::Structure* _data_structure = _i->GetFirstCoreSubnode();
						ODDL::StructureType type = _data_structure->GetStructureType();
						int32_t elementCount = 0;
						const void* _data = nullptr;
						IndexDataType index_type = IndexDataType::kIndexDataTypeInt16;
						switch (type)
						{
						case ODDL::kDataUnsignedInt8:
						{
							index_type = IndexDataType::kIndexDataTypeInt8;
							const ODDL::DataStructure<UnsignedInt8DataType>* dataStructure = dynamic_cast<const ODDL::DataStructure<UnsignedInt8DataType>*>(_data_structure);
							elementCount = dataStructure->GetDataElementCount();
							_data = &dataStructure->GetDataElement(0);

						}
						break;
						case ODDL::kDataUnsignedInt16:
						{
							index_type = IndexDataType::kIndexDataTypeInt16;
							const ODDL::DataStructure<UnsignedInt16DataType>* dataStructure = dynamic_cast<const ODDL::DataStructure<UnsignedInt16DataType>*>(_data_structure);
							elementCount = dataStructure->GetDataElementCount();
							_data = &dataStructure->GetDataElement(0);

						}
						break;
						case ODDL::kDataUnsignedInt32:
						{
							index_type = IndexDataType::kIndexDataTypeInt32;
							const ODDL::DataStructure<UnsignedInt32DataType>* dataStructure = dynamic_cast<const ODDL::DataStructure<UnsignedInt32DataType>*>(_data_structure);
							elementCount = dataStructure->GetDataElementCount();
							_data = &dataStructure->GetDataElement(0);

						}
						break;
						case ODDL::kDataUnsignedInt64:
						{
							index_type = IndexDataType::kIndexDataTypeInt64;
							const ODDL::DataStructure<UnsignedInt64DataType>* dataStructure = dynamic_cast<const ODDL::DataStructure<UnsignedInt64DataType>*>(_data_structure);
							elementCount = dataStructure->GetDataElementCount();
							_data = &dataStructure->GetDataElement(0);

						}
						break;
						default:
							;
						}

						int32_t data_size = 0;
						switch (index_type)
						{
						case IndexDataType::kIndexDataTypeInt8:
							data_size = 1;
							break;
						case IndexDataType::kIndexDataTypeInt16:
							data_size = 2;
							break;
						case IndexDataType::kIndexDataTypeInt32:
							data_size = 4;
							break;
						case IndexDataType::kIndexDataTypeInt64:
							data_size = 8;
							break;
						default:
							;
						}

						size_t buf_size = elementCount * data_size;
						void* data = new uint8_t[buf_size];
						memcpy(data, _data, buf_size);
						SceneObjectIndexArray& _i_array = *new SceneObjectIndexArray(
							material_index,
							restart_index,
							index_type,
							data,
							elementCount);
						mesh->AddIndexArray(std::move(_i_array));

					}
					break;
					default:
						// ignore it
						printf("error mesh");
						;
					}

					sub_structure = sub_structure->Next();
				}

				_object->AddMesh(mesh);
			}
		}
		//-------------end---------------------------------------
	}
	return;//叶子节点，跳出本次递归
	case OGEX::kStructureTranslation:
	{
		const OGEX::TranslationStructure& _structure = dynamic_cast<const OGEX::TranslationStructure&>(structure);
		bool object_flag = _structure.GetObjectFlag();
		std::shared_ptr<SceneObjectTranslation> translation;

		auto kind = _structure.GetTranslationKind();
		auto data = _structure.GetTranslation();
		if (kind == "xyz")
		{
			translation = std::make_shared<SceneObjectTranslation>(data[0], data[1], data[2], object_flag);
		}
		else
		{
			translation = std::make_shared<SceneObjectTranslation>(kind[0], data[0], object_flag);
		}
		auto _key = _structure.GetStructureName();
		base_node->AppendTransform(_key, std::move(translation));
	}
	return;
	case OGEX::kStructureAnimation:
	{
		const OGEX::AnimationStructure& _structure = dynamic_cast<const OGEX::AnimationStructure&>(structure);
		auto clip_index = _structure.GetClipIndex();
		std::shared_ptr<SceneObjectAnimationClip> clip = std::make_shared<SceneObjectAnimationClip>(clip_index);

		const ODDL::Structure* _sub_structure = _structure.GetFirstCoreSubnode();
		while (_sub_structure) {
			switch (_sub_structure->GetStructureType())
			{
			case OGEX::kStructureTrack:
			{
				std::shared_ptr<SceneObjectTrack> track;

				const OGEX::TrackStructure& track_structure = dynamic_cast<const OGEX::TrackStructure&>(*_sub_structure);
				const OGEX::TimeStructure& time_structure = dynamic_cast<const OGEX::TimeStructure&>(*(track_structure.GetTimeStructure()));
				const OGEX::ValueStructure& value_structure = dynamic_cast<const OGEX::ValueStructure&>(*(track_structure.GetValueStructure()));
				auto ref = track_structure.GetTargetRef();
				std::string _key(*ref.GetNameArray());
				std::shared_ptr<SceneObjectTransform> trans = base_node->GetTransform(_key);//变换trans

				auto time_key_value = time_structure.GetKeyValueStructure();
				auto time_key_data_count = time_structure.GetKeyDataElementCount();
				//多次复用此变量
				auto dataStructure = static_cast<const ODDL::DataStructure<ODDL::FloatDataType>*>(
					time_key_value->GetFirstCoreSubnode());

				auto time_array_size = dataStructure->GetArraySize();
				const float * time_knots = &dataStructure->GetDataElement(0);
				// current we only handle 1D time curve
				assert(time_array_size == 0);

				auto value_key_value = value_structure.GetKeyValueStructure();
				auto value_key_data_count = value_structure.GetKeyDataElementCount();
				dataStructure = static_cast<const ODDL::DataStructure<ODDL::FloatDataType>*>(
					value_key_value->GetFirstCoreSubnode());
				auto value_array_size = dataStructure->GetArraySize();
				const float * value_knots = &dataStructure->GetDataElement(0);

				std::shared_ptr<CurveBase> time_curve;
				std::shared_ptr<CurveBase> value_curve;
				SceneObjectTrackType type = SceneObjectTrackType::kScalar;//initialized default

				// time struct process  //Time (curve = "bezier")
				if (time_structure.GetCurveType() == "bezier")
				{
					auto key_incoming_control = time_structure.GetKeyControlStructure(0);
					auto key_outgoing_control = time_structure.GetKeyControlStructure(1);

					dataStructure = static_cast<const ODDL::DataStructure<ODDL::FloatDataType>*>(
						key_incoming_control->GetFirstCoreSubnode());
					const float* in_cp = &dataStructure->GetDataElement(0);
					dataStructure = static_cast<const ODDL::DataStructure<ODDL::FloatDataType>*>(
						key_outgoing_control->GetFirstCoreSubnode());
					const float* out_cp = &dataStructure->GetDataElement(0);

					time_curve = std::make_shared<Bezier<float, float>>(time_knots, in_cp, out_cp, time_key_data_count);
				}
				else
				{
					time_curve = std::make_shared<Linear<float, float>>(time_knots, time_key_data_count);
				}

				// value struct process  //Value (curve = "bezier")
				if (value_structure.GetCurveType() == "bezier")
				{
					auto key_incoming_control = value_structure.GetKeyControlStructure(0);
					auto key_outgoing_control = value_structure.GetKeyControlStructure(1);
					dataStructure =
						static_cast<const ODDL::DataStructure<ODDL::FloatDataType> *>(key_incoming_control->GetFirstCoreSubnode());
					const float* in_cp = &dataStructure->GetDataElement(0);
					dataStructure =
						static_cast<const ODDL::DataStructure<ODDL::FloatDataType> *>(key_outgoing_control->GetFirstCoreSubnode());
					const float* out_cp = &dataStructure->GetDataElement(0);

					switch (value_array_size)
					{
					case 0:
					case 1:
					{
						value_curve = std::make_shared<Bezier<float, float>>(
							value_knots,
							in_cp,
							out_cp,
							value_key_data_count);
						type = SceneObjectTrackType::kScalar;
					}
					break;
					case 3:
					{

						type = SceneObjectTrackType::kVector3;
					}
					break;
					case 4:
					{

						type = SceneObjectTrackType::kQuoternion;
					}
					break;
					case 16:
					{
						cout << "parser matrix missing." << endl;
						type = SceneObjectTrackType::kMatrix;
					}
					break;
					default:
						assert(0);
					}
				}
				else {    // default to linear
					switch (value_array_size)
					{
					case 0:
					case 1:
					{
						value_curve = std::make_shared<Linear<float, float>>(
							value_knots,
							value_key_data_count);
						type = SceneObjectTrackType::kScalar;
					}
					break;
					case 3:
					{

						type = SceneObjectTrackType::kVector3;
					}
					break;
					case 4:
					{

						type = SceneObjectTrackType::kQuoternion;
					}
					break;
					case 16:
					{
						value_curve = std::make_shared<Linear<maths::mat4x4, float>>(
							reinterpret_cast<const maths::mat4x4*>(value_knots),
							value_key_data_count);
						type = SceneObjectTrackType::kMatrix;
					}
					break;
					default:
						assert(0);
					}
				}
				track = std::make_shared<SceneObjectTrack>(trans, time_curve, value_curve, type);
				clip->AddTrack(track);
			}
			break;
			default:
				;
			}
			_sub_structure = _sub_structure->Next();
		}
		base_node->AttachAnimationClip(clip_index, clip);
		scene.AnimationNodes.push_back(base_node);
	}
	return;
	case OGEX::kStructureBoneNode:
	{
		node = std::make_shared<SceneBaseNode>(structure.GetStructureName());
		return;//先不读取Bone下的，需要重载boneNode的attachAnimationClip
	}
	break;
	default:
		node = std::make_shared<SceneBaseNode>(structure.GetStructureName());
		//std::cout << structure.GetStructureName() << "=== name " << std::endl;
		break;
	}
	const ODDL::Structure* sub_structure = structure.GetFirstSubnode();
	while (sub_structure)
	{
		ConvertOddlStructureToSceneNode(*sub_structure, node, scene);

		sub_structure = sub_structure->Next();
	}

	base_node->AppendChild(std::move(node));
}

std::unique_ptr<Scene> Phantom::OpengexParser::Parse(const std::string & buf)
{
	std::unique_ptr<Scene> spScene(new Scene("scene"));
	OGEX::OpenGexDataDescription  openGexDataDescription;

	ODDL::DataResult result = openGexDataDescription.ProcessText(buf.c_str());
	if (result == ODDL::kDataOkay)
	{
		const ODDL::Structure* structure = openGexDataDescription.GetRootStructure()->GetFirstSubnode();
		while (structure)
		{
			ConvertOddlStructureToSceneNode(*structure, spScene->SceneRootGraph, *spScene);

			structure = structure->Next();
		}
	}
	return spScene;
}