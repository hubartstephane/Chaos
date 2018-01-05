#include <chaos/GPUBuffer.h>


namespace chaos
{
	GPUBuffer::GPUBuffer(GLuint in_id) : resource_id(in_id)
	{

	}

	GPUBuffer::~GPUBuffer()
	{
		Release();
	}

	void GPUBuffer::Release()
	{
		if (resource_id != 0)
		{
			glDeleteBuffers(1, &resource_id);
			resource_id = 0;    
		}  
	}

	bool GPUBuffer::SetBufferData(char const * buffer, size_t size)
	{
	
		return true;
	}

	GLint GPUBuffer::GetBufferSize() const
	{
		if (!IsValid())
			return 0;
		GLint result = 0;
		glGetNamedBufferParameteriv(resource_id, GL_BUFFER_SIZE, &result);
		return result;
	
	}

#if 0

	glNamedBufferData
		GL_STREAM_DRAW
		GL_STREAM_READ
		GL_STREAM_COPY
		
		GL_STATIC_DRAW
		GL_STATIC_READ
		GL_STATIC_COPY
		
		GL_DYNAMIC_DRAW
		GL_DYNAMIC_READ
		GL_DYNAMIC_COPY.



	GL_BUFFER_ACCESS
		GL_BUFFER_ACCESS_FLAGS
		GL_BUFFER_IMMUTABLE_STORAGE
		GL_BUFFER_MAPPED
		GL_BUFFER_MAP_LENGTH
		GL_BUFFER_MAP_OFFSET
		GL_BUFFER_SIZE
		GL_BUFFER_STORAGE_FLAGS
				GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT 
		GL_BUFFER_USAGE
			GL_BUFFER_USAGE
#define glGetNamedBufferParameteri64v GLEW_GET_FUN(__glewGetNamedBufferParameteri64v)
#define glGetNamedBufferParameteriv GLEW_GET_FUN(__glewGetNamedBufferParameteriv)
#define glGetNamedBufferPointerv GLEW_GET_FUN(__glewGetNamedBufferPointerv)
#define glGetNamedBufferSubData GLEW_GET_FUN(__glewGetNamedBufferSubData)
#endif


}; // namespace chaos
