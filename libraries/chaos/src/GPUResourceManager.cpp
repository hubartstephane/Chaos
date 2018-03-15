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

  Texture * GPUResourceManager::LoadTexture(FilePathParam const & path)
  {

    return nullptr;
  }

  GPUProgram * GPUResourceManager::LoadProgram(FilePathParam const & path)
  {

    return nullptr;
  }

  RenderMaterial * GPUResourceManager::LoadRenderMaterial(FilePathParam const & path)
  {

    return nullptr;
  }

}; // namespace chaos
