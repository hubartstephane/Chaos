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
}; // namespace chaos
