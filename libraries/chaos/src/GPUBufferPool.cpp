#include <chaos/GPUBufferPool.h>

namespace chaos
{

	GPUBuffer * GPUBufferPool::AllocateBuffer(Renderer * renderer, size_t min_buffer_size, size_t buffer_size)
	{
		assert(renderer != nullptr);

		// try to find an available buffer 
		GPUBuffer * result = FindUnusedBuffer(renderer, min_buffer_size, buffer_size);
		if (result != nullptr)
			return result;
	

		return result;
	}

	GPUBuffer * GPUBufferPool::FindUnusedBuffer(Renderer * renderer, size_t min_buffer_size, size_t buffer_size)
	{
		assert(renderer != nullptr);

		GPUBuffer * result = nullptr;
	


		return result;
	}

}; // namespace chaos
