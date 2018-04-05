#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Buffer.h>
#include <chaos/GPUProgramSourceGenerator.h>
#include <chaos/GPUProgram.h>
#include <chaos/FilePath.h>

namespace chaos
{

	/**
	* GPUProgramLoaderCacheOptions : this class is used to define how cache sytem can be used
	*/

	class GPUProgramLoaderCacheOptions
	{
	public:

		/** whether we can use the cache as an input */
		bool read_from_cache = true;
		/** whether we can use the cache as an output */
		bool write_into_cache = true;
	};

	/**
	* GPUProgramLoader : this class deserves to generate GPU programs from sources.
	*                    It is possible to use cache system and to add some definitions so we can generate multiple programs with small macro differences.
	*/

	class GPUProgramLoader : protected GPUFileResourceFriend
	{
	public:

		/** destructor */
		virtual ~GPUProgramLoader() = default;

		/** Generate a program from a json content */
		virtual GPUProgram * GenProgramObject(nlohmann::json const & json, boost::filesystem::path const & config_path, GPUProgramLoaderCacheOptions const & cache_options = GPUProgramLoaderCacheOptions()) const;
		/** Generate a program from an file */
		virtual GPUProgram * GenProgramObject(FilePathParam const & path, GPUProgramLoaderCacheOptions const & cache_options = GPUProgramLoaderCacheOptions()) const;
	};

}; // namespace chaos
