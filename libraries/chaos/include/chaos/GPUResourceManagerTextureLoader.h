#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ManagerResourceLoader.h>
#include <chaos/GPUResourceManager.h>
#include <chaos/Texture.h>
#include <chaos/TextureLoader.h>
#include <chaos/JSONTools.h>
#include <chaos/FilePath.h>

namespace chaos
{

	class GPUResourceManagerTextureLoader : public ManagerResourceLoader<TextureLoader, GPUResourceManager>
	{
	public:

		/** constructor */
		GPUResourceManagerTextureLoader(GPUResourceManager * in_resource_manager) :
			ManagerResourceLoader<TextureLoader, GPUResourceManager>(in_resource_manager)
		{

		}

		/** texture loading from JSON */
		virtual Texture * GenTextureObject(nlohmann::json const & json, boost::filesystem::path const & config_path, GenTextureParameters const & parameters) const override;
		/** texture loading from path */
		virtual Texture * GenTextureObject(FilePathParam const & path, GenTextureParameters const & parameters = GenTextureParameters()) const override;

	protected:

		/** search whether the path is already in used in the manager */
		virtual bool IsPathAlreadyUsedInManager(FilePathParam const & path) const override;
		/** search whether the name is already in used in the manager */
		virtual bool IsNameAlreadyUsedInManager(std::string const & in_name) const override;
	};

}; // namespace chaos
