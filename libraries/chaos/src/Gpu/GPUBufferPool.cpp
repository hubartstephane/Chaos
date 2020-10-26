#include <chaos/Chaos.h>

namespace chaos
{

	GPUBuffer * GPUBufferPool::AllocateBuffer(GPURenderer * renderer, size_t min_buffer_size, size_t buffer_size)
	{
		assert(renderer != nullptr);

		// try to find an available buffer 
		GPUBuffer * result = FindUnusedBuffer(renderer, min_buffer_size, buffer_size);
		if (result != nullptr)
			return result;
	
		// create a buffer



		return result;
	}

	GPUBuffer * GPUBufferPool::FindUnusedBuffer(GPURenderer * renderer, size_t min_buffer_size, size_t buffer_size)
	{
		assert(renderer != nullptr);

		GPUBuffer * result = nullptr;


		for (GPUBufferPoolEntry & entry : entries)
		{
			// still waiting for a fence ?
			if (entry.fence != nullptr && !entry.fence->WaitForCompletion(0))
				continue;
			entry.fence = nullptr; // the fence is completed : no need to keep it
			// test each buffers
			size_t count = entry.buffers.size();
			for (size_t i = 0 ; i < count ; ++i)
			{
				if (entry.buffers[i] == nullptr)
					continue;
				size_t buffer_size = entry.buffers[i]->GetBufferSize();

			
			}
		}
	


		return result;
	}

}; // namespace chaos
