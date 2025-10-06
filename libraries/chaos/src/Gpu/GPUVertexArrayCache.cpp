#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"


namespace chaos
{
	bool GPUVertexArrayCacheEntry::IsValid() const
	{
		if (vertex_array == nullptr)
			return false;
		return vertex_array->IsValid();
	}

	bool GPUVertexArrayCacheEntry::IsUsingBuffer(GLuint in_buffer_id) const
	{
		if (vertex_buffer_id == in_buffer_id)
			return true;
		if (index_buffer_id == in_buffer_id)
			return true;
		return false;
	}

	bool GPUVertexArrayCacheEntry::IsUsingProgram(GLuint in_program_id) const
	{
		if (program_id == in_program_id)
			return true;
		return false;
	}

	GPUVertexArrayCache::GPUVertexArrayCache(GPURenderContext * in_gpu_render_context):
		GPURenderContextResourceInterface(in_gpu_render_context)
	{
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
		GLuint searched_vertex_buffer_id = (binding_info.vertex_buffer != nullptr)? binding_info.vertex_buffer->GetResourceID() : 0;
		GLuint searched_index_buffer_id  = (binding_info.index_buffer  != nullptr)? binding_info.index_buffer->GetResourceID()  : 0;
		GLuint searched_program_id       = (binding_info.program       != nullptr)? binding_info.program->GetResourceID()  : 0;

		for (GPUVertexArrayCacheEntry const & entry : entries)
		{
			if (entry.program_id              == searched_program_id &&
				entry.vertex_buffer_id        == searched_vertex_buffer_id &&
				entry.index_buffer_id         == searched_index_buffer_id  &&
				entry.vertex_declaration_hash == binding_info.vertex_declaration->GetHash() &&
				entry.vertex_buffer_offset    == binding_info.vertex_buffer_offset)
			{
				return entry.vertex_array.get();
			}
		}
		return nullptr;
	}

	GPUVertexArray const* GPUVertexArrayCache::CreateVertexArray(GPUVertexArrayBindingInfo const& binding_info)
	{
		//assert(IsRenderContextCurrent());

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
				binding_info.vertex_buffer_offset,
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
		new_entry.vertex_array            = result;
		new_entry.program_id              = (binding_info.program != nullptr)? binding_info.program->GetResourceID() : 0;
		new_entry.vertex_buffer_id        = (binding_info.vertex_buffer != nullptr)? binding_info.vertex_buffer->GetResourceID() : 0;
		new_entry.index_buffer_id         = (binding_info.index_buffer != nullptr)? binding_info.index_buffer->GetResourceID() : 0;
		new_entry.vertex_buffer_offset    = binding_info.vertex_buffer_offset;
		new_entry.vertex_declaration_hash = binding_info.vertex_declaration->GetHash();
		entries.push_back(std::move(new_entry));

		return result;
	}

	void GPUVertexArrayCache::ReleaseVertexArray(GPUVertexArray * vertex_array)
	{
		//assert(IsRenderContextCurrent());

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

	void GPUVertexArrayCache::OnBufferDestroyed(GLuint in_buffer_id)
	{
		for (size_t i = entries.size() ; i > 0 ; --i)
		{
			GPUVertexArrayCacheEntry & entry = entries[i - 1];
			if (entry.IsUsingBuffer(in_buffer_id))
			{
				ReleaseVertexArray(entry.vertex_array.get());
				entry = entries[entries.size() - 1];
				entries.pop_back();
			}
		}
	}

	void GPUVertexArrayCache::OnProgramDestroyed(GLuint in_program_id)
	{
		for (size_t i = entries.size() ; i > 0 ; --i)
		{
			GPUVertexArrayCacheEntry & entry = entries[i - 1];
			if (entry.IsUsingProgram(in_program_id))
			{
				ReleaseVertexArray(entry.vertex_array.get());
				entry = entries[entries.size() - 1];
				entries.pop_back();
			}
		}
	}

}; // namespace chaos
