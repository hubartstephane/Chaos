#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUBuffer.h>

namespace chaos
{
	class GPUIndexBuffer : public GPUBuffer
	{
	public:

		/** constructor */
		GPUIndexBuffer(GLuint in_id = 0);
	};

}; // namespace chaos
