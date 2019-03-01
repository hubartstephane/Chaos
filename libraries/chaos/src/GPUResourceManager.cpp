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
		return FindObjectByName(name, textures);
	}

	GPUTexture const * GPUResourceManager::FindTexture(char const * name) const
	{
		return FindObjectByName(name, textures);
	}

	GPUTexture * GPUResourceManager::FindTextureByPath(FilePathParam const & path)
	{
		return FindObjectByPath(path, textures);
	}

	GPUTexture const * GPUResourceManager::FindTextureByPath(FilePathParam const & path) const
	{
		return FindObjectByPath(path, textures);
	}

	GPUProgram * GPUResourceManager::FindProgram(char const * name)
	{
		return FindObjectByName(name, programs);
	}

	GPUProgram const * GPUResourceManager::FindProgram(char const * name) const
	{
		return FindObjectByName(name, programs);
	}

	GPUProgram * GPUResourceManager::FindProgramByPath(FilePathParam const & path)
	{
		return FindObjectByPath(path, programs);
	}

	GPUProgram const * GPUResourceManager::FindProgramByPath(FilePathParam const & path) const
	{
		return FindObjectByPath(path, programs);
	}

	GPURenderMaterial * GPUResourceManager::FindRenderMaterial(char const * name)
	{
		return FindObjectByName(name, render_materials);
	}

	GPURenderMaterial const * GPUResourceManager::FindRenderMaterial(char const * name) const
	{
		return FindObjectByName(name, render_materials);
	}

	GPURenderMaterial * GPUResourceManager::FindRenderMaterialByPath(FilePathParam const & path)
	{
		return FindObjectByPath(path, render_materials);
	}

	GPURenderMaterial const * GPUResourceManager::FindRenderMaterialByPath(FilePathParam const & path) const
	{
		return FindObjectByPath(path, render_materials);
	}

	GPUTexture * GPUResourceManager::LoadTexture(FilePathParam const & path, GenTextureParameters const & texture_parameters)
	{
		boost::filesystem::path const resolved_path = path.GetResolvedPath();
		return LoadTexture(path, BoostTools::PathToName(resolved_path).c_str(), texture_parameters);
	}

	GPUTexture * GPUResourceManager::LoadTexture(FilePathParam const & path, char const * name, GenTextureParameters const & texture_parameters)
	{
		// object already existing ?
		if (!CanAddTexture(name))
			return nullptr;
		if (FindTextureByPath(path) != nullptr)
			return nullptr;
		// prepare the loader : give it the name and the path, so that at the end it can update result members
		GPUResourceManagerTextureLoader loader(this);
		loader.SetResultPath(path.GetResolvedPath());
		loader.SetResultName(name);
		return loader.LoadObject(path, texture_parameters);
	}

	GPUProgram * GPUResourceManager::LoadProgram(FilePathParam const & path, GPUProgramLoaderCacheOptions const & cache_options)
	{
		boost::filesystem::path const resolved_path = path.GetResolvedPath();
		return LoadProgram(path, BoostTools::PathToName(resolved_path).c_str(), cache_options);
	}

	GPUProgram * GPUResourceManager::LoadProgram(FilePathParam const & path, char const * name, GPUProgramLoaderCacheOptions const & cache_options)
	{
		// object already existing ?
		if (!CanAddProgram(name))
			return nullptr;
		if (FindProgramByPath(path) != nullptr)
			return nullptr;
		// prepare the loader: give it the name and the path, so that at the end it can update result members
		GPUResourceManagerProgramLoader loader(this);
		loader.SetResultPath(path.GetResolvedPath());
		loader.SetResultName(name);
		return loader.LoadObject(path, cache_options);
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
		// prepare the loader : give it the name and the path, so that at the end it can update result members
		GPURenderMaterialLoader loader(this);
		loader.SetResultPath(path.GetResolvedPath());
		loader.SetResultName(name);

		// load data
		std::string parent_name;

		GPURenderMaterial * result = loader.LoadObject(path, parent_name);
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
		if (!LoadTexturesFromConfiguration(json, config_path))
			return false;
		if (!LoadProgramsFromConfiguration(json, config_path))
			return false;
		if (!LoadMaterialsFromConfiguration(json, config_path))
			return false;
		return true;
	}

	bool GPUResourceManager::LoadTexturesFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		return LoadObjectsFromConfiguration(
			"textures",
			json,
			config_path,
			GPUResourceManagerTextureLoader(this));
	}

	bool GPUResourceManager::LoadProgramsFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		return LoadObjectsFromConfiguration(
			"programs", 
			json, 
			config_path, 
			GPUResourceManagerProgramLoader(this));
	}

	bool GPUResourceManager::LoadMaterialsFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		return LoadObjectsFromConfiguration(
			"rendermaterials",
			json,
			config_path,
			GPURenderMaterialLoader(this));
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
