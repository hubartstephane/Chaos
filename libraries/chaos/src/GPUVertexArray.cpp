#include <chaos/GPUVertexArray.h>


namespace chaos
{

	GPUVertexArray::GPUVertexArray(GLuint in_id) : 
		vertex_array_id(in_id)
	{

	}

	GPUVertexArray::~GPUVertexArray()
	{
		DoRelease();
	}

	bool GPUVertexArray::DoRelease()
	{
		if (vertex_array_id == 0)
			return false;
		glDeleteVertexArrays(1, &vertex_array_id);
		vertex_array_id = 0;
		return true;
	}
}; // namespace chaos
