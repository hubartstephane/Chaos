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

  Texture * GPUResourceManager::LoadTexture(FilePath const & path)
  {

	  return nullptr;
  }

  GPUProgram * GPUResourceManager::LoadProgram(FilePath const & path)
  {
  
	  return nullptr;
  }

  RenderMaterial * GPUResourceManager::LoadRenderMaterial(FilePath const & path)
  {
  
	  return nullptr;
  }

}; // namespace chaos
