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
    std::vector<boost::intrusive_ptr<Texture>> textures;
    /** the vertex arrays contained by the manager */
    std::vector<boost::intrusive_ptr<VertexArray>> vertex_arrays;
    /** the vertex buffers contained by the manager */
    std::vector<boost::intrusive_ptr<VertexBuffer>> vertex_buffers;
    /** the index buffers contained by the manager */
    std::vector<boost::intrusive_ptr<IndexBuffer>> index_buffers;

    /** the skeleton contained by the manager */
    //std::vector<boost::intrusive_ptr<SkeletonHierarchyDef>> skeleton_defs;
  };


}; // namespace chaos
