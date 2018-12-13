#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUBuffer.h>

namespace chaos
{
	class GPUIndexBuffer : public GPUBuffer
	{
	public:

		/** constructor (create its own resource) */
		GPUIndexBuffer();
		/** constructor (reference a given resource). Call this function with 0 if you do not want to create resource at all */
		GPUIndexBuffer(GLuint in_id, bool in_ownership);
	};

}; // namespace chaos
