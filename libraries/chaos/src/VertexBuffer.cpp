#include <chaos/VertexBuffer.h>


namespace chaos
{

  VertexBuffer::VertexBuffer(GLuint in_id) : vertex_buffer_id(in_id)
  {
  
  }

  VertexBuffer::~VertexBuffer()
  {
    Release();
  }

  void VertexBuffer::Release()
  {
    if (vertex_buffer_id != 0)
    {
      glDeleteBuffers(1, &vertex_buffer_id);
      vertex_buffer_id = 0;    
    }  
  }
}; // namespace chaos
