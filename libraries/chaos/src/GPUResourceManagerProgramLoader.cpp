#include <chaos/GPUResourceManagerProgramLoader.h>
#include <chaos/GPUResourceManager.h>
#include <chaos/GPUProgramLoader.h>

namespace chaos
{

	GPUProgram * GPUResourceManagerProgramLoader::GenProgramObject(nlohmann::json const & json, boost::filesystem::path const & config_path, GPUProgramLoaderCacheOptions const & cache_options) const
	{
		if (!CheckResourceName(json))
			return nullptr;
		// load the program
		GPUProgram * program = GPUProgramLoader::GenProgramObject(json, config_path, cache_options);
		if (program != nullptr)
		{
			// should we update the path of the object ?
			if (!resource_name.empty())
			{
				char const * name = program->GetName();
				if (name == nullptr || name[0] == 0)
					SetResourceName(program, resource_name.c_str());
			}
		}
		return program;
	}

	GPUProgram * GPUResourceManagerProgramLoader::GenProgramObject(FilePathParam const & path, GPUProgramLoaderCacheOptions const & cache_options) const
	{
		if (!CheckResourcePath(path))
			return nullptr;
		// load the program
		GPUProgram * program = GPUProgramLoader::GenProgramObject(path, cache_options);
		if (program != nullptr)
		{
			// should we update the path of the object ?
			if (!resolved_path.empty() && program->GetPath().empty())
				SetResourcePath(program, resolved_path);
		}
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

}; // namespace chaos
