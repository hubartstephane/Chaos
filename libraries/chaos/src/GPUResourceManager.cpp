#include <chaos/GPUResourceManager.h>
#include <chaos/BoostTools.h>
#include <chaos/GLTextureTools.h>
#include <chaos/GPUTextureLoader.h>
#include <chaos/GPUProgramLoader.h>
#include <chaos/GPURenderMaterialLoader.h>
#include <chaos/GPUProgramGenerator.h>
#include <chaos/GPUResourceManagerTextureLoader.h>
#include <chaos/GPUResourceManagerProgramLoader.h>

namespace chaos
{

	/**
	* GPUResourceManager
	**/

	GPUResourceManager::~GPUResourceManager()
	{
		Release();
	}

	void GPUResourceManager::Release()
	{
		textures.clear();
		programs.clear();
		render_materials.clear();
	}

	GPUTexture * GPUResourceManager::FindTexture(char const * name)
	{
		return FindObjectByName<GPUTexture>(name, textures);
	}

	GPUTexture const * GPUResourceManager::FindTexture(char const * name) const
	{
		return FindObjectByName<GPUTexture>(name, textures);
	}

	GPUTexture * GPUResourceManager::FindTextureByPath(FilePathParam const & path)
	{
		return FindObjectByPath<GPUTexture>(path, textures);
	}

	GPUTexture const * GPUResourceManager::FindTextureByPath(FilePathParam const & path) const
	{
		return FindObjectByPath<GPUTexture>(path, textures);
	}

	GPUProgram * GPUResourceManager::FindProgram(char const * name)
	{
		return FindObjectByName<GPUProgram>(name, programs);
	}

	GPUProgram const * GPUResourceManager::FindProgram(char const * name) const
	{
		return FindObjectByName<GPUProgram>(name, programs);
	}

	GPUProgram * GPUResourceManager::FindProgramByPath(FilePathParam const & path)
	{
		return FindObjectByPath<GPUProgram>(path, programs);
	}

	GPUProgram const * GPUResourceManager::FindProgramByPath(FilePathParam const & path) const
	{
		return FindObjectByPath<GPUProgram>(path, programs);
	}

	GPURenderMaterial * GPUResourceManager::FindRenderMaterial(char const * name)
	{
		return FindObjectByName<GPURenderMaterial>(name, render_materials);
	}

	GPURenderMaterial const * GPUResourceManager::FindRenderMaterial(char const * name) const
	{
		return FindObjectByName<GPURenderMaterial>(name, render_materials);
	}

	GPURenderMaterial * GPUResourceManager::FindRenderMaterialByPath(FilePathParam const & path)
	{
		return FindObjectByPath<GPURenderMaterial>(path, render_materials);
	}

	GPURenderMaterial const * GPUResourceManager::FindRenderMaterialByPath(FilePathParam const & path) const
	{
		return FindObjectByPath<GPURenderMaterial>(path, render_materials);
	}

	GPUTexture * GPUResourceManager::LoadTexture(FilePathParam const & path)
	{
		boost::filesystem::path const resolved_path = path.GetResolvedPath();
		return LoadTexture(path, BoostTools::PathToName(resolved_path).c_str());
	}

	GPUTexture * GPUResourceManager::LoadTexture(FilePathParam const & path, char const * name)
	{
		// object already existing ?
		if (!CanAddTexture(name))
			return nullptr;
		if (FindTextureByPath(path) != nullptr)
			return nullptr;

		// prepare the loader
		//  - give it the name and the path, so that at the end it can update result members
		GPUResourceManagerTextureLoader loader(this);
		loader.SetResultPath(path.GetResolvedPath());
		loader.SetResultName(name);

		// load data
		GPUTexture * result = loader.GenTextureObject(path, GenTextureParameters());
		if (result != nullptr)
			textures.push_back(result);
		return result;
	}

	GPUProgram * GPUResourceManager::LoadProgram(FilePathParam const & path)
	{
		boost::filesystem::path const resolved_path = path.GetResolvedPath();
		return LoadProgram(path, BoostTools::PathToName(resolved_path).c_str());
	}

	GPUProgram * GPUResourceManager::LoadProgram(FilePathParam const & path, char const * name)
	{
		// object already existing ?
		if (!CanAddProgram(name))
			return nullptr;
		if (FindProgramByPath(path) != nullptr)
			return nullptr;

		// prepare the loader
		//  - give it the name and the path, so that at the end it can update result members
		GPUResourceManagerProgramLoader loader(this);
		loader.SetResultPath(path.GetResolvedPath());
		loader.SetResultName(name);

		// load data
		GPUProgram * result = loader.GenProgramObject(path, GPUProgramLoaderCacheOptions());
		if (result != nullptr)
			programs.push_back(result);
		return result;
	}

	GPURenderMaterial * GPUResourceManager::LoadRenderMaterial(FilePathParam const & path)
	{
		boost::filesystem::path const resolved_path = path.GetResolvedPath();
		return LoadRenderMaterial(path, BoostTools::PathToName(resolved_path).c_str());
	}

	GPURenderMaterial * GPUResourceManager::LoadRenderMaterial(FilePathParam const & path, char const * name)
	{
		// object already existing ?
		if (!CanAddRenderMaterial(name))
			return nullptr;
		if (FindRenderMaterialByPath(path) != nullptr)
			return nullptr;

		// prepare the loader
		//  - give it the name and the path, so that at the end it can update result members
		GPURenderMaterialLoader loader(this);
		loader.SetResultPath(path.GetResolvedPath());
		loader.SetResultName(name);

		// load data
		std::string parent_name;

		GPURenderMaterial * result = loader.GenRenderMaterialObject(path, parent_name);
		if (result != nullptr)
		{
			SetRenderMaterialParent(result, parent_name);
			render_materials.push_back(result);
		}			
		return result;
	}

	bool GPUResourceManager::CanAddTexture(char const * name) const
	{
		return CanAddObject(name, [this](char const * n) {return FindTexture(n); } );
	}

	bool GPUResourceManager::CanAddProgram(char const * name) const
	{
		return CanAddObject(name, [this](char const * n) {return FindProgram(n); } );
	}

	bool GPUResourceManager::CanAddRenderMaterial(char const * name) const
	{
		return CanAddObject(name, [this](char const * n) {return FindRenderMaterial(n); } );
	}

	bool GPUResourceManager::LoadManager(FilePathParam const & path)
	{
		nlohmann::json json;
		if (!JSONTools::LoadJSONFile(path, json, true))
			return true;
		return InitializeFromConfiguration(json, path.GetResolvedPath());	
	}

	bool GPUResourceManager::InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		if (!InitializeTexturesFromConfiguration(json, config_path))
			return false;
		if (!InitializeProgramsFromConfiguration(json, config_path))
			return false;
		if (!InitializeMaterialsFromConfiguration(json, config_path))
			return false;
		return true;
	}

	bool GPUResourceManager::InitializeTexturesFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		return InitializeObjectsFromConfiguration(
			"textures", 
			json, 
			config_path, 
			[this](char const * name, nlohmann::json const & obj_json, boost::filesystem::path const & path)
			{
				LoadTexture(name, obj_json, path);
			}
		);
		return true;
	}

	bool GPUResourceManager::InitializeProgramsFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		return InitializeObjectsFromConfiguration(
			"programs", 
			json, 
			config_path, 
			[this](char const * name, nlohmann::json const & obj_json, boost::filesystem::path const & path)
			{
				LoadProgram(name, obj_json, path);
			}
		);
	}

	bool GPUResourceManager::InitializeMaterialsFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		std::map<GPURenderMaterial *, std::string> parenting_map;

		bool result = InitializeObjectsFromConfiguration(
			"rendermaterials",
			json,
			config_path,
			[this, &parenting_map](char const * name, nlohmann::json const & obj_json, boost::filesystem::path const & path)
			{
				std::string parent_name;

				GPURenderMaterial * render_material = LoadRenderMaterial(name, obj_json, path, parent_name);
				if (render_material != nullptr && !parent_name.empty())
					parenting_map[render_material] = std::move(parent_name);
			}
		);
		// resolve the parenting
		if (result)
		{
			for (auto it : parenting_map)
				SetRenderMaterialParent(it.first, it.second);
		}
		return result;
	}

	GPUTexture * GPUResourceManager::LoadTexture(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		// ensure no name collision
		if (!CanAddTexture(name))
			return nullptr;
		// initialize the loader, so te name will be given to result at the end
		GPUResourceManagerTextureLoader loader(this);
		loader.SetResultName(name);

		// load the resource
		GPUTexture * result = loader.GenTextureObject(json, config_path, GenTextureParameters());
		if (result != nullptr)
			textures.push_back(result);
		return result;
	}

	GPUProgram * GPUResourceManager::LoadProgram(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		// ensure no name collision
		if (!CanAddProgram(name))
			return nullptr;
		// initialize the loader, so te name will be given to result at the end
		GPUResourceManagerProgramLoader loader(this);
		loader.SetResultName(name);

		// load the resource
		GPUProgram * program = loader.GenProgramObject(json, config_path, GPUProgramLoaderCacheOptions());
		if (program != nullptr)
			programs.push_back(program);
		return program;
	}

	GPURenderMaterial * GPUResourceManager::LoadRenderMaterial(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path, std::string & parent_name)
	{
		// ensure no name collision
		if (!CanAddRenderMaterial(name))
			return nullptr;
		// initialize the loader, so te name will be given to result at the end
		GPURenderMaterialLoader loader(this);
		loader.SetResultName(name);

		// load the resource
		GPURenderMaterial * render_material = loader.GenRenderMaterialObject(json, config_path, parent_name);
		if (render_material != nullptr)
			render_materials.push_back(render_material);
		return render_material;
	}

	void GPUResourceManager::SetRenderMaterialParent(GPURenderMaterial * render_material, std::string const & parent_name)
	{
		assert(render_material != nullptr);

		if (parent_name.empty())
			return;
		GPURenderMaterial * parent = FindRenderMaterial(parent_name.c_str());
		if (parent != nullptr)
			render_material->SetParentMaterial(parent); // some recursive verification here
	}

}; // namespace chaos
