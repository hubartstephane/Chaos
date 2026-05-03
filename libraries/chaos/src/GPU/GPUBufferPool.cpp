#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	GPUBufferPool::GPUBufferPool(GPUDevice * in_gpu_device):
		GPUDeviceResourceInterface(in_gpu_device)
	{
	}

	GPUBufferPool::~GPUBufferPool()
	{
		assert(buffer_info_groups.size() == 0); // ensure pool has properly been cleared before
	}

	bool GPUBufferPool::DoTick(float delta_time)
	{
		return true;
	}

	GPUBuffer * GPUBufferPool::CreateBuffer(size_t in_buffer_size, GPUBufferFlags in_flags)
	{
		if (GPUBuffer * result = DoFindCachedBuffer(in_buffer_size, in_flags))
			return result;
		return DoCreateBuffer(in_buffer_size, in_flags);
	}

	GPUBuffer * GPUBufferPool::DoFindCachedBuffer(size_t in_buffer_size, GPUBufferFlags in_flags)
	{
		for (auto entry_it = buffer_info_groups.begin() ; entry_it != buffer_info_groups.end() ; ++entry_it)
		{
			GPUBufferPoolBufferInfoGroup & buffer_info_group = *entry_it;

			for (auto buffer_info_it = buffer_info_group.buffers_info.begin() ; buffer_info_it != buffer_info_group.buffers_info.end() ; ++buffer_info_it)
			{
				GPUBufferPoolBufferInfo & buffer_info = *buffer_info_it;

				if (DoMatchRequestedBufferInfo(buffer_info, in_buffer_size, in_flags))
				{
					if (GPUBuffer * result = new GPUBuffer(GetGPUDevice(), buffer_info.buffer_id, buffer_info.buffer_size, buffer_info.buffer_flags))
					{
						buffer_info_group.buffers_info.erase(buffer_info_it);
						if (buffer_info_group.buffers_info.size() == 0)
							buffer_info_groups.erase(entry_it);
						return result;
					}
				}
			}
		}
		return nullptr;
	}

	bool GPUBufferPool::DoMatchRequestedBufferInfo(GPUBufferPoolBufferInfo & in_buffer_info, size_t in_buffer_size, GPUBufferFlags in_flags) const
	{
		if (in_buffer_info.buffer_flags != in_flags)
			return false;
		if (in_buffer_info.buffer_size < in_buffer_size)
			return false;
		if (rejected_size_percentage > 0)
		{
			size_t max_accepted_size = ((100 + rejected_size_percentage) * in_buffer_size) / 100;
			if (in_buffer_info.buffer_size >= max_accepted_size)
				return false;
		}
		return true;
	}

	GPUBuffer * GPUBufferPool::DoCreateBuffer(size_t in_buffer_size, GPUBufferFlags in_flags)
	{
		// create the GL resource
		GLuint buffer_id = 0;
		glCreateBuffers(1, &buffer_id);
		if (buffer_id == 0)
			return nullptr;
		// create the object
		GPUBuffer * result = new GPUBuffer(GetGPUDevice(), buffer_id, in_buffer_size, in_flags);
		if (result == nullptr)
		{
			glDeleteBuffers(1, &buffer_id);
			return nullptr;
		}
		// prepare the buffer
		GLenum buffer_type = (HasAnyFlags(in_flags, GPUBufferFlags::Dynamic)) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW; // there are more kind of buffers we don't support : STREAM ... COPY/READ
		glNamedBufferData(buffer_id, in_buffer_size, nullptr, buffer_type);

		return result;
	}

	GPUBufferPoolBufferInfoGroup * GPUBufferPool::FindOrCreateBufferInfoGroup(GPUFence * in_fence)
	{
		// search if there is an entry matching wanted fence
		for (GPUBufferPoolBufferInfoGroup & buffer_info_group : buffer_info_groups)
			if (buffer_info_group.fence == in_fence)
				return &buffer_info_group;
		// create a new entry for this fence
		GPUBufferPoolBufferInfoGroup buffer_info_group;
		buffer_info_group.fence = in_fence;
		buffer_info_groups.push_back(buffer_info_group);
		return &buffer_info_groups[buffer_info_groups.size() - 1];
	}

	void GPUBufferPool::OnBufferUnused(GPUBuffer * in_buffer)
	{
		if (WantToReuseBuffer(in_buffer))
		{
			ReleaseBuffer(in_buffer->buffer_id);
		}
		else if (GPUBufferPoolBufferInfoGroup * buffer_info_group = FindOrCreateBufferInfoGroup(nullptr))
		{
			GPUBufferPoolBufferInfo buffer_info;
			buffer_info.buffer_id    = in_buffer->buffer_id;
			buffer_info.buffer_size  = in_buffer->buffer_size;
			buffer_info.buffer_flags = in_buffer->flags;
			buffer_info_group->buffers_info.push_back(buffer_info);

			in_buffer->buffer_id = 0; // capture the underlying resource
		}
	}

	bool GPUBufferPool::WantToReuseBuffer(GPUBuffer * in_buffer) const
	{
		return true;
	}

	void GPUBufferPool::ClearPool()
	{
		for (GPUBufferPoolBufferInfoGroup & buffer_info_group : buffer_info_groups)
			for (GPUBufferPoolBufferInfo & buffer_info : buffer_info_group.buffers_info)
				ReleaseBuffer(buffer_info.buffer_id);
		buffer_info_groups.clear();
	}

	void GPUBufferPool::ReleaseBuffer(GLuint & in_buffer_id) const
	{
		glDeleteBuffers(1, &in_buffer_id);
		OnBufferDestroyed(in_buffer_id);
		in_buffer_id = 0;
	}

	void GPUBufferPool::OnBufferDestroyed(GLuint in_buffer_id) const
	{
		gpu_device->OnBufferDestroyed(in_buffer_id);
	}

}; // namespace chaos

