#include <chaos/VertexDeclaration.h>
#include <chaos/VertexArrayCache.h>

namespace chaos
{
	VertexArrayCacheEntry::~VertexArrayCacheEntry()
	{
		GPUProgramToVertexArrayCacheCallbacks * callbacks = program_destruction_callback;
		if (callbacks != nullptr)
		{
			program_destruction_callback = nullptr;
			// invalidate the destruction callback
			callbacks->cache_entry = nullptr; 
			// destroy the callback			
			if (program != nullptr)
				program->RemoveReleaseCallback(callbacks); 
		}	
	}

	VertexArray * VertexArrayCache::FindVertexArray(GPUProgram * program)
	{
		// early exit
		if (program == nullptr)
			return nullptr;

		// test whether the program id match
		size_t count = entries.size();
		for (size_t i = 0 ; i < count ; ++i)
		{
			VertexArrayCacheEntry * entry = entries[i].get();
			if (entry != nullptr && entry->program == program)
				return entry->vertex_array.get();		
		}	
		return nullptr;
	}

	void VertexArrayCache::RemoveEntry(VertexArrayCacheEntry * entry)
	{
		assert(entry != nullptr);
		size_t count = entries.size();
		for (size_t i = 0 ; i < count ; ++i)
		{
			if (entries[i] == entry)
			{
				if (i != count - 1)
					entries[i] = entries[count - 1];
				entries.pop_back();
				break;
			}		
		}	
	}

	bool GPUProgramToVertexArrayCacheCallbacks::OnResourceReleased(GPUResource const * object, bool destruction)
	{
		VertexArrayCacheEntry * entry = cache_entry;
		if (entry != nullptr)
		{
			cache_entry = nullptr;
			

			entry->program = nullptr;
			entry->program_destruction_callback = nullptr;
			if (entry->cache != nullptr)
				entry->cache->RemoveEntry(entry);
			
		}
		return true; // this destruction callback is to be removed
	}



}; // namespace chaos
