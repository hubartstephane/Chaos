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

  Texture * GPUResourceManager::LoadTexture(boost::filesystem::path path)
  {

    return nullptr;
  }

  GPUProgram * GPUResourceManager::LoadProgram(boost::filesystem::path path)
  {

    return nullptr;
  }

  RenderMaterial * GPUResourceManager::LoadRenderMaterial(boost::filesystem::path path)
  {

    return nullptr;
  }

}; // namespace chaos
