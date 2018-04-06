#include <chaos/VertexArray.h>


namespace chaos
{

	VertexArray::VertexArray(GLuint in_id) : vertex_array_id(in_id)
	{

	}

	VertexArray::~VertexArray()
	{
		DoRelease();
	}

	void VertexArray::DoRelease()
	{
		if (vertex_array_id != 0)
		{
			glDeleteVertexArrays(1, &vertex_array_id);
			vertex_array_id = 0;    
		}  
	}
}; // namespace chaos
