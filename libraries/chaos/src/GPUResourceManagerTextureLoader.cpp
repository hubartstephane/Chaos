#include <chaos/GPUResourceManagerTextureLoader.h>
#include <chaos/GPUResourceManager.h>
#include <chaos/TextureLoader.h>

namespace chaos
{

	Texture * GPUResourceManagerTextureLoader::GenTextureObject(nlohmann::json const & json, boost::filesystem::path const & config_path, GenTextureParameters const & parameters) const
	{
		if (!CheckResourceName(json))
			return nullptr;
		// load the texture
		Texture * texture = TextureLoader::GenTextureObject(json, config_path, parameters);
		if (texture != nullptr)
			FinalizeLoadedResource(texture);
		return texture;
	}

	Texture * GPUResourceManagerTextureLoader::GenTextureObject(FilePathParam const & path, GenTextureParameters const & parameters) const
	{
		if (!CheckResourcePath(path))
			return nullptr;
		// load the texture
		Texture * texture = TextureLoader::GenTextureObject(path, parameters);
		if (texture != nullptr)
			FinalizeLoadedResource(texture);
		return texture;
	}

	bool GPUResourceManagerTextureLoader::IsPathAlreadyUsedInManager(FilePathParam const & path) const
	{
		return (manager->FindTextureByPath(path) != nullptr);
	}

	bool GPUResourceManagerTextureLoader::IsNameAlreadyUsedInManager(std::string const & in_name) const
	{ 
		return (manager->FindTexture(in_name.c_str()) != nullptr);
	}

}; // namespace chaos
