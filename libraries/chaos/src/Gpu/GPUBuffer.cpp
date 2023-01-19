#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"


namespace chaos
{
	GPUBuffer::GPUBuffer(bool in_dynamic)
	{
		CreateResource(in_dynamic);
	}

	GPUBuffer::GPUBuffer(GLuint in_id, bool in_ownership)
	{
		SetResource(in_id, in_ownership);
	}

	GPUBuffer::~GPUBuffer()
	{
		assert(!mapped);
		Release();
	}

	bool GPUBuffer::CreateResource(bool in_dynamic)
	{
		// release previous resource
		Release();
		// create new resource
		glCreateBuffers(1, &buffer_id);
		if (buffer_id == 0)
			return false;
		ownership = true;
		dynamic = in_dynamic;
		buffer_size = 0;
		return true;
	}

	bool GPUBuffer::SetResource(GLuint in_id, bool in_ownership)
	{
		// early exit
		if (buffer_id == in_id)
		{
			ownership = in_ownership;
			return true;
		}

		// release previous resource
		Release();

		// reference new resource (if exisiting)
		if (in_id != 0)
		{
			// bad incomming resource
			if (!glIsBuffer(in_id)) 
				return false;
			// get the resource size
			GLint size = 0;
			glGetNamedBufferParameteriv(in_id, GL_BUFFER_SIZE, &size);
			buffer_size = (size_t)size;
			// get the usage : dynamic, static ...
			int usage = 0;
			glGetNamedBufferParameteriv(in_id, GL_BUFFER_USAGE, &usage);
			dynamic = (usage == GL_STREAM_DRAW || usage == GL_DYNAMIC_DRAW);
			// initialize internals
			buffer_id = in_id;
			ownership = in_ownership;
		}
		return true;
	}		

	bool GPUBuffer::IsValid() const 
	{ 
		return (buffer_id != 0);
	}

	void GPUBuffer::Release()
	{
		if (buffer_id != 0 && ownership)
			glDeleteBuffers(1, &buffer_id);
		buffer_id = 0;
		buffer_size = 0;
	}

	bool GPUBuffer::SetBufferData(char const * in_data, size_t in_size)
	{
		// early exit
		if (buffer_id == 0)
			return false;

		// the type of buffer we want (there are more kind of buffers we don't support : STREAM ... COPY/READ */
		GLenum buffer_type = (dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;  // GL_STREAM_DRAW ??? GL_DYNAMIC_DRAW ??

		// XXX: a mapping a STATIC_BUFFER triggers a performance warning (not a notification)
		//      static buffers should use glBufferData(...)
		//      test what happens with glBufferStorage(...)

        size_t effective_size = in_size;

		// just want to transfert some data
		if (in_data != nullptr)
		{
			bool transfered = false;

			// resize necessary
			if (effective_size != buffer_size)
			{
				// can resize + transfert at the same time
				if (effective_size == in_size)
				{
					glNamedBufferData(buffer_id, in_size, in_data, buffer_type); // can transfert data in the same time because data_size == allocation_size
					transfered = true;
				}
				// resize. cannot transfert at the same time
				else
				{
					glNamedBufferData(buffer_id, effective_size, nullptr, buffer_type); // reallocate and transfert NO data
				}
			}
			// transfert data if not done yet
			if (!transfered && in_size != 0)
				glNamedBufferSubData(buffer_id, 0, in_size, in_data);

			buffer_size = effective_size;
			return true;
		}
		// we want to resize buffer without any transfert
		else
		{
			// no need to do any thing, time is the same
			if (effective_size == buffer_size)
				return true;
			glNamedBufferData(buffer_id, effective_size, nullptr, buffer_type);
			buffer_size = effective_size;
		}
		return true;
	}

	size_t GPUBuffer::GetBufferSize() const
	{
		return buffer_size;
	}

	char * GPUBuffer::MapBuffer(size_t start, size_t count, bool read, bool write)
	{
		assert(!mapped);
		assert(read || write);

		// early exit
		if (buffer_id == 0)
			return nullptr;
		if (!read && !write)
			return nullptr;

		// search kind of mapping
		GLbitfield map_type = GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT; // theses flag are REALLY important !! can create significant framerate drops
		if (read)
			map_type |= GL_MAP_READ_BIT;
		if (write)
			map_type |= GL_MAP_WRITE_BIT;

       // map_type |= GL_MAP_UNSYNCHRONIZED_BIT;

		// check for map range
		if (start >= buffer_size)
			return nullptr;
		if (count == 0) // 0 = map all
		{
			count = buffer_size - start;
			if (count == 0)
				return nullptr; // nothing more to map
		}
		else if (start + count > buffer_size) // map all what required or nothing
			return nullptr;
		// do the mapping
		char* result = (char*)glMapNamedBufferRange(buffer_id, start, count, map_type);
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
