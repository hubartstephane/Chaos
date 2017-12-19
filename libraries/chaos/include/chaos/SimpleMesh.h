#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/GLProgramData.h>
#include <chaos/GLProgram.h>
#include <chaos/GLProgramVariableProvider.h>
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

    /** render the primitive (base_instance is an offset applyed to gl_InstanceID) */
    void Render(int instance_count = 0, int base_instance = 0) const;

    /** offset the index or vertex position */
    void ShiftIndexAndVertexPosition(int vb_offset, int ib_offset);

  public:

    /** self descriptive */
		GLenum primitive_type = GL_TRIANGLES;
    /** is this an indexed rendering */
		bool   indexed = false;
    /** number of vertex or index to use */
		int    count = 0;
    /** beginning in vertex or index buffer (if indexed rendering or not is used) */
    int    start = 0;
    /** for indexed rendering, this is an offset applyed to each index */
    int    base_vertex_index = 0;
  };

  /**
  * SimpleMesh : how it says
  */

  class SimpleMesh : public ReferencedObject
  {

  protected:

    class VertexArrayBindingInfo
    {
    public:

      boost::intrusive_ptr<GLProgram> program;
      boost::intrusive_ptr<VertexArray> vertex_array;
    };

  public:

    /** constructor */
    SimpleMesh() = default;
    /** destructor */
    ~SimpleMesh();

    /** clear the mesh */
    void Clear();
    /** render the primitive (base_instance is an offset applyed to gl_InstanceID) */
    void Render(GLProgram * program, GLProgramVariableProvider const * uniform_provider = nullptr, int instance_count = 0, int base_instance = 0) const;
    /** should bind index buffer and vertex buffer, as musch as for the vertex declaration */
    void FinalizeBindings(GLintptr vertex_buffer_offset);
    /** offset the index or vertex position */
    void ShiftPrimitivesIndexAndVertexPosition(int vb_offset, int ib_offset);
    
  protected:

    /** find or create a vertex array for a given program */
    VertexArray const * GetVertexArrayForProgram(GLProgram * program) const;

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
    /** in the vertex buffer (that may be shared by other simpled mesh), the offset from the begining given to it */
    GLintptr vertex_buffer_offset;

  protected:

    /** the vertex binding depends on the program that is used. This is a map that make relation between program / vertex array */
    mutable std::vector<VertexArrayBindingInfo> vertex_array_bindings;

  };

}; // namespace chaos
