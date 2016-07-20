#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GraphicResource.h>
#include <chaos/Texture.h>
#include <chaos/VertexArray.h>
#include <chaos/VertexBuffer.h>
#include <chaos/IndexBuffer.h>
#include <chaos/SkeletonHierarchyDef.h>

namespace chaos
{
  class GraphicResourceManager : public GraphicResource
  {
  public:

    /** destructor */
    virtual ~GraphicResourceManager();
    /** cleaning the resources */
    virtual void Release();

  public:

    /** the textures */
    std::vector<Texture *> textures;
    /** the vertex arrays contained by the manager */
    std::vector<VertexArray *> vertex_arrays;
    /** the vertex buffers contained by the manager */
    std::vector<VertexBuffer *> vertex_buffers;
    /** the index buffers contained by the manager */
    std::vector<IndexBuffer *> index_buffers;   

    /** the skeleton contained by the manager */
    std::vector<SkeletonHierarchyDef *> skeleton_defs;   
  };


}; // namespace chaos
