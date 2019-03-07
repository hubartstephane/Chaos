#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Buffer.h>
#include <chaos/GPUProgramSourceGenerator.h>
#include <chaos/ResourceManagerLoader.h>
#include <chaos/GPUProgram.h>
#include <chaos/FilePath.h>
#include <chaos/JSONTools.h>
#include <chaos/FileResource.h>
#include <chaos/GPUResourceManager.h>

namespace chaos
{

	/**
	* GPUProgramLoader : this class deserves to generate GPU programs from sources.
	*                    It is possible to use cache system and to add some definitions so we can generate multiple programs with small macro differences.
	*/

	class GPUProgramLoader : public ResourceManagerLoader<GPUProgram, ResourceFriend, GPUResourceManager>
	{
	public:

		/** constructor */
		GPUProgramLoader(GPUResourceManager * in_resource_manager = nullptr) :
			ResourceManagerLoader<GPUProgram, ResourceFriend, GPUResourceManager>(in_resource_manager){}

		/** load an object from JSON */
		virtual GPUProgram * LoadObject(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path, GPUProgramLoaderCacheOptions const & cache_options = GPUProgramLoaderCacheOptions()) const;
		/** program loading from path */
		virtual GPUProgram * LoadObject(FilePathParam const & path, char const * name = nullptr, GPUProgramLoaderCacheOptions const & cache_options = GPUProgramLoaderCacheOptions()) const;

		/** Generate a program from a json content */
		virtual GPUProgram * GenProgramObject(nlohmann::json const & json, boost::filesystem::path const & config_path, GPUProgramLoaderCacheOptions const & cache_options = GPUProgramLoaderCacheOptions()) const;
		/** Generate a program from an file */
		virtual GPUProgram * GenProgramObject(FilePathParam const & path, GPUProgramLoaderCacheOptions const & cache_options = GPUProgramLoaderCacheOptions()) const;

	protected:

		/** search whether the path is already in used in the manager */
		virtual bool IsPathAlreadyUsedInManager(FilePathParam const & path) const override;
		/** search whether the name is already in used in the manager */
		virtual bool IsNameAlreadyUsedInManager(char const * in_name) const override;
	};

}; // namespace chaos
