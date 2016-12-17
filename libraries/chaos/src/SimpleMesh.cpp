#include <chaos/SimpleMesh.h>
#include <chaos/GLTools.h>

namespace chaos
{

  MeshPrimitive::MeshPrimitive() :
    primitive_type(GL_TRIANGLES),
    indexed(false),
    count(0),
    start(0),
    base_vertex_index(0)
  {
  }
  //
  // XXX : the vb_offset is not really usefull in mixed BUFFER (one buffer with 2 parts)
  //       
  //       x------------------x-------------------x
  //       | vertex size = 6  |  vertex_size = 18 |
  //       x------------------x-------------------x
  //
  //       because BASE_VERTEX_INDEX is a number of vertices 
  //        => the real offset depends of the size of ONE vertex
  //       
  //      to create a shift, use glVertexArrayVertexBuffer(...offset ...) that helps create a view of buffer
  //
  void MeshPrimitive::ShiftIndexAndVertexPosition(int vb_offset, int ib_offset)
  {
    if (!indexed)
      start += vb_offset; 
    else
    {
      start += ib_offset;
      base_vertex_index += vb_offset;
    }
  }

  void MeshPrimitive::Render(int instance_count, int base_instance) const
  {
    // This function is able to render : 
    //   -normal primitives
    //   -indexed primitives
    //   -instanced primitives
    //
    // We do not use yet :
    //   -indirect primitive
    //   -multi draws possibilities

    if (count <= 0)
      return;

    if (!indexed)
    {
      if (instance_count <= 1)
      {
        glDrawArrays(primitive_type, start, count);
      }
      else
      {
        if (base_instance == 0)
          glDrawArraysInstanced(primitive_type, start, count, instance_count);
        else
          glDrawArraysInstancedBaseInstance(primitive_type, start, count, instance_count, base_instance);
      }
    }
    else
    {
      GLvoid * offset = ((int32_t*)nullptr) + start;
      if (instance_count <= 1)
      {
        if (base_vertex_index == 0)
          glDrawElements(primitive_type, count, GL_UNSIGNED_INT, offset);
        else
          glDrawElementsBaseVertex(primitive_type, count, GL_UNSIGNED_INT, offset, base_vertex_index);
      }
      else
      {
        if (base_vertex_index == 0)
          glDrawElementsInstanced(primitive_type, count, GL_UNSIGNED_INT, offset, instance_count);
        else
          glDrawElementsInstancedBaseVertexBaseInstance(primitive_type, count, GL_UNSIGNED_INT, offset, instance_count, base_vertex_index, base_instance);
      }
    }
  }

  SimpleMesh::SimpleMesh()
  {
  }

  SimpleMesh::~SimpleMesh()
  {
    Clear();
  }

  void SimpleMesh::ShiftPrimitivesIndexAndVertexPosition(int vb_offset, int ib_offset)
  {
    if (vb_offset != 0 || ib_offset != 0)
      for (auto & primitive : primitives)
        primitive.ShiftIndexAndVertexPosition(vb_offset, ib_offset);
  }

  void SimpleMesh::Clear()
  {
    vertex_array  = nullptr;    
    vertex_buffer = nullptr;    
    index_buffer  = nullptr;    
    
    declaration.Clear();
    primitives.clear();
  }

  void SimpleMesh::Render(GLProgramData const & data, GLProgramVariableProvider const * provider, int instance_count, int base_instance) const
  {
    assert(vertex_array != nullptr);

    GLuint va = vertex_array->GetResourceID();

    // bind what needs to be
    glBindVertexArray(va);      
    data.BindAttributes(va, declaration, provider);

    // render the primitives
    for (auto const & primitive : primitives)
      primitive.Render(instance_count, base_instance);
  }

  void SimpleMesh::FinalizeBindings(GLintptr vertex_buffer_offset)
  {
    assert(vertex_array != nullptr);

    GLuint va = vertex_array->GetResourceID();
      
    if (index_buffer != nullptr)    
      glVertexArrayElementBuffer(va, index_buffer->GetResourceID());

    if (vertex_buffer != nullptr)  // simple mesh only use one vertex_buffer : binding_index is always 0
    {
      GLuint binding_index = 0;
      glVertexArrayVertexBuffer(va, binding_index, vertex_buffer->GetResourceID(), vertex_buffer_offset, declaration.GetVertexSize());
    }
  }

}; // namespace chaos
