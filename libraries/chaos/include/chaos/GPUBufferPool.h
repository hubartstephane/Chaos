#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Renderer.h>
#include <chaos/GPUBuffer.h>

namespace chaos
{
	class GPUBufferPool : public ReferencedObject
	{
	public:

		/** allocate a buffer and bind it to a fence */
		GPUBuffer * AllocateBuffer(Renderer * renderer, size_t min_buffer_size, size_t buffer_size);

	protected:

		GPUBuffer * FindUnusedBuffer(Renderer * renderer, size_t min_buffer_size, size_t buffer_size);
		
	};

}; // namespace chaos
