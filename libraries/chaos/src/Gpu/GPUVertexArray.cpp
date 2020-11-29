#include <chaos/Chaos.h>


namespace chaos
{

	GPUVertexArray::GPUVertexArray()
	{
		CreateResource();	
	}

	GPUVertexArray::GPUVertexArray(GLuint in_id, bool in_ownership)
	{
		SetResource(in_id, in_ownership);
	}

	GPUVertexArray::~GPUVertexArray()
	{
		DoRelease();
	}

	bool GPUVertexArray::IsValid() const 
	{ 
		return (vertex_array_id != 0);
	}

	void GPUVertexArray::Invalidate()
	{
		vertex_array_id = 0;
	}

	bool GPUVertexArray::DoRelease()
	{
		if (vertex_array_id == 0)
			return false;
		if (ownership)
			glDeleteVertexArrays(1, &vertex_array_id);
		vertex_array_id = 0;
		return true;
	}

	bool GPUVertexArray::CreateResource()
	{
		// release previous resource
		Release();
		// create new resource
		glCreateVertexArrays(1, &vertex_array_id);
		if (vertex_array_id == 0)
			return false;
		ownership = true;
		return true;
	}

	bool GPUVertexArray::SetResource(GLuint in_id, bool in_ownership)
	{
		// early exit
		if (vertex_array_id == in_id)
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
			if (!glIsVertexArray(in_id)) 
				return false;
			// initialize internals
			vertex_array_id = in_id;
			ownership = in_ownership;
		}
		return true;
	}	

}; // namespace chaos
