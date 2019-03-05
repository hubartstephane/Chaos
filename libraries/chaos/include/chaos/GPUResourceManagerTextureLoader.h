#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ResourceManagerLoader.h>
#include <chaos/GPUResourceManager.h>
#include <chaos/GPUTexture.h>
#include <chaos/GPUTextureLoader.h>
#include <chaos/JSONTools.h>
#include <chaos/FilePath.h>

namespace chaos
{

	class GPUResourceManagerTextureLoader : public ResourceManagerLoader<GPUTexture, GPUFileResourceFriend, GPUResourceManager>
	{
	public:

		/** constructor */
		GPUResourceManagerTextureLoader(GPUResourceManager * in_resource_manager) :
			ResourceManagerLoader<GPUTexture, GPUFileResourceFriend, GPUResourceManager>(in_resource_manager){}

		/** load an object from JSON */
		virtual GPUTexture * LoadObject(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path, GenTextureParameters const & parameters = GenTextureParameters()) const;
		/** texture loading from path */
		virtual GPUTexture * LoadObject(FilePathParam const & path, char const * name = nullptr, GenTextureParameters const & parameters = GenTextureParameters()) const;

	protected:

		/** search whether the path is already in used in the manager */
		virtual bool IsPathAlreadyUsedInManager(FilePathParam const & path) const override;
		/** search whether the name is already in used in the manager */
		virtual bool IsNameAlreadyUsedInManager(char const * in_name) const override;
	};

}; // namespace chaos
