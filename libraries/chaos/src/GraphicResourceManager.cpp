#include <chaos/GraphicResourceManager.h>


namespace chaos
{

  GraphicResourceManager::~GraphicResourceManager()
  {
    Release();
  }

  void GraphicResourceManager::Release()
  {
    for (auto obj : textures)
      if (obj != nullptr)
        obj->SubReference();
    for (auto obj : vertex_arrays)
      if (obj != nullptr)
        obj->SubReference();
    for (auto obj : vertex_buffers)
      if (obj != nullptr)
        obj->SubReference();
    for (auto obj : index_buffers)
      if (obj != nullptr)
        obj->SubReference();

    /*
    for (auto obj : skeleton_defs)
      if (obj != nullptr)
        obj->SubReference();
    */
    textures.clear();
    vertex_arrays.clear();
    vertex_buffers.clear();
    index_buffers.clear();
    skeleton_defs.clear();
  }
}; // namespace chaos
