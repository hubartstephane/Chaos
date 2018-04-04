#include <chaos/GPUProgramLoader.h>
#include <chaos/GLTools.h>
#include <chaos/GLShaderTools.h>
#include <chaos/LogTools.h>
#include <chaos/FileTools.h>
#include <chaos/StringTools.h>
#include <chaos/Buffer.h>
#include <chaos/GPUProgramData.h>

namespace chaos
{

	GPUProgram * GPUProgramLoader::GenProgramObject(nlohmann::json const & json, boost::filesystem::path const & config_path, GPUProgramLoaderCacheOptions const & cache_options) const
	{

		return nullptr;
	}
	
	GPUProgram * GPUProgramLoader::GenProgramObject(FilePathParam const & path, GPUProgramLoaderCacheOptions const & cache_options) const
	{
	
		return nullptr;
	}

}; // namespace chaos
