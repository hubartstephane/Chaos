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

  Texture * GPUResourceManager::LoadTexture(FilePathParam const & path, char const * name)
  {




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
