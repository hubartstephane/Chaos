#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"


namespace chaos
{
	bool GPUVertexArrayCacheEntry::IsValid() const
	{
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
			if (index_buffer_id != 0) // there was an index buffer. It has been deleted
				return false;
		}
		else if (index_buffer_id != index_buffer->GetResourceID()) // the index buffer OpenGL resource has changed. it is invalid
			return false;
		// OK
		return true;
	}

	// Due to OpenGL implementation, VertexArrays have to be deleted with the proper GLFW context set
	// That's why we don't rely on native GPUVertexArray destructor to destroy OpenGL resource
	// Instead we rely on an explicit Destroy() call
	// The destructor is only here to ensure everything has been done properly

	GPUVertexArrayCache::~GPUVertexArrayCache()
	{
		assert(entries.size() == 0);
	}

	bool GPUVertexArrayCache::DoTick(float delta_time)
	{
		purge_timer -= delta_time;
		if (purge_timer <= 0.0f)
		{
			purge_timer = delay_between_purge;
			PurgeCache();
		}
		return true;
	}

	void GPUVertexArrayCache::PurgeCache()
	{
		for (size_t i = entries.size(); i > 0; --i)
		{
			size_t index = i - 1;

			GPUVertexArrayCacheEntry& entry = entries[index];
			if (!entry.IsValid())
			{
				ReleaseVertexArray(entry.vertex_array.get());
				if (index != entries.size() - 1)
					std::swap(entry, entries[entries.size() - 1]);
				entries.pop_back();
			}
		}
	}

	GPUVertexArray const* GPUVertexArrayCache::FindOrCreateVertexArray(GPUVertexArrayBindingInfo const& binding_info)
	{
		// early exit
		if (binding_info.vertex_declaration == nullptr)
			return nullptr;
		if (binding_info.program == nullptr || !binding_info.program->IsValid())
			return nullptr;
		if (binding_info.vertex_buffer != nullptr && !binding_info.vertex_buffer->IsValid()) // vertex buffer provided but invalid
			return nullptr;
		if (binding_info.index_buffer != nullptr && !binding_info.index_buffer->IsValid())   // index  buffer provided but invalid
			return nullptr;
		// find existing vertex array
		if (GPUVertexArray const* result = FindVertexArray(binding_info))
			return result;
		// create new vertex array
		return CreateVertexArray(binding_info);
	}

	GPUVertexArray const* GPUVertexArrayCache::FindVertexArray(GPUVertexArrayBindingInfo const& binding_info) const
	{
		for (GPUVertexArrayCacheEntry const & entry : entries)
		{
			if (entry.program == binding_info.program &&
				entry.vertex_buffer == binding_info.vertex_buffer &&
				entry.index_buffer == binding_info.index_buffer &&
				entry.vertex_buffer_offset == binding_info.offset)
			{
				return entry.vertex_array.get();
			}
		}
		return nullptr;
	}

	GPUVertexArray const* GPUVertexArrayCache::CreateVertexArray(GPUVertexArrayBindingInfo const& binding_info)
	{
		// create new GPUVertexArray
		GLuint vertex_array_id = 0;
		glCreateVertexArrays(1, &vertex_array_id);
		if (vertex_array_id == 0)
			return nullptr;
		
		GPUVertexArray * result = new GPUVertexArray(vertex_array_id);
		if (result == nullptr)
		{
			glDeleteVertexArrays(1, &vertex_array_id);
			return nullptr;
		}

		// set the vertex buffer
		if (binding_info.vertex_buffer != nullptr)  // simple mesh only use one vertex_buffer : binding_index is always 0
		{
			GLuint binding_index = 0;
			glVertexArrayVertexBuffer(
				vertex_array_id,
				binding_index,
				binding_info.vertex_buffer->GetResourceID(),
				binding_info.offset,
				binding_info.vertex_declaration->GetVertexSize());
		}

		// set the index buffer
		if (binding_info.index_buffer != nullptr)
			glVertexArrayElementBuffer(vertex_array_id, binding_info.index_buffer->GetResourceID());

		// bind attributes
		GPUProgramData const& data = binding_info.program->GetProgramData();
		data.BindAttributes(vertex_array_id, *binding_info.vertex_declaration, nullptr);

		// create the entry in the cache
		GPUVertexArrayCacheEntry new_entry;
		new_entry.vertex_array         = result;
		new_entry.program              = binding_info.program;
		new_entry.vertex_buffer        = binding_info.vertex_buffer;
		new_entry.index_buffer         = binding_info.index_buffer;
		new_entry.program_id           = binding_info.program->GetResourceID();
		new_entry.vertex_buffer_id     = (binding_info.vertex_buffer != nullptr) ? binding_info.vertex_buffer->GetResourceID() : 0;
		new_entry.index_buffer_id      = (binding_info.index_buffer != nullptr) ? binding_info.index_buffer->GetResourceID() : 0;
		new_entry.vertex_buffer_offset = binding_info.offset;
		entries.push_back(std::move(new_entry));

		return result;
	}

	void GPUVertexArrayCache::ReleaseVertexArray(GPUVertexArray * vertex_array)
	{
		if (vertex_array->vertex_array_id != 0)
		{
			glDeleteVertexArrays(1, &vertex_array->vertex_array_id);
			vertex_array->vertex_array_id = 0;
		}
	}

	void GPUVertexArrayCache::Destroy()
	{
		for (GPUVertexArrayCacheEntry const& entry : entries)
			if (entry.vertex_array != nullptr)
				ReleaseVertexArray(entry.vertex_array.get());
		entries.clear();
	}

}; // namespace chaos
