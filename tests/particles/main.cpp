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


	bool GetBuffer(size_t required_size, GPURenderParams & render_params, shared_ptr<GPUBuffer> & result);


protected:

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
	
	static int const RT_TRIANGLES      = 0;
	static int const RT_QUAD           = 1;
	static int const RT_TRIANGLE_STRIP = 2;
	static int const RT_TRIANGLE_FAN   = 3;
	
	
	
	
	void ParticleToVertices(particle_type const & p, ){}
	
	
	/** even if particles do not change, the vertices are to be updated */
	bool dynamic_vertices = true;
	/** particles may change after their creation */
	bool dynamic_particles = true;
	
};



int main(int argc, char **argv)
{
	return 0;
}