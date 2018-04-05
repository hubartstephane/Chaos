#include <chaos/GPUResourceManager.h>
#include <chaos/BoostTools.h>
#include <chaos/GLTextureTools.h>
#include <chaos/TextureLoader.h>
#include <chaos/GPUProgramLoader.h>
#include <chaos/GPUProgramGenerator.h>

namespace chaos
{

	class GPUResourceManagerTextureLoader : public TextureLoader
	{
	public:

		GPUResourceManagerTextureLoader(GPUResourceManager * in_resource_manager):
			resource_manager(in_resource_manager){}

    virtual Texture * GenTextureObject(FilePathParam const & path, GenTextureParameters const & parameters = GenTextureParameters()) const override
    {
      if (resource_manager->FindTextureByPath(path) != nullptr)
        return nullptr;
      Texture * result = TextureLoader::GenTextureObject(path, parameters);
      if (result != nullptr)
        SetResourcePath(result, path.GetResolvedPath());
      return result;
    }

	protected:

		GPUResourceManager * resource_manager = nullptr;
	};

	class GPUResourceManagerGPUProgramLoader : public GPUProgramLoader
	{
	public:

		GPUResourceManagerGPUProgramLoader(GPUResourceManager * in_resource_manager):
			resource_manager(in_resource_manager){}

    virtual GPUProgram * GenProgramObject(FilePathParam const & path, GPUProgramLoaderCacheOptions const & cache_options = GPUProgramLoaderCacheOptions()) const override
    {
      if (resource_manager->FindProgramByPath(path) != nullptr)
        return nullptr;
      GPUProgram * result = GPUProgramLoader::GenProgramObject(path, cache_options);
      if (result != nullptr)
        SetResourcePath(result, path.GetResolvedPath());
      return result;
    }

	protected:

		GPUResourceManager * resource_manager = nullptr;
	};






	GPUResourceManager::~GPUResourceManager()
	{
		Release();
	}

	void GPUResourceManager::Release()
	{
		textures.clear();
		programs.clear();
		render_materials.clear();
	}

	Texture * GPUResourceManager::FindTexture(char const * name)
	{
		return FindObjectByName<Texture>(name, textures);
	}

	Texture const * GPUResourceManager::FindTexture(char const * name) const
	{
		return FindObjectByName<Texture>(name, textures);
	}

	Texture * GPUResourceManager::FindTextureByPath(FilePathParam const & path)
	{
		return FindObjectByPath<Texture>(path, textures);
	}

	Texture const * GPUResourceManager::FindTextureByPath(FilePathParam const & path) const
	{
		return FindObjectByPath<Texture>(path, textures);
	}

	GPUProgram * GPUResourceManager::FindProgram(char const * name)
	{
		return FindObjectByName<GPUProgram>(name, programs);
	}
	GPUProgram const * GPUResourceManager::FindProgram(char const * name) const
	{
		return FindObjectByName<GPUProgram>(name, programs);
	}
	GPUProgram * GPUResourceManager::FindProgramByPath(FilePathParam const & path)
	{
		return FindObjectByPath<GPUProgram>(path, programs);
	}
	GPUProgram const * GPUResourceManager::FindProgramByPath(FilePathParam const & path) const
	{
		return FindObjectByPath<GPUProgram>(path, programs);
	}

	RenderMaterial * GPUResourceManager::FindRenderMaterial(char const * name)
	{
		return FindObjectByName<RenderMaterial>(name, render_materials);
	}

	RenderMaterial const * GPUResourceManager::FindRenderMaterial(char const * name) const
	{
		return FindObjectByName<RenderMaterial>(name, render_materials);
	}

	RenderMaterial * GPUResourceManager::FindRenderMaterialByPath(FilePathParam const & path)
	{
		return FindObjectByPath<RenderMaterial>(path, render_materials);
	}

	RenderMaterial const * GPUResourceManager::FindRenderMaterialByPath(FilePathParam const & path) const
	{
		return FindObjectByPath<RenderMaterial>(path, render_materials);
	}

	Texture * GPUResourceManager::LoadTexture(FilePathParam const & path)
	{
		boost::filesystem::path const resolved_path = path.GetResolvedPath();
		return LoadTexture(path, BoostTools::PathToName(resolved_path).c_str());
	}

	Texture * GPUResourceManager::LoadTexture(FilePathParam const & path, char const * name)
	{
		if (!CanAddTexture(name)) // ensure no name collision
			return nullptr;

		GPUResourceManagerTextureLoader loader(this);
		Texture * texture = loader.GenTextureObject(path);
		if (texture != nullptr)
		{
			// set the name and the path
			if (name != nullptr)
				SetResourceName(texture, name);		
			// keep a reference on the texture
			textures.push_back(texture);
		}
		return texture;
	}

	GPUProgram * GPUResourceManager::LoadProgram(FilePathParam const & path)
	{
		boost::filesystem::path const resolved_path = path.GetResolvedPath();
		return LoadProgram(path, BoostTools::PathToName(resolved_path).c_str());
	}

	GPUProgram * GPUResourceManager::LoadProgram(FilePathParam const & path, char const * name)
	{
		if (!CanAddProgram(name)) // ensure no name collision
			return nullptr;

		GPUResourceManagerGPUProgramLoader loader(this);
		GPUProgram * program = loader.GenProgramObject(path);
		if (program != nullptr)
		{
			// set the name and the path
			if (name != nullptr)
				SetResourceName(program, name);		
			// keep a reference on the program
			programs.push_back(program);
		}
		return program;
	}

	RenderMaterial * GPUResourceManager::LoadRenderMaterial(FilePathParam const & path)
	{
		boost::filesystem::path const resolved_path = path.GetResolvedPath();
		return LoadRenderMaterial(path, BoostTools::PathToName(resolved_path).c_str());
	}

	RenderMaterial * GPUResourceManager::LoadRenderMaterial(FilePathParam const & path, char const * name)
	{
		if (!CanAddRenderMaterial(name))
			return nullptr;




		return nullptr;
	}

	bool GPUResourceManager::CanAddTexture(char const * name) const
	{
		return CanAddObject(name, [this](char const * n) {return FindTexture(n); } );
	}

	bool GPUResourceManager::CanAddProgram(char const * name) const
	{
		return CanAddObject(name, [this](char const * n) {return FindProgram(n); } );
	}

	bool GPUResourceManager::CanAddRenderMaterial(char const * name) const
	{
		return CanAddObject(name, [this](char const * n) {return FindRenderMaterial(n); } );
	}

	bool GPUResourceManager::InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		if (!InitializeTexturesFromConfiguration(json, config_path))
			return false;
		if (!InitializeProgramsFromConfiguration(json, config_path))
			return false;
		if (!InitializeMaterialsFromConfiguration(json, config_path))
			return false;
		return true;
	}

	bool GPUResourceManager::InitializeTexturesFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		return InitializeObjectsFromConfiguration(
			"textures", 
			json, 
			config_path, 
			[this](char const * name, nlohmann::json const & obj_json, boost::filesystem::path const & path)
		{
			AddJSONTexture(name, obj_json, path);
		}
		);
		return true;
	}

	bool GPUResourceManager::InitializeProgramsFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		return InitializeObjectsFromConfiguration(
			"programs", 
			json, 
			config_path, 
			[this](char const * name, nlohmann::json const & obj_json, boost::filesystem::path const & path)
		{
			AddJSONProgram(name, obj_json, path);
		}
		);
		return true;
	}

	bool GPUResourceManager::InitializeMaterialsFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		return InitializeObjectsFromConfiguration(
			"materials", 
			json, 
			config_path, 
			[this](char const * name, nlohmann::json const & obj_json, boost::filesystem::path const & path)
		{
			AddJSONMaterial(name, obj_json, path);
		}
		);
		return true;
	}

	Texture * GPUResourceManager::AddJSONTexture(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
    // ensure no name collision
		if (!CanAddTexture(name)) 
			return nullptr;
    // XXX : well, while GenTextureObject( PATH ) is overriden, it seems that GenTextureObject( JSON ) is forbidden throw derived class !!
		GPUResourceManagerTextureLoader loader(this);    
		Texture * texture = ((TextureLoader *)&loader)->GenTextureObject(json, config_path);
		if (texture != nullptr)
		{			
			// set the name and the path
			if (name != nullptr)
				SetResourceName(texture, name);		
			// keep a reference on the texture
			textures.push_back(texture);
		}
		return texture;
	}

	GPUProgram * GPUResourceManager::AddJSONProgram(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
    // ensure no name collision
    if (!CanAddProgram(name)) 
      return nullptr;    
    // XXX : well, while GenProgramObject( PATH ) is overriden, it seems that GenProgramObject( JSON ) is forbidden throw derived class !!
    GPUResourceManagerGPUProgramLoader loader(this);
    GPUProgram * program = ((GPUProgramLoader *)&loader)->GenProgramObject(json, config_path);
    if (program != nullptr)
    {
      // set the name and the path
      if (name != nullptr)
        SetResourceName(program, name);
      // keep a reference on the texture
      programs.push_back(program);
    }
    return program;
	}

	RenderMaterial * GPUResourceManager::AddJSONMaterial(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
    // ensure no name collision
    if (!CanAddRenderMaterial(name))
      return nullptr;






		return nullptr;
	}


}; // namespace chaos
