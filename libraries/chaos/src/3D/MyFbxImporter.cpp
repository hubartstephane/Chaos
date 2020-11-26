#include <chaos/Chaos.h>

namespace chaos
{


	static void DisplayCurves(FbxObject * obj)
	{
		if (obj == nullptr)
			return;

		FbxProperty prop = obj->GetFirstProperty();
		while (prop.IsValid())
		{
			if (prop.IsAnimated())
			{
				Log::Message("PROPERTY [%s]", prop.GetName());



			}


			prop = obj->GetNextProperty(prop);
		}
	}


	static void DisplaySceneAnimation(FbxScene* pScene)
	{

		//int i;FbxAnimStack
		//for (i = 0; i < pScene->GetSrcObjectCount<FbxObject>(); i++)
		{

			//FbxAnimStack* lAnimStack = pScene->GetSrcObject<FbxAnimStack>(i);


			//Log::Message("anim  stack [%s]", lAnimStack->GetName());
		}

	}






	MyFbxImporterOutput::~MyFbxImporterOutput()
	{
		Clean();
	}

	void MyFbxImporterOutput::Clean()
	{
		for (auto m : meshes)
			delete(m);
		meshes.clear();

		for (auto m : skeleton_defs)
			delete(m);
		skeleton_defs.clear();  
	}

	MyFbxImporterParams::MyFbxImporterParams():
		import_scene_hierarchy(true),

		import_meshes(true),
		import_skeletons(true),
		import_materials(true),
		import_textures(true),
		import_characters(true),
		import_character_poses(true),
		import_poses(true),

		import_camera_nodes(true),
		import_light_nodes(true),
		import_marker_nodes(true),
		import_geometry_nodes(true),
		import_skeleton_nodes(true),

		remove_duplicated_vertex(true),
		vertex_transforms(1.0f) // identity matrix
	{
	}

	MyFbxImporter::MyFbxImporter() :
		current_skeleton_index(std::numeric_limits<size_t>::max()),
		current_bone_index(std::numeric_limits<size_t>::max())
	{

	}

	bool MyFbxImporter::ImportScene(boost::filesystem::path const & model_path, MyFbxImporterOutput & in_output, MyFbxImporterParams const & in_params)
	{
		bool result = false;

		FbxManager * lSdkManager = FbxManager::Create();
		if (lSdkManager != nullptr)
		{
			FbxIOSettings * ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
			if (ios != nullptr)
			{
				ios->SetBoolProp(IMP_FBX_MATERIAL, true);
				ios->SetBoolProp(IMP_FBX_TEXTURE, true);
				ios->SetBoolProp(IMP_FBX_LINK, true);
				ios->SetBoolProp(IMP_FBX_SHAPE, true);
				ios->SetBoolProp(IMP_FBX_GOBO, true);
				ios->SetBoolProp(IMP_FBX_ANIMATION, true);
				ios->SetBoolProp(IMP_FBX_MODEL, true);
				ios->SetBoolProp(IMP_FBX_CHARACTER, true);
				ios->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);

				lSdkManager->SetIOSettings(ios);

				FbxImporter * lImporter = FbxImporter::Create(lSdkManager, "");
				if (lImporter != nullptr)
				{
					if (!lImporter->Initialize(model_path.string().c_str(), -1, ios))
					{
						char const * error = lImporter->GetStatus().GetErrorString();
						if (error != nullptr)
							Log::Message("FBX Importer initialize failure [%s]", error);
					}
					else
					{
						params = in_params;
						output = &in_output;
						result = ImportScene(lSdkManager, lImporter);
					}
					lImporter->Destroy();
				}
				ios->Destroy();
			}
			lSdkManager->Destroy();
		}

		return result;
	}


	bool MyFbxImporter::ImportScene(FbxManager * lSdkManager, FbxImporter * lImporter)
	{
		bool result = false;

		FbxScene * myScene = FbxScene::Create(lSdkManager, ""); // create an empty scene
		if (myScene != nullptr)
		{
			if (lImporter->Import(myScene)) // populate the scene with the result of importer
				result = DoImportScene(myScene);
			myScene->Destroy();
		}
		return result;
	}

	bool MyFbxImporter::DoImportScene(FbxScene * myScene)
	{  
		if (DoImportSceneSkeletals(myScene))
			if (DoImportSceneResources(myScene))
				return DoImportSceneHierarchy(myScene);
		return false;
	}

	bool MyFbxImporter::DoImportSceneResources(FbxScene * myScene)
	{
		bool result = true;

		// read the characters
		if (params.import_characters)
		{
			int character_count = myScene->GetCharacterCount();
			for (int i = 0 ; (i < character_count) && (result) ; ++i)
			{
				FbxCharacter * character = myScene->GetCharacter(i);
				if (character == nullptr)
					continue;
				result &= DoImportCharacter(character);
			}  
		}

		// read the character poses
		if (params.import_character_poses)
		{
			int character_pose_count = myScene->GetCharacterPoseCount();
			for (int i = 0 ; (i < character_pose_count) && (result) ; ++i)
			{
				FbxCharacterPose * pose = myScene->GetCharacterPose(i);
				if (pose == nullptr)
					continue;
				result &= DoImportCharacterPose(pose);
			}
		}

		// read the poses
		if (params.import_poses)
		{
			int pose_count = myScene->GetPoseCount();
			for (int i = 0 ; (i < pose_count) && (result) ; ++i)
			{
				FbxPose * pose = myScene->GetPose(i);
				if (pose == nullptr)
					continue;
				result &= DoImportPose(pose);
			}
		}

		// read the materials
		if (params.import_materials)
		{
			int material_count = myScene->GetMaterialCount();
			for (int i = 0 ; (i < material_count) && (result) ; ++i)
			{
				FbxSurfaceMaterial * material = myScene->GetMaterial(i);
				if (material == nullptr)
					continue;
				result &= DoImportMaterial(material);
			}
		}

		// read the textures
		if (params.import_textures)
		{
			int texture_count = myScene->GetTextureCount();
			for (int i = 0 ; (i < texture_count) && (result) ; ++i)
			{
				FbxTexture * texture = myScene->GetTexture(i);
				if (texture == nullptr)
					continue;
				result &= DoImportTexture(texture);
			}
		}

		// read the geometries
		int geometry_count = myScene->GetGeometryCount();
		for (int i = 0 ; (i < geometry_count) && (result) ; ++i)
		{
			FbxGeometry * geometry = myScene->GetGeometry(i);
			if (geometry == nullptr)
				continue;
			result &= DoImportGeometry(geometry);
		} 

		return true;
	}

	bool MyFbxImporter::DoImportCharacter(FbxCharacter * character)
	{
		assert(character != nullptr);

		return true;
	}

	bool MyFbxImporter::DoImportPose(FbxPose * character)
	{
		assert(character != nullptr);

		return true;
	}

	bool MyFbxImporter::DoImportCharacterPose(FbxCharacterPose * character_pose)
	{
		assert(character_pose != nullptr);

		return true;
	}

	bool MyFbxImporter::DoImportMaterial(FbxSurfaceMaterial * material)
	{
		assert(material != nullptr);

		FbxImplementation const * ImplCGFX = GetImplementation(material, FBXSDK_IMPLEMENTATION_CGFX);
		if (ImplCGFX != nullptr)
		{

		}

		FbxImplementation const * ImplHLSL = GetImplementation(material, FBXSDK_IMPLEMENTATION_HLSL);
		if (ImplHLSL != nullptr)
		{

		}

		return true;
	}

	bool MyFbxImporter::DoImportTexture(FbxTexture * texture)
	{
		assert(texture != nullptr);

		return true;
	}

	bool MyFbxImporter::DoImportGeometry(FbxGeometry * geometry)
	{
		assert(geometry != nullptr);

		FbxNodeAttribute::EType type = geometry->GetAttributeType();

		if (type == FbxNodeAttribute::EType::eMesh)
		{
			if (params.import_meshes)
				return DoImportMesh((FbxMesh *)geometry);
		}
		return true;
	}

	bool MyFbxImporter::DoImportSceneHierarchy(FbxScene * myScene)
	{

		DisplaySceneAnimation(myScene);

		if (params.import_scene_hierarchy)
		{        
			FbxNode * root = myScene->GetRootNode();
			return DoImportSceneHierarchy(root, 0);
		}
		return true;
	}

	bool MyFbxImporter::DoImportSceneHierarchy(FbxNode * node, int level)
	{
		if (node == nullptr)
			return true;

		char const * name = node->GetName();

		FbxDouble3 translation = node->LclTranslation.Get();
		FbxDouble3 rotation = node->LclRotation.Get();
		FbxDouble3 scaling = node->LclScaling.Get();

		char const * type = nullptr;

		if (node->GetMarker())
			type = "MARKER   : ";
		else if (node->GetSkeleton())
			type = "SKELETON : ";
		else if (node->GetGeometry())
			type = "GEOMETRY : ";
		else if (node->GetCamera())
			type = "CAMERA   : ";
		else if (node->GetLight())
			type = "LIGHT    : ";
		else
			type = "UNKNOWN  : ";

		Log::Message("[%08x] %s%s : %s (%s)", node, type, StringTools::IndentString(level * 2).c_str(), name, node->GetTypeName());

		Log::Message(" NODE PROPERTIES :");
		DisplayCurves(node);
		Log::Message(" ATTRIBUTE PROPERTIES :");
		DisplayCurves(node->GetNodeAttribute());


		// try to convert node into marker
		if (params.import_marker_nodes)
		{
			FbxMarker * marker = node->GetMarker();
			if (marker != nullptr)
				return DoImportMarkerNode(marker, node, level);
		}

		// try to convert node into geometry
		if (params.import_geometry_nodes)
		{
			FbxGeometry * geometry = node->GetGeometry();
			if (geometry != nullptr)
				return DoImportGeometryNode(geometry, node, level);
		}

		// try to convert node into camera
		if (params.import_camera_nodes)
		{
			FbxCamera * camera = node->GetCamera();
			if (camera != nullptr)
				return DoImportCameraNode(camera, node, level);
		}

		// try to convert node into light
		if (params.import_light_nodes)
		{
			FbxLight * light = node->GetLight();
			if (light != nullptr)
				return DoImportLightNode(light, node, level);
		}

		// by default, import children
		return DoImportSceneHierarchyChildren(node, level);
	}

	bool MyFbxImporter::DoImportSceneHierarchyChildren(FbxNode * node, int level)
	{
		bool result = true;
		// recursive enumeration
		int count = node->GetChildCount();
		for (int i = 0; (i < count) && (result); ++i)
			result &= DoImportSceneHierarchy(node->GetChild(i), level + 1);
		return result;
	}

	bool MyFbxImporter::DoImportMarkerNode(FbxMarker * marker, FbxNode * node, int level)
	{
		assert(marker != nullptr);
		assert(node   != nullptr);

		return DoImportSceneHierarchyChildren(node, level);
	}

	bool MyFbxImporter::DoImportGeometryNode(FbxGeometry * geometry, FbxNode * node, int level)
	{
		assert(geometry != nullptr);
		assert(node     != nullptr);

		return true;
	}

	bool MyFbxImporter::DoImportCameraNode(FbxCamera * camera, FbxNode * node, int level)
	{
		assert(camera != nullptr);
		assert(node   != nullptr);

		return DoImportSceneHierarchyChildren(node, level);
	}

	bool MyFbxImporter::DoImportLightNode(FbxLight * light, FbxNode * node, int level)
	{
		assert(light != nullptr);
		assert(node  != nullptr);

		return DoImportSceneHierarchyChildren(node, level);
	}

}; // namespace chaos
