#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/GLProgramData.h>
#include <chaos/GLProgramAttributeProvider.h>
#include <chaos/VertexArray.h>
#include <chaos/VertexBuffer.h>
#include <chaos/IndexBuffer.h>

namespace chaos
{

  /**
   * MeshPrimitive : a draw call
   */

  class MeshPrimitive
  {
  public:

    /** constructor */
    MeshPrimitive();

    /** render the primitive (base_instance is an offset applyed to gl_InstanceID) */
    void Render(int instance_count = 0, int base_instance = 0) const;

  public:

    /** self descriptive */
    GLenum primitive_type;
    /** is this an indexed rendering */
    bool   indexed;
    /** number of vertex or index to use */
    int    count;
    /** beginning in vertex or index buffer (if indexed rendering or not is used) */
    int    start;
    /** for indexed rendering, this is an offset applyed to each index */
    int    base_vertex_index;
  };

  /**
  * SimpleMesh : how it says
  */

  class SimpleMesh
  {

  public:

    /** constructor */
    SimpleMesh();
    /** destructor */
    ~SimpleMesh();

    /** clear the mesh */
    void Clear();
    /** render the primitive (base_instance is an offset applyed to gl_InstanceID) */
    void Render(GLProgramData const & data, GLProgramAttributeProvider const * provider = nullptr, int instance_count = 0, int base_instance = 0) const;
    /** should bind index buffer and vertex buffer, as musch as for the vertex declaration */
    void FinalizeBindings();

  public:

    /** self descriptive */
    VertexDeclaration declaration;
    /** the primitives to render */
    std::vector<MeshPrimitive> primitives;

    /** self descriptive */
    boost::intrusive_ptr<VertexArray> vertex_array;
    /** self descriptive */
    boost::intrusive_ptr<VertexBuffer> vertex_buffer;
    /** self descriptive */
    boost::intrusive_ptr<IndexBuffer> index_buffer;
  };

}; // namespace chaos
