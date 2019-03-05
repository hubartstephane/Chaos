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

	class GPUResourceManagerProgramLoader : public GPUResourceManagerLoader<GPUProgram, GPUProgramLoader, GPUResourceManager>
	{
	public:

		/** constructor */
		GPUResourceManagerProgramLoader(GPUResourceManager * in_resource_manager) :
			GPUResourceManagerLoader<GPUProgram, GPUProgramLoader, GPUResourceManager>(in_resource_manager){}

		/** load an object from JSON */
		virtual GPUProgram * LoadObject(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path, GPUProgramLoaderCacheOptions const & cache_options = GPUProgramLoaderCacheOptions()) const;
		/** program loading from path */
		virtual GPUProgram * LoadObject(FilePathParam const & path, char const * name = nullptr, GPUProgramLoaderCacheOptions const & cache_options = GPUProgramLoaderCacheOptions()) const;

	protected:

		/** search whether the path is already in used in the manager */
		virtual bool IsPathAlreadyUsedInManager(FilePathParam const & path) const override;
		/** search whether the name is already in used in the manager */
		virtual bool IsNameAlreadyUsedInManager(char const * in_name) const override;
	};

}; // namespace chaos
