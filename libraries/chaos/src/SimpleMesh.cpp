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

  SimpleMesh::SimpleMesh() :
    vertex_array(nullptr),
    vertex_buffer(nullptr),
    index_buffer(nullptr)
  {
  }

  SimpleMesh::~SimpleMesh()
  {
    Clear();
  }

  void SimpleMesh::Clear()
  {
    if (vertex_array != nullptr)
    {
      vertex_array->SubReference();
      vertex_array = nullptr;    
    }

    if (vertex_buffer != nullptr)
    {
      vertex_buffer->SubReference();
      vertex_buffer = nullptr;    
    }

    if (index_buffer != nullptr)
    {
      index_buffer->SubReference();
      index_buffer = nullptr;    
    }
    
    declaration.Clear();
    primitives.clear();
  }

  void SimpleMesh::Render(GLProgramData const & data, GLProgramAttributeProvider const * provider, int instance_count, int base_instance) const
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

  void SimpleMesh::FinalizeBindings()
  {
    assert(vertex_array != nullptr);

    GLuint va = vertex_array->GetResourceID();
      
    if (index_buffer != nullptr)    
      glVertexArrayElementBuffer(va, index_buffer->GetResourceID());

    if (vertex_buffer != nullptr)  // simple mesh only use one vertex_buffer : binding_index is always 0
    {
      GLuint binding_index = 0;
      glVertexArrayVertexBuffer(va, binding_index, vertex_buffer->GetResourceID(), 0, declaration.GetVertexSize());
    }
  }

}; // namespace chaos
