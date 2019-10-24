#include <stdio.h>
#include <stdlib.h>

#include <vector>

class GPUBuffer{};
class GPUFence{};
class GPURenderParams{};

template<typename T>
class shared_ptr{};









class BufferCacheEntries
{
	

	
public:

	/** the fence until which the buffers are in used */
	shared_ptr<GPUFence> fence;
	/** the buffers attached to the fence */
	std::vector<shared_ptr<GPUBuffer>> buffers;
};






class BufferCache
{
	
public:


	/** gets a buffer of at least given size. Look first at free buffers, or create a new one and register it according to the frame fence */
	bool GetBuffer(size_t required_size, GPURenderParams & render_params, shared_ptr<GPUBuffer> & result)
	{
		// the buffer is ordered from older FENCE to youngest FENCE
		size_t count = entries.size();
		for (size_t i = 0 ; i < count ; ++i)
		{
			BufferCacheEntries & entry = entries[i];
			if (entry.fence != nullptr)
			{
				// as soon as a FENCE exists and is not completed yet, there is no need to search further in the array
				// none of the other are completed	
				if (entry->fence->WaitForCompletion(0.0f))					
					entry->fence = nullptr;					
				else
					break;									
			}
			
		}			
		return CreateBuffer(required_size, render_params, result);					
	}
	
protected:

	bool CreateBuffer(size_t required_size, GPURenderParams & render_params, shared_ptr<GPUBuffer> & result)
	{
		BufferCacheEntries * entries = GetCacheEntryForFence(render_params.GetCurrentFrameFence());
		if (entries == nullptr)
			return nullptr;
		
		
	}
	
	/** get or create an entry for given fence */
	BufferCacheEntries * GetCacheEntryForFence(GPUFence * fence)
	{
		assert(fence != nullptr);

		// maybe an entry for this fence already exists : in that case it's necesseraly on top of the stack
		if (entries.num() > 0)
		{
			BufferCacheEntries & last_entry = &entries[entries.size() - 1];
			if (last_entry.fence == fence)
				return &last_entry;
		}
		// create a new entry for incomming fence
		BufferCacheEntries new_entry;
		new_entry.fence = fence;
		entries.push_back(std::move(new_entry));
		return &entries[entries.size() - 1];			
	}
	
protected:

	/** some GPUBuffers per GPUFence */
	std::vector<BufferCacheEntries> entries;
};


template<typename VERTEX_TYPE>
class VertexStream
{
	
	VertexStream & operator << (VERTEX_TYPE const & vertex)
	{
		
		
		
		return *this;
	}
	
	
public:

	GPURenderParams * render_params = nullptr;
};






template<typename PARTICLE_TYPE, typename VERTEX_TYPE>
class MyTrait
{
	using particle_type = PARTICLE_TYPE;
	using vertex_type   = VERTEX_TYPE;
	
	static int const PT_TRIANGLES      = 0;
	static int const PT_QUAD           = 1;
	static int const PT_TRIANGLE_STRIP = 2;
	static int const PT_TRIANGLE_FAN   = 3;
	
	
	/** returns true to destroy the particle */
	bool UpdateParticle(particle_type & p, float delta_time){ return false; } 
	
	
	void ParticleToVertices(particle_type const & p, AllocationData & allocation_data, VertexStream<VERTEX_TYPE> & stream){}
	
	
	/** even if particles do not change, the vertices are to be updated */
	static bool const dynamic_vertices = true;
	/** particles may change after their creation */
	static bool const dynamic_particles = true;
	/** the wanted primitive type */
	static int const primitive_type = PT_TRIANGLES;
	
};



int main(int argc, char **argv)
{
	return 0;
}