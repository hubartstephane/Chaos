#include <chaos/GPUResourceManagerTextureLoader.h>
#include <chaos/GPUResourceManager.h>
#include <chaos/GPUTextureLoader.h>

namespace chaos
{

	GPUTexture * GPUResourceManagerTextureLoader::LoadObject(nlohmann::json const & json, boost::filesystem::path const & config_path, GenTextureParameters const & parameters) const
	{
		if (!CheckResourceName(json))
			return nullptr;
		// load the texture
		GPUTexture * result = GPUTextureLoader::GenTextureObject(json, config_path, parameters);
		if (result != nullptr)
		{
			FinalizeLoadedResource(result);
			manager->textures.push_back(result);
		}
		return result;
	}

	GPUTexture * GPUResourceManagerTextureLoader::LoadObject(FilePathParam const & path, GenTextureParameters const & parameters) const
	{
		if (!CheckResourcePath(path))
			return nullptr;
		// load the texture
		GPUTexture * result = GPUTextureLoader::GenTextureObject(path, parameters);
		if (result != nullptr)
		{
			FinalizeLoadedResource(result);
			manager->textures.push_back(result);
		}
		return result;
	}

	bool GPUResourceManagerTextureLoader::IsPathAlreadyUsedInManager(FilePathParam const & path) const
	{
		return (manager->FindTextureByPath(path) != nullptr);
	}

	bool GPUResourceManagerTextureLoader::IsNameAlreadyUsedInManager(std::string const & in_name) const
	{ 
		return (manager->FindTexture(in_name.c_str()) != nullptr);
	}

	GPUTexture * GPUResourceManagerTextureLoader::LoadObject(char const * keyname, nlohmann::json const & json, boost::filesystem::path const & config_path) const
	{




		return nullptr;
	}


#if 0 // shuxxx MANAGER_LOADER

	GPUTexture * GPUResourceManager::LoadTexture(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		// ensure no name collision
		if (!CanAddTexture(name))
			return nullptr;
		// initialize the loader, so te name will be given to result at the end
		GPUResourceManagerTextureLoader loader(this);
		loader.SetResultName(name);

		// load the resource
		GPUTexture * result = loader.LoadObject(json, config_path, GenTextureParameters());
		if (result != nullptr)
			textures.push_back(result);

		return result;
	}

#endif


}; // namespace chaos
