#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUResourceManagerLoader.h>
#include <chaos/GPUResourceManager.h>
#include <chaos/GPUTexture.h>
#include <chaos/GPUTextureLoader.h>
#include <chaos/JSONTools.h>
#include <chaos/FilePath.h>

namespace chaos
{

	class GPUResourceManagerTextureLoader : public GPUResourceManagerLoader<GPUTexture, GPUTextureLoader, GPUResourceManager>
	{
	public:

		/** constructor */
		GPUResourceManagerTextureLoader(GPUResourceManager * in_resource_manager) :
			GPUResourceManagerLoader<GPUTexture, GPUTextureLoader, GPUResourceManager>(in_resource_manager){}

		/** load an object from JSON */
		virtual GPUTexture * LoadObject(char const * keyname, nlohmann::json const & json, boost::filesystem::path const & config_path) const;
		/** texture loading from JSON */
		virtual GPUTexture * LoadObject(nlohmann::json const & json, boost::filesystem::path const & config_path, GenTextureParameters const & parameters) const;
		/** texture loading from path */
		virtual GPUTexture * LoadObject(FilePathParam const & path, GenTextureParameters const & parameters = GenTextureParameters()) const;

	protected:

		/** search whether the path is already in used in the manager */
		virtual bool IsPathAlreadyUsedInManager(FilePathParam const & path) const override;
		/** search whether the name is already in used in the manager */
		virtual bool IsNameAlreadyUsedInManager(std::string const & in_name) const override;
	};

}; // namespace chaos
