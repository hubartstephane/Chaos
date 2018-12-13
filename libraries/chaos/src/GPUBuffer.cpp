#include <chaos/GPUBuffer.h>


namespace chaos
{

	GPUBuffer::GPUBuffer()
	{
		CreateResource();	
	}

	GPUBuffer::GPUBuffer(GLuint in_id, bool in_ownership)
	{
		SetResource(in_id, in_ownership);
	}

	GPUBuffer::~GPUBuffer()
	{
		DoRelease();
	}

	bool GPUBuffer::CreateResource()
	{
		// release previous resource
		Release();
		// create new resource
		glCreateBuffers(1, &buffer_id);
		if (buffer_id == 0)
			return false;
		ownership = true;
		return true;
	}

	bool GPUBuffer::SetResource(GLuint in_id, bool in_ownership)
	{
		// early exit
		if (buffer_id == in_id)
		{
			ownership = in_ownership;
			return true;
		}

		// release previous resource
		Release();

		// reference new resource (if exisiting)
		if (in_id != 0)
		{
			// bad incomming resource
			if (!glIsBuffer(in_id)) 
				return false;
			// get the resource size
			GLint size = 0;
			glGetNamedBufferParameteriv(buffer_id, GL_BUFFER_SIZE, &size);
			buffer_size = (size_t)size;
			// initialize internals
			buffer_id = in_id;
			ownership = in_ownership;
		}
		return true;
	}		

	bool GPUBuffer::IsValid() const 
	{ 
		return (buffer_id != 0);
	}

	bool GPUBuffer::DoRelease()
	{
		if (buffer_id == 0)
			return false;
		if (ownership)
			glDeleteBuffers(1, &buffer_id);
		buffer_id = 0;
		buffer_size = 0;
		return true;
	}

	bool GPUBuffer::SetBufferData(char const * buffer, size_t size)
	{

		return true;
	}

	size_t GPUBuffer::GetBufferSize() const
	{
		return buffer_size;
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
