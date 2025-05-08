#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool GPUVertexAndIndexMappedBuffers::IsValid() const
	{
		return (vertex_buffer.get() != nullptr) || (index_buffer.get() != nullptr); // if a single of theses checks is ok, the whole process of creating buffer and mapping has succeed
	}

	void GPUVertexAndIndexMappedBuffers::CleanResources()
	{
		if (vertex_buffer != nullptr)
		{
			vertex_buffer->UnMapBuffer();
			vertex_buffer = nullptr;
			mapped_vertex_buffer = nullptr;
		}
		if (index_buffer != nullptr)
		{
			index_buffer->UnMapBuffer();
			index_buffer = nullptr;
			mapped_index_buffer = nullptr;
		}
	}

	GPUVertexAndIndexMappedBuffers GPUVertexAndIndexMappedBuffers::CreateMappedBuffers(GPUDevice* in_gpu_device, size_t vertex_buffer_size, size_t index_buffer_size)
	{
		GPUVertexAndIndexMappedBuffers result;

		// create a vertex buffer
		if (vertex_buffer_size > 0)
		{
			result.vertex_buffer = new GPUBuffer(false);
			if (result.vertex_buffer == nullptr || !result.vertex_buffer->IsValid())
				return {};
			result.vertex_buffer->SetBufferData(nullptr, vertex_buffer_size);
		}

		// create a index buffer
		if (index_buffer_size > 0)
		{
			result.index_buffer = new GPUBuffer(false);
			if (result.index_buffer == nullptr || !result.index_buffer->IsValid())
				return {};
			result.index_buffer->SetBufferData(nullptr, index_buffer_size);
		}

		// map vertex buffer
		if (result.vertex_buffer != nullptr)
		{
			result.mapped_vertex_buffer = result.vertex_buffer->MapBuffer(0, 0, GPUBufferMapFlags::Write);
			if (result.mapped_vertex_buffer == nullptr)
				return {};
		}

		// map index buffer
		if (result.index_buffer != nullptr)
		{
			result.mapped_index_buffer = result.index_buffer->MapBuffer(0, 0, GPUBufferMapFlags::Write);
			if (result.mapped_index_buffer == nullptr)
			{
				if (result.vertex_buffer != nullptr)
					result.vertex_buffer->UnMapBuffer(); // must unmap vertex_buffer before returning default object
				return {};
			}
		}

		return result;
	}

}; // namespace chaos
