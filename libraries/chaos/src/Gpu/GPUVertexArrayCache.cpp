#include <chaos/Chaos.h>


namespace chaos
{
	bool GPUVertexArrayCacheEntry::IsValid() const
	{
		// context window has been destroyed ?
		if (context_window == nullptr)
			return false;
		// should always have a program 
		if (program == nullptr)
			return false;
		else if (program_id != program->GetResourceID()) // the program OpenGL resource has changed. it is invalid
            return false;
		// check vertex buffer
		if (vertex_buffer == nullptr)
		{
			if (vertex_buffer_id != 0) // there was a vertex buffer. It has been deleted
				return false;
		}
		else if (vertex_buffer_id != vertex_buffer->GetResourceID()) // the vertex buffer OpenGL resource has changed. it is invalid
			return false;
		// check index buffer
		if (index_buffer == nullptr)
		{
			if (index_buffer_id != 0) // there was a vertex buffer. It has been deleted
				return false;
		}
		else if (index_buffer_id != index_buffer->GetResourceID()) // the index buffer OpenGL resource has changed. it is invalid
			return false;
		// OK
		return true;
	}

	GPUVertexArray const * GPUVertexArrayCache::FindVertexArray(GPURenderer* renderer, GPUProgram const * program, GPUBuffer const * vertex_buffer, GPUBuffer const * index_buffer) const
	{
		// early exit
		if (program == nullptr)
			return nullptr;

		// whether to purge during this pas or not
		bool purge = false;

		double t = glfwGetTime();		
		if (t - last_purge_time > 10.0)
		{
			last_purge_time = t;
			purge = true;
		}

		// search matching entry
		GPUVertexArray const * result = nullptr;

		size_t count = entries.size();

		size_t i = 0;
		while (i < count)
		{
			// check whether entry is still valid (else swap/pop with last)
			GPUVertexArrayCacheEntry & entry = entries[i];
			if (!entry.IsValid())				
			{
				if (i != count - 1)
					std::swap(entry, entries[count - 1]);
				entries.pop_back();
				--count;
			}			
			else
			{				
				// check whether this is expected entry
				if (result == nullptr && entry.program == program && entry.vertex_buffer == vertex_buffer && entry.index_buffer == index_buffer && entry.context_window == renderer->GetWindow())
				{
					result = entry.vertex_array.get();
					if (!purge && result != nullptr)
						return result;
				}
				// next element
				++i;
			}
		}	
		return result;
	}

	GPUVertexArray const * GPUVertexArrayCache::FindOrCreateVertexArray(GPURenderer * renderer, GPUProgram const * program, GPUBuffer const * vertex_buffer, GPUBuffer const * index_buffer, GPUVertexDeclaration const * declaration, GLintptr offset)
	{
		assert(renderer != nullptr);
        assert(declaration != nullptr);

		// early exit
		if (program == nullptr || program->GetResourceID() == 0)
			return nullptr;
		if (vertex_buffer != nullptr && vertex_buffer->GetResourceID() == 0) // vertex buffer provided but empty => error
			return nullptr;
		if (index_buffer != nullptr && index_buffer->GetResourceID() == 0)   // index  buffer provided but empty => error
			return nullptr;

		// find exisiting data
		GPUVertexArray const * result = FindVertexArray(renderer, program, vertex_buffer, index_buffer);
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
			glVertexArrayVertexBuffer(va, binding_index, vertex_buffer->GetResourceID(), offset, declaration->GetVertexSize());
		}

		// set the index buffer
		if (index_buffer != nullptr)
			glVertexArrayElementBuffer(va, index_buffer->GetResourceID());

		// bind attributes
		GPUProgramData const & data = program->GetProgramData();
		data.BindAttributes(va, *declaration, nullptr);

		// create the entry in the cache
		GPUVertexArrayCacheEntry new_entry;
		new_entry.program           = program;
		new_entry.vertex_buffer     = vertex_buffer;
		new_entry.index_buffer      = index_buffer;
		new_entry.vertex_array      = new_vertex_array;
		new_entry.program_id        = program->GetResourceID();
		new_entry.vertex_buffer_id  = (vertex_buffer != nullptr)? vertex_buffer->GetResourceID() : 0;
		new_entry.index_buffer_id   = (index_buffer != nullptr) ? index_buffer->GetResourceID() : 0;
		new_entry.context_window    = renderer->GetWindow();

		entries.push_back(std::move(new_entry));

		// result
		return new_vertex_array.get();
	}

	void GPUVertexArrayCache::Clear()
	{
		entries.clear();
	}

}; // namespace chaos
