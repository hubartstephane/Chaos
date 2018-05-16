#include <chaos/ParticleManager.h>
#include <chaos/GLTools.h>
#include <chaos/DrawPrimitive.h>

namespace chaos
{
	// ==============================================================
	// ParticleAllocation
	// ==============================================================

	ParticleAllocation::ParticleAllocation(ParticleLayer * in_layer):
		layer(in_layer)
	{
		assert(in_layer != nullptr);
	}

	ParticleAllocation::~ParticleAllocation()
	{
		assert(layer == nullptr);
	}

	void ParticleAllocation::RemoveFromLayer()
	{
		if (layer == nullptr)
			return;
		layer->RemoveParticleAllocation(this);
	}

	void ParticleAllocation::OnRemovedFromLayer()
	{		
		ConditionalRequireGPUUpdate(false, false);
		layer = nullptr;
	}

	void ParticleAllocation::ConditionalRequireGPUUpdate(bool ignore_visibility, bool ignore_particle_count)
	{
		if (layer == nullptr)
			return;
		if (!ignore_visibility && !IsVisible())
			return;
		if (!ignore_particle_count && GetParticleCount() == 0)
			return;			
		layer->require_GPU_update = true;
	}

	bool ParticleAllocation::IsAttachedToLayer() const
	{
		return (layer != nullptr);
	}

	void ParticleAllocation::Pause(bool in_paused)
	{
		paused = in_paused;
	}

	bool ParticleAllocation::IsPaused() const
	{
		return paused;
	}

	void ParticleAllocation::Show(bool in_visible)
	{
		if (visible != in_visible)
		{
			visible = in_visible;
			ConditionalRequireGPUUpdate(false, false); // the GPU buffer is about to be changed
		}
	}

	bool ParticleAllocation::IsVisible() const
	{
		return visible;
	}

	size_t ParticleAllocation::GetParticleCount() const
	{
		return 0;
	}

	void * ParticleAllocation::GetParticleBuffer()
	{
		return nullptr;
	}

	void const * ParticleAllocation::GetParticleBuffer() const
	{
		return nullptr;
	}

	bool ParticleAllocation::Resize(size_t new_count)
	{
		return true;
	}

	// ==============================================================
	// PARTICLE LAYER DESC
	// ==============================================================

	ParticleAllocation * ParticleLayerDesc::NewAllocation(ParticleLayer * in_layer)
	{
		return new ParticleAllocation(in_layer);
	}

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

	bool ParticleLayerDesc::AreParticlesDynamic() const
	{
		return false;
	}

	VertexDeclaration ParticleLayerDesc::GetVertexDeclaration() const
	{
		return VertexDeclaration();
	}

	size_t ParticleLayerDesc::UpdateParticles(float delta_time, void * particles, size_t particle_count, ParticleAllocation * allocation)
	{
		return particle_count; // no particles destroyed
	}

	size_t ParticleLayerDesc::ParticlesToVertices(void const * particles, size_t particles_count, char * vertices, ParticleAllocation * allocation) const
	{
		assert(particles != nullptr);
		assert(vertices != nullptr);
		return 0; // no vertex inserted
	}

	// ==============================================================
	// PARTICLE LAYER
	// ==============================================================

	ParticleLayer::ParticleLayer(ParticleLayerDesc * in_layer_desc)
		: layer_desc(in_layer_desc)
	{
		assert(in_layer_desc != nullptr);
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
		// faster to do that from end to begin
		while (particles_allocations.size())
			RemoveParticleAllocation(particles_allocations[particles_allocations.size() - 1]);
	}

	void ParticleLayer::RemoveParticleAllocation(ParticleAllocation * allocation)
	{
		assert(allocation != nullptr);
		assert(allocation->layer == this);

		for (size_t i = particles_allocations.size(); i > 0; --i)
		{
			size_t index = i - 1;
			if (particles_allocations[index] == allocation)
			{
				particles_allocations.erase(particles_allocations.begin() + index);
				allocation->OnRemovedFromLayer();
				return;
			}
		}
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

	uintptr_t ParticleLayer::GetParticleID() const
	{
		return layer_desc->GetParticleID();
	}

	void ParticleLayer::TickParticles(float delta_time)
	{
		// early exit
		if (IsPaused())
			return;
		// update the particles themselves
		if (AreParticlesDynamic())
			require_GPU_update |= UpdateParticles(delta_time);
	}

	bool ParticleLayer::UpdateParticles(float delta_time)
	{
		bool result = false;

		size_t count = particles_allocations.size();
		for (size_t i = 0; i < count; ++i)
		{
			ParticleAllocation * allocation = particles_allocations[i];
			// early exit
			if (allocation->IsPaused())
				continue;
			// get the number of particles
			size_t particle_count = allocation->GetParticleCount();
			if (particle_count == 0)
				continue;
			// get the particle (void) buffer
			void * particles = allocation->GetParticleBuffer();
			if (particles == nullptr)
				continue;
			// update all particles
			size_t remaining_particles = layer_desc->UpdateParticles(delta_time, particles, particle_count, allocation);
			// clean buffer of all particles that have bean destroyed
			if (remaining_particles != particle_count)
				allocation->Resize(remaining_particles);
			// particles have changed ... so must it be for vertices
			result = true;
		}
		return result;
	}

	ParticleAllocation * ParticleLayer::SpawnParticles(size_t count)
	{
		// create an allocation
		ParticleAllocation * result = layer_desc->NewAllocation(this);
		if (result == nullptr)
			return nullptr;
		// increase the particle count for that allocation
		result->Resize(count);
		// register the allocation
		particles_allocations.push_back(result);
		return result;
	}

	void ParticleLayer::Display(RenderMaterial const * material_override, GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing) const
	{
		if (!IsVisible())
			return;
		// Update GPU buffers	
		size_t vcount = UpdateGPUBuffers();
		if (vcount == 0)
			return;
		// update the vertex declaration
		UpdateVertexDeclaration();
		// search the material
		RenderMaterial const * final_material = material_override;
		if (final_material == nullptr)
		{
			final_material = render_material.get();
			if (final_material == nullptr)
				return;
		}
		// do the rendering
		DoDisplay(vcount, final_material, uniform_provider, instancing);
	}

	void ParticleLayer::DoDisplay(size_t vcount, RenderMaterial const * final_material, GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing) const
	{
		// no vertices, no rendering
		if (vcount == 0)
			return;
		// get the vertex array
		VertexArray const * vertex_array = vertex_array_cache.FindOrCreateVertexArray(final_material->GetEffectiveProgram(), vertex_buffer.get(), nullptr, vertex_declaration, 0);
		if (vertex_array == nullptr)
			return;
		// use the material
		final_material->UseMaterial(uniform_provider);
		// bind the vertex array
		glBindVertexArray(vertex_array->GetResourceID());
		// one draw call for the whole buffer
		DrawPrimitive primitive;
		primitive.primitive_type = GL_TRIANGLES;
		primitive.indexed = false;
		primitive.count = vcount;
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

	size_t ParticleLayer::UpdateGPUBuffers() const
	{
		size_t result = 0;

		// return the number of vertices from the previous call
		if (!require_GPU_update)
			return vertices_count; 
		// create the vertex buffer if necessary
		if (vertex_buffer == nullptr)
		{
			GLTools::GenerateVertexAndIndexBuffersObject(nullptr, &vertex_buffer, nullptr);
			if (vertex_buffer == nullptr)
				return 0;
		}
		// reserve memory (for the maximum number of vertices possible)
		size_t vertex_buffer_size = GetVertexSize() * GetVerticesCountPerParticles() * ComputeMaxParticleCount();
		if (vertex_buffer_size == 0)
			return 0;

		GLuint buffer_id = vertex_buffer->GetResourceID();
		GLenum map_type = (AreParticlesDynamic()) ?
			GL_STREAM_DRAW :
			GL_STATIC_DRAW;
		glNamedBufferData(buffer_id, vertex_buffer_size, nullptr, map_type);

		// map the vertex buffer
		char * buffer = (char*)glMapNamedBuffer(buffer_id, GL_WRITE_ONLY);
		if (buffer == nullptr)
			return 0;
		// update the buffer
		result = DoUpdateGPUBuffers(buffer, vertex_buffer_size);
		// unmap the buffer
		glUnmapNamedBuffer(buffer_id);

		// no more update required
		require_GPU_update = false;
		// cache the number of particles inserted
		vertices_count = result;

		return result;
	}

	size_t ParticleLayer::DoUpdateGPUBuffers(char * buffer, size_t vertex_buffer_size) const
	{
		size_t result = 0;

		size_t vertex_size = GetVertexSize();

		size_t count = particles_allocations.size();
		for (size_t i = 0; i < count; ++i)
		{
			// get the allocation, ignore if invisible
			ParticleAllocation * allocation = particles_allocations[i];
			if (!allocation->IsVisible())
				continue;
			// ignore empty allocations
			size_t particle_count = allocation->GetParticleCount();
			if (particle_count == 0)
				continue;
			// get the buffer
			void * particles = allocation->GetParticleBuffer();
			if (particles == nullptr)
				continue;
			// transform particles into vertices
			size_t new_vertices = layer_desc->ParticlesToVertices(particles, particle_count, buffer, allocation);
			// shift buffer
			buffer += new_vertices * vertex_size;
			result += new_vertices;
		}

		return result;
	}

	size_t ParticleLayer::ComputeMaxParticleCount() const
	{
		size_t result = 0;

		size_t count = particles_allocations.size();
		for (size_t i = 0; i < count; ++i)
		{
			// get the allocation, ignore if invisible
			ParticleAllocation * allocation = particles_allocations[i];
			if (!allocation->IsVisible())
				continue;
			// ignore empty allocations
			size_t particle_count = allocation->GetParticleCount();
			if (particle_count == 0)
				continue;

			result += allocation->GetParticleCount();
		}
		return result;
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

	ParticleLayer * ParticleManager::AddLayer(ParticleLayerDesc * layer_desc)
	{
		// test entry
		if (layer_desc == nullptr)
			return nullptr;
		// create the layer
		ParticleLayer * result = new ParticleLayer(layer_desc);
		if (result == nullptr)
			return nullptr;
		// insert the layer at the end
		layers.push_back(result);
		return result;
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
		for (size_t i = 0; i < count; ++i)
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
}; // namespace chaos

