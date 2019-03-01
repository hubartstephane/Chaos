#include <chaos/GPUResourceManagerProgramLoader.h>
#include <chaos/GPUResourceManager.h>
#include <chaos/GPUProgramLoader.h>

namespace chaos
{

	GPUProgram * GPUResourceManagerProgramLoader::LoadObject(nlohmann::json const & json, boost::filesystem::path const & config_path, GPUProgramLoaderCacheOptions const & cache_options) const
	{
		if (!CheckResourceName(json))
			return nullptr;
		// load the program
		GPUProgram * program = GPUProgramLoader::GenProgramObject(json, config_path, cache_options);
		if (program != nullptr)
			FinalizeLoadedResource(program);
		return program;
	}

	GPUProgram * GPUResourceManagerProgramLoader::LoadObject(FilePathParam const & path, GPUProgramLoaderCacheOptions const & cache_options) const
	{
		if (!CheckResourcePath(path))
			return nullptr;
		// load the program
		GPUProgram * program = GPUProgramLoader::GenProgramObject(path, cache_options);
		if (program != nullptr)
			FinalizeLoadedResource(program);
		return program;
	}

	bool GPUResourceManagerProgramLoader::IsPathAlreadyUsedInManager(FilePathParam const & path) const
	{
		return (manager->FindProgramByPath(path) != nullptr);
	}

	bool GPUResourceManagerProgramLoader::IsNameAlreadyUsedInManager(std::string const & in_name) const
	{ 
		return (manager->FindProgram(in_name.c_str()) != nullptr);
	}

	GPUProgram * GPUResourceManagerProgramLoader::LoadObject(char const * keyname, nlohmann::json const & json, boost::filesystem::path const & config_path) const
	{




		return nullptr;
	}


#if 0 // shuxxx MANAGER_LOADER

	GPUProgram * GPUResourceManager::LoadProgram(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		// ensure no name collision
		if (!CanAddProgram(name))
			return nullptr;
		// initialize the loader, so te name will be given to result at the end
		GPUResourceManagerProgramLoader loader(this);
		loader.SetResultName(name);

		// load the resource
		GPUProgram * program = loader.LoadObject(json, config_path, GPUProgramLoaderCacheOptions());
		if (program != nullptr)
			programs.push_back(program);
		return program;
	}
#endif







}; // namespace chaos
