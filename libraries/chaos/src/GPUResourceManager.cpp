#include <chaos/GPUResourceManager.h>
#include <chaos/BoostTools.h>
#include <chaos/GLTextureTools.h>
#include <chaos/GPUTextureLoader.h>
#include <chaos/GPUProgramLoader.h>
#include <chaos/GPURenderMaterialLoader.h>
#include <chaos/GPUProgramGenerator.h>
#include <chaos/GPUProgramLoader.h>

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

	GPUTexture * GPUResourceManager::LoadTexture(FilePathParam const & path, char const * name, GenTextureParameters const & texture_parameters)
	{
		return GPUTextureLoader(this).LoadObject(path, name, texture_parameters);
	}

	GPUProgram * GPUResourceManager::LoadProgram(FilePathParam const & path, char const * name, GPUProgramLoaderCacheOptions const & cache_options)
	{
		return GPUProgramLoader(this).LoadObject(path, name, cache_options);
	}

	GPURenderMaterial * GPUResourceManager::LoadRenderMaterial(FilePathParam const & path, char const * name)
	{
		return GPURenderMaterialLoader(this).LoadObject(path, name);
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
			boost::mpl::true_(),
			GPUTextureLoader(this));
	}

	bool GPUResourceManager::LoadProgramsFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		return LoadObjectsFromConfiguration(
			"programs", 
			json, 
			config_path, 
			boost::mpl::true_(),
			GPUProgramLoader(this));
	}

	bool GPUResourceManager::LoadMaterialsFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		return LoadObjectsFromConfiguration(
			"rendermaterials",
			json,
			config_path,
			boost::mpl::true_(),
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


	bool GPUResourceManager::RefreshGPUResources(GPUResourceManager * other_gpu_manager)
	{
		assert(other_gpu_manager != nullptr);
		if (!RefreshTextures(other_gpu_manager))
			return false;
		if (!RefreshPrograms(other_gpu_manager))
			return false;
		if (!RefreshMaterial(other_gpu_manager))
			return false;
		return true;
	}

	bool GPUResourceManager::RefreshTextures(GPUResourceManager * other_gpu_manager)
	{
		assert(other_gpu_manager != nullptr);
		
		// update all resources that are newer 
		size_t ori_count = textures.size();
		for (size_t i = 0; i < ori_count; ++i)
		{
			// original object
			GPUTexture * ori_texture = textures[i].get();
			if (ori_texture == nullptr)
				continue;
			// find corresponding resource in new manager
			GPUTexture * other_texture = nullptr;
			if (ori_texture->GetName() != nullptr)
				other_texture = other_gpu_manager->FindTexture(ori_texture->GetName());
			if (other_texture == nullptr)
				other_texture = other_gpu_manager->FindTextureByPath(ori_texture->GetPath());			
			if (other_texture == nullptr)
				continue;
			// test whether the other resource is effectively newer ?
			if (ori_texture->GetFileTimestamp() >= other_texture->GetFileTimestamp())
				continue;
			// swap the resources
			std::swap(ori_texture->texture_id, other_texture->texture_id);
			std::swap(ori_texture->file_timestamp, other_texture->file_timestamp);
			std::swap(ori_texture->texture_description, other_texture->texture_description);
		}

		// add new resources
		size_t new_count = other_gpu_manager->textures.size();
		for (size_t i = 0; i < new_count; ++i)
		{
			// original object
			GPUTexture * new_texture = other_gpu_manager->textures[i].get();
			if (new_texture == nullptr)
				continue;
			// find whether this texture is new
			GPUTexture * ori_texture = nullptr;
			if (new_texture->GetName() != nullptr)
				ori_texture = other_gpu_manager->FindTexture(new_texture->GetName());
			if (ori_texture == nullptr)
				ori_texture = other_gpu_manager->FindTextureByPath(new_texture->GetPath());
			if (ori_texture != nullptr)
				continue;
			// copy the texture in the original manager
			textures.push_back(new_texture);
		}
		// resources that have been destroyed are simply ignored (we have no real way to be sure they are not used)
		return true;
	}

	bool GPUResourceManager::RefreshPrograms(GPUResourceManager * other_gpu_manager)
	{
		assert(other_gpu_manager != nullptr);



		return true;
	}

	bool GPUResourceManager::RefreshMaterial(GPUResourceManager * other_gpu_manager)
	{
		assert(other_gpu_manager != nullptr);


		return true;
	}


}; // namespace chaos
