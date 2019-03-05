#include <chaos/GPUResourceManagerTextureLoader.h>
#include <chaos/GPUResourceManager.h>
#include <chaos/GPUTextureLoader.h>

namespace chaos
{

	GPUTexture * GPUResourceManagerTextureLoader::LoadObject(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path, GenTextureParameters const & parameters) const
	{
		// check for name
		if (!CheckResourceName(nullptr, name, &json))
			return nullptr;
		// load the texture
		GPUTexture * result = GPUTextureLoader().GenTextureObject(json, config_path, parameters);
		if (result != nullptr)
		{
			FinalizeLoadedResource(result);
			manager->textures.push_back(result);
		}
		return result;
	}

	GPUTexture * GPUResourceManagerTextureLoader::LoadObject(FilePathParam const & path, char const * name, GenTextureParameters const & parameters) const
	{
		// check for path
		if (!CheckResourcePath(path))
			return nullptr;
		// check for name
		if (!CheckResourceName(&path.GetResolvedPath(), name, nullptr))
			return nullptr;
		// load the texture
		GPUTexture * result = GPUTextureLoader().GenTextureObject(path, parameters);
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

	bool GPUResourceManagerTextureLoader::IsNameAlreadyUsedInManager(char const * in_name) const
	{ 
		return (manager->FindTexture(in_name) != nullptr);
	}

}; // namespace chaos
