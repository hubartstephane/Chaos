#include <chaos/GPUResourceManagerProgramLoader.h>
#include <chaos/GPUResourceManager.h>
#include <chaos/GPUProgramLoader.h>

namespace chaos
{
	GPUProgram * GPUResourceManagerProgramLoader::LoadObject(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path, GPUProgramLoaderCacheOptions const & cache_options) const
	{
		// check for name
		if (!CheckResourceName(nullptr, name, &json))
			return nullptr;
		// load the texture
		GPUProgram * result = GPUProgramLoader::GenProgramObject(json, config_path, cache_options);
		if (result != nullptr)
		{
			FinalizeLoadedResource(result);
			manager->programs.push_back(result);
		}
		return result;
	}

	GPUProgram * GPUResourceManagerProgramLoader::LoadObject(FilePathParam const & path, char const * name, GPUProgramLoaderCacheOptions const & cache_options) const
	{
		// check for path
		if (!CheckResourcePath(path))
			return nullptr;
		// check for name
		if (!CheckResourceName(&path.GetResolvedPath(), name, nullptr))
			return nullptr;
		// load the texture
		GPUProgram * result = GPUProgramLoader::GenProgramObject(path, cache_options);
		if (result != nullptr)
		{
			FinalizeLoadedResource(result);
			manager->programs.push_back(result);
		}
		return result;
	}

	bool GPUResourceManagerProgramLoader::IsPathAlreadyUsedInManager(FilePathParam const & path) const
	{
		return (manager->FindProgramByPath(path) != nullptr);
	}

	bool GPUResourceManagerProgramLoader::IsNameAlreadyUsedInManager(char const * in_name) const
	{ 
		return (manager->FindProgram(in_name) != nullptr);
	}

}; // namespace chaos
