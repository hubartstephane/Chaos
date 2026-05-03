#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"


namespace chaos
{
	GPUVertexArray::GPUVertexArray(GLuint in_id):
		vertex_array_id(in_id)
	{
	}

	GPUVertexArray::~GPUVertexArray()
	{
		assert(vertex_array_id == 0);
	}

	bool GPUVertexArray::IsValid() const
	{
		return (vertex_array_id != 0);
	}

}; // namespace chaos
