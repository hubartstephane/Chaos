#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUBuffer.h>

namespace chaos
{
	class GPUVertexBuffer : public GPUBuffer
	{
	public:

		/** constructor (create its own resource) */
		GPUVertexBuffer();
		/** constructor (reference a given resource). Call this function with 0 if you do not want to create resource at all */
		GPUVertexBuffer(GLuint in_id, bool in_ownership = true);
	};

}; // namespace chaos
