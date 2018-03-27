#include <chaos/GPUResourceManager.h>
#include <chaos/BoostTools.h>

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
    if (!CanAddTexture(name))
      return nullptr;

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

  GPUProgram * GPUResourceManager::LoadProgram(FilePathParam const & path)
  {
    boost::filesystem::path const resolved_path = path.GetResolvedPath();
    return LoadProgram(path, BoostTools::PathToName(resolved_path).c_str());
  }

  GPUProgram * GPUResourceManager::LoadProgram(FilePathParam const & path, char const * name)
  {
    if (!CanAddProgram(name))
      return nullptr;





    return nullptr;
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
