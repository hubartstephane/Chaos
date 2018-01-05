#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUBuffer.h>

namespace chaos
{
	class VertexBuffer : public GPUBuffer
	{
	public:

		/** constructor */
		VertexBuffer(GLuint in_id = 0);
	};

}; // namespace chaos
