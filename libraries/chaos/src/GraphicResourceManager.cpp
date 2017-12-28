#include <chaos/GraphicResourceManager.h>


namespace chaos
{

  GraphicResourceManager::~GraphicResourceManager()
  {
    Release();
  }

  void GraphicResourceManager::Release()
  {
    textures.clear();
	programs.clear();
	render_materials.clear();
  }

  Texture * GraphicResourceManager::LoadTexture(boost::filesystem::path path)
  {

	  return nullptr;
  }

  GLProgram * GraphicResourceManager::LoadProgram(boost::filesystem::path path)
  {
  
	  return nullptr;
  }

  RenderMaterial * GraphicResourceManager::LoadRenderMaterial(boost::filesystem::path path)
  {
  
	  return nullptr;
  }

}; // namespace chaos
