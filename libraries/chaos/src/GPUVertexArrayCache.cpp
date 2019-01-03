#include <chaos/GPUVertexArrayCache.h>
#include <chaos/GLTools.h>


namespace chaos
{
	bool VertexArrayCacheEntry::IsValid() const
	{
		// should always have a program 
		if (program.get() == nullptr)
			return false;
		// check for vertex buffer
		if (has_vertex_buffer && vertex_buffer.get() == nullptr)
			return false;
		// check for index buffer
		if (has_index_buffer && index_buffer.get() == nullptr)
			return false;
		// OK
		return true;
	}

	GPUVertexArray const * GPUVertexArrayCache::FindVertexArray(GPUProgram const * program) const
	{
		// early exit
		if (program == nullptr)
			return nullptr;

		// search matching entry
		GPUVertexArray const * result = nullptr;

		size_t count = entries.size();

		size_t i = 0;
		while ((i < count) && (result == nullptr))
		{
			// check whether entry is still valid (else swap/pop with last)
			VertexArrayCacheEntry & entry = entries[i];
			if (!entry.IsValid())				
			{
				if (i != count - 1)
					std::swap(entry, entries[count - 1]);
				entries.pop_back();
				--count;			
				continue;
			}
			// check whether this is expected entry
			if (entry.program.get() == program)
				result = entry.vertex_array.get();
			// next element
			++i;
		}	

		// complete the loop to purge invalid entries
		while (i < count)
		{
			// check whether entry is still valid (else swap/pop with last)
			VertexArrayCacheEntry & entry = entries[i];
			if (!entry.IsValid())				
			{
				if (i != count - 1)
					std::swap(entry, entries[count - 1]);
				entries.pop_back();
				--count;			
				continue;
			}
			// next element
			++i;
		}	

		return result;
	}

	GPUVertexArray const * GPUVertexArrayCache::FindOrCreateVertexArray(GPUProgram const * program, GPUVertexBuffer const * vertex_buffer, GPUIndexBuffer const * index_buffer, GPUVertexDeclaration const & declaration, GLintptr offset)
	{
		// early exit
		if (program == nullptr)
			return nullptr;

		// find exisiting data
		GPUVertexArray const * result = FindVertexArray(program);
		if (result != nullptr)
			return result;

		// create the vertex array
		shared_ptr<GPUVertexArray> new_vertex_array = new GPUVertexArray();
		if (new_vertex_array == nullptr || !new_vertex_array->IsValid())
			return nullptr;

		GLuint va = new_vertex_array->GetResourceID();

		// set the vertex buffer
		if (vertex_buffer != nullptr)  // simple mesh only use one vertex_buffer : binding_index is always 0
		{
			GLuint binding_index = 0;
			glVertexArrayVertexBuffer(va, binding_index, vertex_buffer->GetResourceID(), offset, declaration.GetVertexSize());
		}

		// set the index buffer
		if (index_buffer != nullptr)
			glVertexArrayElementBuffer(va, index_buffer->GetResourceID());

		// bind attributes
		GPUProgramData const & data = program->GetProgramData();
		data.BindAttributes(va, declaration, nullptr);

		// create the entry in the cache
		VertexArrayCacheEntry new_entry;
		new_entry.program           = program;
		new_entry.vertex_buffer     = vertex_buffer;
		new_entry.index_buffer      = index_buffer;
		new_entry.vertex_array      = new_vertex_array;
		new_entry.has_vertex_buffer = (vertex_buffer != nullptr);
		new_entry.has_index_buffer  = (index_buffer != nullptr);

		entries.push_back(std::move(new_entry));

		// result
		return new_vertex_array.get();
	}

	void GPUVertexArrayCache::Clear()
	{
		entries.clear();
	}

}; // namespace chaos
