#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUClasses.h>

#include <chaos/GPURenderer.h>
#include <chaos/GPUBuffer.h>
#include <chaos/GPUFence.h>

namespace chaos
{
	class GPUBufferPoolEntry
	{
	public:

		/** the fence after which all buffers are available */
		weak_ptr<GPUFence> fence;
		/** the buffers */
		std::vector<weak_ptr<GPUBuffer>> buffers;

	};

	class GPUBufferPool : public ReferencedObject
	{
	public:

		/** allocate a buffer and bind it to a fence */
		GPUBuffer * AllocateBuffer(GPURenderer * renderer, size_t min_buffer_size, size_t buffer_size);

	protected:

		/** internal method to find an available buffer */
		GPUBuffer * FindUnusedBuffer(GPURenderer * renderer, size_t min_buffer_size, size_t buffer_size);
		

	protected:

		/** the buffers that are stored */
		std::vector<GPUBufferPoolEntry> entries;
	};

}; // namespace chaos

