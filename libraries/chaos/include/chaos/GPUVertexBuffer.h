#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUBuffer.h>

namespace chaos
{
	class GPUVertexBuffer : public GPUBuffer
	{
	public:

		/** constructor */
		GPUVertexBuffer(GLuint in_id = 0);
	};

}; // namespace chaos
