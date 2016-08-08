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
    vertex_arrays.clear();
    vertex_buffers.clear();
    index_buffers.clear();
    //skeleton_defs.clear();
  }
}; // namespace chaos
