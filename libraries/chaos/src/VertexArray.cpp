#include <chaos/VertexArray.h>


namespace chaos
{

	VertexArray::VertexArray(GLuint in_id) : 
		vertex_array_id(in_id)
	{

	}

	VertexArray::~VertexArray()
	{
		DoRelease();
	}

	bool VertexArray::DoRelease()
	{
		if (vertex_array_id == 0)
			return false;
		glDeleteVertexArrays(1, &vertex_array_id);
		vertex_array_id = 0;
		return true;
	}
}; // namespace chaos
