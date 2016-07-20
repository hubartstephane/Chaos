#include <chaos/IndexBuffer.h>


namespace chaos
{

  IndexBuffer::IndexBuffer(GLuint in_id) : index_buffer_id(in_id)
  {
  
  }

  IndexBuffer::~IndexBuffer()
  {
    Release();
  }

  void IndexBuffer::Release()
  {
    if (index_buffer_id != 0)
    {
      glDeleteBuffers(1, &index_buffer_id);
      index_buffer_id = 0;    
    }  
  }
}; // namespace chaos
