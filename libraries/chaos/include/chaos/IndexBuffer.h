#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUBuffer.h>

namespace chaos
{
	class IndexBuffer : public GPUBuffer
	{
	public:

		/** constructor */
		IndexBuffer(GLuint in_id = 0);
	};

}; // namespace chaos
