#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(GPUBufferFlags, nullptr, CHAOS_API);
	CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(GPUBufferMapFlags, nullptr, CHAOS_API);

	GPUBuffer::GPUBuffer(GPUDevice * in_gpu_device, GLuint in_buffer_id, size_t in_buffer_size, GPUBufferFlags in_flags):
		GPUDeviceResourceInterface(in_gpu_device),
		buffer_id(in_buffer_id),
		buffer_size(in_buffer_size),
		flags(in_flags)
	{
	}

	GPUBuffer::~GPUBuffer()
	{
		assert(buffer_id == 0);
		assert(!mapped);
	}

	size_t GPUBuffer::GetBufferSize() const
	{
		return buffer_size;
	}

	GPUBufferFlags GPUBuffer::GetBufferFlags() const
	{
		return flags;
	}

	bool GPUBuffer::IsValid() const
	{
		return (buffer_id != 0);
	}

	void GPUBuffer::OnLastReferenceLost()
	{
		gpu_device->OnBufferUnused(this);
		GPUResource::OnLastReferenceLost();
	}

	bool GPUBuffer::CheckAndComputeBufferRange(size_t & inout_start, size_t & inout_size) const
	{
		if (inout_start >= buffer_size) // check for range
			return false;
		if (inout_size == 0) // 0 = map all
		{
			inout_size = buffer_size - inout_start;
			if (inout_size == 0)
				return false; // nothing more to map
		}
		else if (inout_start + inout_size > buffer_size) // map all that is required or nothing
			return false;

		return true;
	}

	bool GPUBuffer::SetBufferData(char const * in_data, size_t in_start, size_t in_size)
	{
		// early exit
		if (buffer_id == 0)
			return false;
		if (in_data == nullptr)
			return false;
		if (in_size == 0)
			return false;
		if (!CheckAndComputeBufferRange(in_start, in_size))
			return false;

		glNamedBufferSubData(buffer_id, in_start, in_size, in_data);

		return true;
	}

	char * GPUBuffer::MapBuffer(size_t in_start, size_t in_size, GPUBufferMapFlags in_flags)
	{
		assert(!mapped);
		assert(HasAnyFlags(in_flags, GPUBufferMapFlags::Read | GPUBufferMapFlags::Write));

		// early exit
		if (buffer_id == 0)
			return nullptr;
		if (!HasAnyFlags(in_flags, GPUBufferMapFlags::Read | GPUBufferMapFlags::Write))
			return nullptr;
		if (!CheckAndComputeBufferRange(in_start, in_size))
			return nullptr;

		// search kind of mapping
		GLbitfield map_type = GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT; // theses flags are REALLY important !! can create significant framerate drops
		if (HasAnyFlags(in_flags, GPUBufferMapFlags::Read))
			map_type |= GL_MAP_READ_BIT;
		if (HasAnyFlags(in_flags, GPUBufferMapFlags::Write))
			map_type |= GL_MAP_WRITE_BIT;

		// map_type |= GL_MAP_UNSYNCHRONIZED_BIT;

		// do the mapping
		char* result = (char*)glMapNamedBufferRange(buffer_id, in_start, in_size, map_type);
		if (result != nullptr)
			mapped = true;
		return result;
	}

	void GPUBuffer::UnMapBuffer()
	{
		assert(mapped);
		if (buffer_id == 0)
			return;
		mapped = false;
		glUnmapNamedBuffer(buffer_id);
	}

}; // namespace chaos
