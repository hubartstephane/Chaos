#include <chaos/ParticleManager.h>
#include <chaos/GLTools.h>
#include <chaos/DrawPrimitive.h>

namespace chaos
{

	// ==============================================================
	// PARTICLE RANGE ALLOCATION
	// ==============================================================

	ParticleRangeAllocation::~ParticleRangeAllocation()
	{
		RemoveFromLayer();
	}

	void ParticleRangeAllocation::RemoveFromLayer()
	{
		if (layer == nullptr)
			return;
		layer->RemoveParticleAllocation(this);
	}

	ParticleRange ParticleRangeAllocation::GetParticleRange() const
	{
		if (layer == nullptr)
			return ParticleRange();
		return layer->particles_ranges[range_index];
	}

	size_t ParticleRangeAllocation::GetParticleCount() const
	{
		return GetParticleRange().count;
	}

	void * ParticleRangeAllocation::GetParticleBuffer()
	{
		if (layer == nullptr)
			return nullptr;
		return layer->GetParticleBuffer(layer->particles_ranges[range_index]);
	}

	void const * ParticleRangeAllocation::GetParticleBuffer() const
	{
		if (layer == nullptr)
			return nullptr;
		return layer->GetParticleBuffer(layer->particles_ranges[range_index]);
	}

	// ==============================================================
	// PARTICLE LAYER DESC
	// ==============================================================

	size_t ParticleLayerDesc::GetParticleSize() const
	{
		return 0;
	}

	size_t ParticleLayerDesc::GetVerticesCountPerParticles() const
	{
		return 2 * 3; // 2 triangles per particles to have a square
	}

	size_t ParticleLayerDesc::GetVertexSize() const
	{
		return 0;
	}

	void ParticleLayerDesc::UpdateParticles(float delta_time, void * particles, size_t particle_count, size_t * deletion_vector)
	{

	}

	size_t ParticleLayerDesc::DestroyOutdatedParticles(void * particles, size_t particle_count, size_t * deletion_vector)
	{
		return particle_count; // no particle destruction.
	}

	bool ParticleLayerDesc::HasParticleLimitedLifeTime() const
	{
		return false;
	}

	bool ParticleLayerDesc::AreParticlesDynamic() const
	{
		return false;
	}

	VertexDeclaration ParticleLayerDesc::GetVertexDeclaration() const
	{
		return VertexDeclaration();
	}
		
	void ParticleLayerDesc::ParticlesToVertices(char const * particles, size_t particles_buffer_size, char * vertices, size_t vertices_buffer_size) const
	{
		assert(particles != nullptr);
		assert(vertices != nullptr);
	}
	
	// ==============================================================
	// PARTICLE LAYER
	// ==============================================================

	ParticleLayer::ParticleLayer(ParticleLayerDesc * in_layer_desc)
		: layer_desc(in_layer_desc)
	{
		assert(in_layer_desc != nullptr);
		particle_size = layer_desc->GetParticleSize();
		vertex_size = layer_desc->GetVertexSize();
		vertices_count_per_particles = layer_desc->GetVerticesCountPerParticles();
	}

	ParticleLayer::~ParticleLayer()
	{
		DetachAllParticleAllocations();
	}

	void ParticleLayer::SetLayerName(char const * in_name)
	{
		if (in_name == nullptr)
			name.clear();
		else
			name = in_name;
	}

	void ParticleLayer::SetLayerID(int in_id)
	{
		id = in_id;
	}

	void ParticleLayer::DetachAllParticleAllocations()
	{
		while (range_allocations.size())
			RemoveParticleAllocation(range_allocations[range_allocations.size() - 1]);
	}

	size_t ParticleLayer::GetParticleCount() const
	{
		if (particle_size == 0)
			return 0;
		return particles.size() / particle_size;
	}

	size_t ParticleLayer::GetParticleCount(ParticleRange range) const
	{
		if (particle_size == 0)
			return 0;
		return range.count;
	}

	void * ParticleLayer::GetParticleBuffer(ParticleRange range)
	{
		if (particle_size == 0)
			return nullptr;
		return &particles[range.start * particle_size];
	}

	void const * ParticleLayer::GetParticleBuffer(ParticleRange range) const
	{
		if (particle_size == 0)
			return nullptr;
		return &particles[range.start * particle_size];
	}

	bool ParticleLayer::HasParticleLimitedLifeTime() const
	{
		return layer_desc->HasParticleLimitedLifeTime();
	}

	bool ParticleLayer::AreParticlesDynamic() const
	{
		return layer_desc->AreParticlesDynamic();
	}

	void ParticleLayer::Pause(bool in_paused)
	{
		paused = in_paused;
	}

	bool ParticleLayer::IsPaused() const
	{
		return paused;
	}

	void ParticleLayer::Show(bool in_visible)
	{
		visible = in_visible;
	}

	bool ParticleLayer::IsVisible() const
	{
		return visible;
	}

	void ParticleLayer::TickParticles(float delta_time)
	{
		// no particles, nothing to do
		size_t particle_count = GetParticleCount();
		if (particle_count == 0)
			return;
		// early exit
		if (IsPaused())
			return;
		// update the particles themselves
		if (AreParticlesDynamic())
		{
			UpdateParticles(delta_time);
			require_GPU_update = true;
		}
		// destroy the particles that are to be destroyed
		if (pending_kill_particles > 0 || HasParticleLimitedLifeTime())
		{
			size_t new_particle_count = DestroyOutdatedParticles();
			if (new_particle_count != particle_count)
			{
				UpdateParticleRanges(new_particle_count);
				require_GPU_update = true;
			}
			pending_kill_particles = 0;
		}
	}

	void ParticleLayer::UpdateParticles(float delta_time)
	{
		layer_desc->UpdateParticles(delta_time, &particles[0], GetParticleCount(), &deletion_vector[0]);
	}

	size_t ParticleLayer::DestroyOutdatedParticles()
	{
		return layer_desc->DestroyOutdatedParticles(&particles[0], GetParticleCount(), &deletion_vector[0]);
	}

	ParticleRangeAllocation * ParticleLayer::SpawnParticlesAndKeepRange(size_t count, bool particles_owner)
	{
		ParticleRange range = SpawnParticles(count);
		if (range.count == 0)
			return nullptr;

		ParticleRangeAllocation * result = new ParticleRangeAllocation;

		result->layer = this;
		result->range_index = particles_ranges.size();
		result->particles_owner = particles_owner;

		particles_ranges.push_back(range);
		range_allocations.push_back(result);

		return result;
	}

	ParticleRange ParticleLayer::SpawnParticles(size_t count)
	{
		ParticleRange result;
		if (count > 0 && particle_size > 0)
		{
			size_t current_particle_count = GetParticleCount();

			// initialize the result
			result.start = current_particle_count;
			result.count = count;
			// create the particles and the suppression corresponding data
			size_t new_count = current_particle_count + count;
			particles.resize(new_count * particle_size, 0);
			deletion_vector.resize(new_count, 0);
			// indicates that GPU needs to be rebuild
			require_GPU_update = true;
		}
		return result;
	}

	void ParticleLayer::MarkParticlesToDestroy(size_t start, size_t count)
	{
		// clamp the range
		size_t suppression_count = deletion_vector.size();
		if (start >= suppression_count)
			return;
		size_t end = min(start + count, suppression_count);
		// mark the particles to destroy    
		while (start != end)
			deletion_vector[start++] = DESTROY_PARTICLE_MARK;
		// count the number of particles to destroy
		pending_kill_particles += count;
	}

	void ParticleLayer::RemoveParticleAllocation(ParticleRangeAllocation * allocation)
	{
		assert(allocation != nullptr);

		// mark the range to be destroyed
		if (allocation->particles_owner)
		{
			ParticleRange range = allocation->GetParticleRange();
			MarkParticlesToDestroy(range.start, range.count);
		}
		// displace range and allocation
		size_t last_index = range_allocations.size() - 1;
		size_t range_index = allocation->range_index;
		if (allocation->range_index < last_index)
		{
			range_allocations[range_index] = range_allocations[last_index]; // replace the allocation
			particles_ranges[range_index] = particles_ranges[last_index];
			range_allocations[range_index]->range_index = range_index;
		}
		range_allocations.pop_back();
		particles_ranges.pop_back();

		// reset the object
		*allocation = ParticleRangeAllocation();
	}

	void ParticleLayer::UpdateParticleRanges(size_t new_particle_count)
	{
		// update the ranges (code is useless from one TICK to the next. the only important value is NUMERIC LIMIT)
		size_t range_count = particles_ranges.size();
		for (size_t i = 0; i < range_count; ++i)
		{
			ParticleRange & range = particles_ranges[i];
			if (range.count == 0)
				continue;

			size_t start = std::numeric_limits<size_t>::max();
			size_t end   = std::numeric_limits<size_t>::max();
			
			for (size_t i = 0 ; i < range.count ; ++i) // search the index of the extrem particles
			{
				if (!deletion_vector[i + range.start] != ParticleLayer::DESTROY_PARTICLE_MARK)
				{
					if (start == std::numeric_limits<size_t>::max())
						start = i;
					end = i;
				}
			}

			if (start == std::numeric_limits<size_t>::max())
				range.start = range.count = 0;
		}
		// resize some vectors
		particles.resize(new_particle_count * particle_size);
		deletion_vector.resize(new_particle_count);
		// reset the suppression vector
		for (size_t i = 0; i < new_particle_count; ++i)
			deletion_vector[i] = 0;
	}

	void ParticleLayer::Display(RenderMaterial const * material_override, GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing) const
	{
		// early exit
		if (GetParticleCount() == 0)
			return;
		if (!IsVisible())
			return;
		// search the material
		RenderMaterial const * final_material = material_override;
		if (final_material == nullptr)
		{
			final_material = render_material.get();
			if (final_material == nullptr)
				return;
		}
		// update the vertex declaration
		UpdateVertexDeclaration();
		// Update GPU buffers	
		UpdateGPUBuffers();
		// do the rendering
		DoDisplay(final_material, uniform_provider, instancing);
	}

	void ParticleLayer::DoDisplay(RenderMaterial const * final_material, GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing) const
	{
		// get the vertex array
		VertexArray const * vertex_array = vertex_array_cache.FindOrCreateVertexArray(final_material->GetEffectiveProgram(), vertex_buffer.get(), nullptr, vertex_declaration, 0);
		if (vertex_array == nullptr)
			return;
		// use the material
		final_material->UseMaterial(uniform_provider);
		// bind the vertex array
		glBindVertexArray(vertex_array->GetResourceID());
		// compute the number of vertices
		size_t vertices_count = GetVerticesCountPerParticles() * GetParticleCount();
		// one draw call for the whole buffer
		DrawPrimitive primitive;
		primitive.primitive_type = GL_TRIANGLES;
		primitive.indexed = false;
		primitive.count = vertices_count;
		primitive.start = 0;
		primitive.base_vertex_index = 0;

		primitive.Render(instancing);
	}

	void ParticleLayer::UpdateVertexDeclaration() const
	{
		// is the vertex declaration already filled
		if (vertex_declaration.entries.size() > 0)
			return;
		// fill the vertex declaration
		vertex_declaration = layer_desc->GetVertexDeclaration();
	}

	void ParticleLayer::UpdateGPUBuffers() const
	{
		if (!require_GPU_update)
			return;

		// create the vertex buffer if necessary
		if (vertex_buffer == nullptr)
		{
			GLTools::GenerateVertexAndIndexBuffersObject(nullptr, &vertex_buffer, nullptr);
			if (vertex_buffer == nullptr)
				return;
		}

		// reserve memory
		size_t vertex_buffer_size = GetVertexSize() * GetVerticesCountPerParticles() * GetParticleCount();
		if (vertex_buffer_size == 0)
			return;

		GLuint buffer_id = vertex_buffer->GetResourceID();
		GLenum map_type = (AreParticlesDynamic()) ?
			GL_STREAM_DRAW :
			GL_STATIC_DRAW;
		glNamedBufferData(buffer_id, vertex_buffer_size, nullptr, map_type);

		// map the vertex buffer
		char * buffer = (char*)glMapNamedBuffer(buffer_id, GL_WRITE_ONLY);
		if (buffer == nullptr)
			return;
		// update the buffer
		layer_desc->ParticlesToVertices(&particles[0], particles.size(), buffer, vertex_buffer_size);
		// unmap the buffer
		glUnmapNamedBuffer(buffer_id);

		require_GPU_update = false;
	}

	// ==============================================================
	// PARTICLE MANAGER
	// ==============================================================

	void ParticleManager::SetTextureAtlas(BitmapAtlas::TextureArrayAtlas * in_atlas)
	{
		atlas = in_atlas;
	}

	ParticleLayer * ParticleManager::FindLayer(char const * name)
	{
		if (name == nullptr)
			return nullptr;
		size_t count = layers.size();
		for (size_t i = 0; i < count; ++i)
		{
			ParticleLayer * layer = layers[i].get();
			if (strcmp(layer->GetName(), name) == 0)
				return layer;
		}
		return nullptr;
	}

	ParticleLayer const * ParticleManager::FindLayer(char const * name) const
	{
		if (name == nullptr)
			return nullptr;
		size_t count = layers.size();
		for (size_t i = 0; i < count; ++i)
		{
			ParticleLayer const * layer = layers[i].get();
			if (strcmp(layer->GetName(), name) == 0)
				return layer;
		}
		return nullptr;
	}

	ParticleLayer * ParticleManager::FindLayer(int id)
	{
		size_t count = layers.size();
		for (size_t i = 0; i < count; ++i)
		{
			ParticleLayer * layer = layers[i].get();
			if (layer->GetLayerID() == id)
				return layer;
		}
		return nullptr;
	}

	ParticleLayer const * ParticleManager::FindLayer(int id) const
	{
		size_t count = layers.size();
		for (size_t i = 0; i < count; ++i)
		{
			ParticleLayer const * layer = layers[i].get();
			if (layer->GetLayerID() == id)
				return layer;
		}
		return nullptr;
	}

	size_t ParticleManager::FindLayerIndex(ParticleLayer * layer) const
	{
		size_t count = layers.size();
		for (size_t i = 0; i < count; ++i)
			if (layers[i].get() == layer)
				return i;
		return std::numeric_limits<size_t>::max();
	}

	void ParticleManager::AddLayer(ParticleLayer * layer)
	{
		if (layer == nullptr)
			return;
		// search whether the layer is not already in the manager
		size_t index = FindLayerIndex(layer);
		if (index < layers.size())
			return;
		// insert the layer at the end
		layers.push_back(layer);
	}

	void ParticleManager::RemoveLayer(ParticleLayer * layer)
	{
		if (layer == nullptr)
			return;
		// search whether the layer is in the manager
		size_t index = FindLayerIndex(layer);
		if (index >= layers.size())
			return;
		// remove the layer from the manager
		layers.erase(layers.begin() + index);
	}

	void ParticleManager::Tick(float delta_time)
	{
		size_t count = layers.size();
		for (size_t i = 1; i < count; ++i)
			layers[i]->TickParticles(delta_time);
	}

	bool ParticleManager::AreLayersSorted(bool test_program_id) const
	{
		size_t count = layers.size();
		for (size_t i = 1; i < count; ++i)
		{
			ParticleLayer const * L1 = layers[i - 1].get();
			ParticleLayer const * L2 = layers[i].get();
			// test for render order
			int RenderOrder1 = L1->GetRenderOrder();
			int RenderOrder2 = L2->GetRenderOrder();
			if (RenderOrder1 > RenderOrder2)
				return false;
			// test for program ID
			if (test_program_id && RenderOrder1 == RenderOrder2)
			{
				RenderMaterial const * M1 = L1->GetRenderMaterial();
				if (M1 == nullptr)
					continue;
				RenderMaterial const * M2 = L2->GetRenderMaterial();
				if (M2 == nullptr)
					continue;
				if (M1 == M2)
					continue; // same material => order is good

				GPUProgram const * P1 = M1->GetEffectiveProgram();
				if (P1 == nullptr)
					continue;
				GPUProgram const * P2 = M2->GetEffectiveProgram();
				if (P2 == nullptr)
					continue;
				if (P1 == P2)
					continue; // same program => order is good

				GLuint PID1 = P1->GetResourceID();
				GLuint PID2 = P2->GetResourceID();
				if (PID1 > PID2)
					return false;
			}
		}
		return true;
	}


	void ParticleManager::SortLayers(bool test_program_id) const
	{
		using arg_type = boost::intrusive_ptr<ParticleLayer>;
		std::sort(layers.begin(), layers.end(), [test_program_id](arg_type & V1, arg_type & V2)
		{
			ParticleLayer const * L1 = V1.get();
			ParticleLayer const * L2 = V2.get();
			// test for render order
			int RenderOrder1 = L1->GetRenderOrder();
			int RenderOrder2 = L2->GetRenderOrder();
			if (RenderOrder2 > RenderOrder1)
				return 1;
			// test for material ordering
			if (test_program_id && RenderOrder1 == RenderOrder2)
			{
				RenderMaterial const * M1 = L1->GetRenderMaterial();
				if (M1 == nullptr)
					return 0;
				RenderMaterial const * M2 = L2->GetRenderMaterial();
				if (M2 == nullptr)
					return 0;
				if (M1 == M2)
					return 0; // same material => order is good

				GPUProgram const * P1 = M1->GetEffectiveProgram();
				if (P1 == nullptr)
					return 0;
				GPUProgram const * P2 = M2->GetEffectiveProgram();
				if (P2 == nullptr)
					return 0;
				if (P1 == P2)
					return 0; // same program => order is good

				GLuint PID1 = P1->GetResourceID();
				GLuint PID2 = P2->GetResourceID();
				if (PID2 > PID1)
					return 1;
			}
			return 0;
		});
	}

	void ParticleManager::Display(GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing) const
	{
		// early exit
		size_t count = layers.size();
		if (count == 0)
			return;
		// sort the layer if necessary (Z order + material)
		bool test_program_id = true;
		if (!AreLayersSorted(test_program_id))
			SortLayers(test_program_id);
		// update the states
		UpdateRenderingStates(true);
		// create an uniform provider to enrich the input
		GPUProgramProviderChain main_uniform_provider(uniform_provider);
		if (atlas != nullptr)
			main_uniform_provider.AddVariableTexture("material", atlas->GetTexture());
		// draw all the layers		
		for (size_t i = 0; i < count; ++i)
			layers[i]->Display(nullptr, (atlas != nullptr)? &main_uniform_provider : uniform_provider, instancing);
		// update the states
		UpdateRenderingStates(false);
	}

	void ParticleManager::UpdateRenderingStates(bool begin) const
	{
		if (begin)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
		}
		else
		{
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
		}
	}

#if 0
	GLuint framebuffer = 0;
	glGenFramebuffers(1, &framebuffer);

	GLuint depth_renderbuffer = 0;
	glGenRenderbuffers(1, &depth_renderbuffer);

	glNamedRenderbufferStorage(depth_renderbuffer, GL_DEPTH24_STENCIL8, 512, 512);

	GLuint color_renderbuffer = 0;
	glGenRenderbuffers(1, &color_renderbuffer);
	glNamedRenderbufferStorage(color_renderbuffer, GL_RGBA, 512, 512);

	glNamedFramebufferRenderbuffer(framebuffer, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_renderbuffer);
	glNamedFramebufferTexture(framebuffer, GL_COLOR_ATTACHMENT0, texture_id, texture_level);
	glCheckNamedFramebufferStatus(framebuffer, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;

#endif



}; // namespace chaos

