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
		GPURenderMaterial * result = GPURenderMaterialLoader(this).LoadObject(path, name);
		if (result != nullptr)
			CheckForRenderMaterialInheritance();
		return result;
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
		bool result = LoadObjectsFromConfiguration(
			"rendermaterials",
			json,
			config_path,
			boost::mpl::true_(),
			GPURenderMaterialLoader(this));
		if (result)
			CheckForRenderMaterialInheritance();
		return result;
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

	void GPUResourceManager::CheckForRenderMaterialInheritance()
	{
		size_t count = render_materials.size();
		for (size_t i = 0; i < count; ++i)
		{
			GPURenderMaterial * render_material = render_materials[i].get();
			if (render_material == nullptr)
				continue;
			if (render_material->parent_material == nullptr && !render_material->parent_name.empty())
				SetRenderMaterialParent(render_material, render_material->parent_name);
		}
	}

	bool GPUResourceManager::RefreshGPUResources(GPUResourceManager * other_gpu_manager)
	{
		GPUResourceManagerReloadData reload_data;

		assert(other_gpu_manager != nullptr);
		if (!RefreshTextures(other_gpu_manager, reload_data))
			return false;
		if (!RefreshPrograms(other_gpu_manager, reload_data))
			return false;
		if (!RefreshMaterial(other_gpu_manager, reload_data))
			return false;
		return true;
	}

	template<typename FIND_BY_NAME, typename FIND_BY_PATH, typename RESOURCE_VECTOR, typename SWAP_OBJECTS_FUNC> 
	void RefreshObjects(FIND_BY_NAME find_by_name, FIND_BY_PATH find_by_path, RESOURCE_VECTOR resource_vector, GPUResourceManager * gpu_manager, GPUResourceManager * other_gpu_manager, SWAP_OBJECTS_FUNC swap_objects)
	{
		auto & ori_objects = gpu_manager->*resource_vector;
		auto & new_objects = other_gpu_manager->*resource_vector;

		// update all resources that are newer 
		size_t ori_count = ori_objects.size();
		for (size_t i = 0; i < ori_count; ++i)
		{
			// original object
			auto * ori_object = ori_objects[i].get();
			if (ori_object == nullptr)
				continue;
			// find corresponding resource in new manager
			decltype(ori_object) other_object = nullptr;
			if (ori_object->GetName() != nullptr)
				other_object = (other_gpu_manager->*find_by_name)(ori_object->GetName());
			if (other_object == nullptr)
				other_object = (other_gpu_manager->*find_by_path)(ori_object->GetPath());
			if (other_object == nullptr)
				continue;

			// XXX : we have to implement a real file depencie system to test for timestamp
			//       => for example, we may modify a Shader file without the Program noticing it is out dated
#if 0
			// test whether the other resource is effectively newer ?
			std::time_t t1 = ori_object->GetFileTimestamp();
			std::time_t t2 = other_object->GetFileTimestamp();
			if (t1 >= t2)
				continue;
#endif
			// swap the resources
			swap_objects(ori_object, other_object);
		}
		// add new resources
		size_t new_count = new_objects.size();
		for (size_t i = 0; i < new_count; ++i)
		{
			// original object
			auto * new_object = new_objects[i].get();
			if (new_object == nullptr)
				continue;
			// find whether this object is new
			decltype(new_object) ori_object = nullptr;
			if (new_object->GetName() != nullptr)
				ori_object = (gpu_manager->*find_by_name)(new_object->GetName());
			if (ori_object == nullptr)
				ori_object = (gpu_manager->*find_by_path)(new_object->GetPath());
			if (ori_object != nullptr)
				continue;
			// copy the object in the original manager
			ori_objects.push_back(new_object);
		}
		// resources that have been destroyed are simply ignored (we have no real way to be sure they are not used)
	}
		

	bool GPUResourceManager::RefreshTextures(GPUResourceManager * other_gpu_manager, GPUResourceManagerReloadData & reload_data)
	{
		assert(other_gpu_manager != nullptr);

		GPUTexture * (GPUResourceManager::*find_by_name)(char const *) = &GPUResourceManager::FindTexture;
		GPUTexture * (GPUResourceManager::*find_by_path)(FilePathParam const &) = &GPUResourceManager::FindTextureByPath;
		 
		std::vector<shared_ptr<GPUTexture>> GPUResourceManager::*resource_vector = &GPUResourceManager::textures;

		RefreshObjects(find_by_name, find_by_path, resource_vector, this, other_gpu_manager, [&reload_data](GPUTexture * ori_object, GPUTexture * other_object){

			reload_data.texture_map[ori_object] = other_object;

			std::swap(ori_object->texture_id, other_object->texture_id);
			std::swap(ori_object->file_timestamp, other_object->file_timestamp);
			std::swap(ori_object->texture_description, other_object->texture_description);
		});
		return true;
	}

	bool GPUResourceManager::RefreshPrograms(GPUResourceManager * other_gpu_manager, GPUResourceManagerReloadData & reload_data)
	{
		assert(other_gpu_manager != nullptr);

		GPUProgram * (GPUResourceManager::*find_by_name)(char const *) = &GPUResourceManager::FindProgram;
		GPUProgram * (GPUResourceManager::*find_by_path)(FilePathParam const &) = &GPUResourceManager::FindProgramByPath;

		std::vector<shared_ptr<GPUProgram>> GPUResourceManager::*resource_vector = &GPUResourceManager::programs;

		RefreshObjects(find_by_name, find_by_path, resource_vector, this, other_gpu_manager, [&reload_data](GPUProgram * ori_object, GPUProgram * other_object) {

			reload_data.program_map[ori_object] = other_object;

			std::swap(ori_object->program_id, other_object->program_id);
			std::swap(ori_object->file_timestamp, other_object->file_timestamp);
			std::swap(ori_object->program_data, other_object->program_data);
		});

		return true;
	}

	bool GPUResourceManager::RefreshMaterial(GPUResourceManager * other_gpu_manager, GPUResourceManagerReloadData & reload_data)
	{
		assert(other_gpu_manager != nullptr);


		GPURenderMaterial * (GPUResourceManager::*find_by_name)(char const *) = &GPUResourceManager::FindRenderMaterial;
		GPURenderMaterial * (GPUResourceManager::*find_by_path)(FilePathParam const &) = &GPUResourceManager::FindRenderMaterialByPath;

		std::vector<shared_ptr<GPURenderMaterial>> GPUResourceManager::*resource_vector = &GPUResourceManager::render_materials;

		RefreshObjects(find_by_name, find_by_path, resource_vector, this, other_gpu_manager, [&reload_data](GPURenderMaterial * ori_object, GPURenderMaterial * other_object) {

			reload_data.render_material_map[ori_object] = other_object;

			// shuxxx RefreshMaterial

			std::swap(ori_object->program, other_object->program); // program_map
			std::swap(ori_object->file_timestamp, other_object->file_timestamp);
			std::swap(ori_object->parent_name, other_object->parent_name);
			// shuxxx std::swap(ori_object->uniform_provider, other_object->uniform_provider);
		});


		return true;
	}


}; // namespace chaos
