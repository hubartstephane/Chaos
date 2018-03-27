#include <chaos/GPUResourceManager.h>


namespace chaos
{

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

  Texture * GPUResourceManager::FindTextureByPath(FilePathParam const & in_path)
  {
    return FindObjectByPath<Texture>(in_path, textures);
  }

  Texture const * GPUResourceManager::FindTextureByPath(FilePathParam const & in_path) const
  {
    return FindObjectByPath<Texture>(in_path, textures);
  }

  GPUProgram * GPUResourceManager::FindProgram(char const * name)
  {
    return FindObjectByName<GPUProgram>(name, programs);
  }
  GPUProgram const * GPUResourceManager::FindProgram(char const * name) const
  {
    return FindObjectByName<GPUProgram>(name, programs);
  }
  GPUProgram * GPUResourceManager::FindProgramByPath(FilePathParam const & in_path)
  {
    return FindObjectByPath<GPUProgram>(in_path, programs);
  }
  GPUProgram const * GPUResourceManager::FindProgramByPath(FilePathParam const & in_path) const
  {
    return FindObjectByPath<GPUProgram>(in_path, programs);
  }

  RenderMaterial * GPUResourceManager::FindRenderMaterial(char const * name)
  {
    return FindObjectByName<RenderMaterial>(name, render_materials);
  }

  RenderMaterial const * GPUResourceManager::FindRenderMaterial(char const * name) const
  {
    return FindObjectByName<RenderMaterial>(name, render_materials);
  }

  RenderMaterial * GPUResourceManager::FindRenderMaterialByPath(FilePathParam const & in_path)
  {
    return FindObjectByPath<RenderMaterial>(in_path, render_materials);
  }

  RenderMaterial const * GPUResourceManager::FindRenderMaterialByPath(FilePathParam const & in_path) const
  {
    return FindObjectByPath<RenderMaterial>(in_path, render_materials);
  }








  Texture * GPUResourceManager::LoadTexture(FilePathParam const & path, char const * name)
  {

#if 0
    if (name != nullptr)
    {
      Texture * texture = FindTexture(name);
      if (texture != nullptr)
      {
        if (path.GetResolvedPath() == texture->GetPath())
          return texture;

      }


    }
    


    if (FindTextureByPath(path))
      retu


#endif

    return nullptr;
  }

  GPUProgram * GPUResourceManager::LoadProgram(FilePathParam const & path, char const * name)
  {


    return nullptr;
  }

  RenderMaterial * GPUResourceManager::LoadRenderMaterial(FilePathParam const & path, char const * name)
  {

    return nullptr;
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

    return nullptr;
  }

  GPUProgram * GPUResourceManager::AddJSONProgram(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path)
  {

    return nullptr;
  }

  RenderMaterial * GPUResourceManager::AddJSONMaterial(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path)
  {

    return nullptr;
  }


}; // namespace chaos
