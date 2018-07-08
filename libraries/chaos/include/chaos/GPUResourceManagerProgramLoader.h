#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUResourceManagerLoader.h>
#include <chaos/GPUResourceManager.h>
#include <chaos/GPUProgram.h>
#include <chaos/GPUProgramLoader.h>
#include <chaos/JSONTools.h>
#include <chaos/FilePath.h>

namespace chaos
{

	class GPUResourceManagerProgramLoader : public GPUResourceManagerLoader<GPUProgramLoader, GPUResourceManager>
	{
	public:

		/** constructor */
		GPUResourceManagerProgramLoader(GPUResourceManager * in_resource_manager) :
			GPUResourceManagerLoader<GPUProgramLoader, GPUResourceManager>(in_resource_manager)
		{

		}

		/** program loading from JSON */
		virtual GPUProgram * GenProgramObject(nlohmann::json const & json, boost::filesystem::path const & config_path, GPUProgramLoaderCacheOptions const & cache_options = GPUProgramLoaderCacheOptions()) const override;
		/** program loading from path */
		virtual GPUProgram * GenProgramObject(FilePathParam const & path, GPUProgramLoaderCacheOptions const & cache_options = GPUProgramLoaderCacheOptions()) const override;

	protected:

		/** search whether the path is already in used in the manager */
		virtual bool IsPathAlreadyUsedInManager(FilePathParam const & path) const override;
		/** search whether the name is already in used in the manager */
		virtual bool IsNameAlreadyUsedInManager(std::string const & in_name) const override;
	};

}; // namespace chaos
