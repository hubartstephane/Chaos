#include <chaos/ParticleManager.h>
#include <chaos/GLTools.h>
#include <chaos/DrawPrimitive.h>
#include <chaos/GPUResourceManager.h>
#include <chaos/MyGLFWSingleWindowApplication.h>
#include <chaos/ParticleTools.h>

namespace chaos
{

	// ==============================================================
	// ParticleAllocationEmptyCallback
	// ==============================================================

	bool ParticleAllocationEmptyCallback::OnAllocationEmpty(ParticleAllocation * allocation)
	{
		assert(allocation != nullptr);
		return true;
	}

	// ==============================================================
	// ParticleAllocationAutoRemoveEmptyCallback
	// ==============================================================

	bool ParticleAllocationAutoRemoveEmptyCallback::OnAllocationEmpty(ParticleAllocation * allocation)
	{
		assert(allocation != nullptr);
		allocation->RemoveFromLayer();
		return true;
	}		

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
		ConditionalRequireGPUUpdate(true, true);
		layer = nullptr;
	}

	void ParticleAllocation::ConditionalRequireGPUUpdate(bool skip_if_invisible, bool skip_if_empty)
	{
		if (layer == nullptr)
			return;
		if (skip_if_invisible && !IsVisible())
			return;
		if (skip_if_empty && GetParticleCount() == 0)
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
			ConditionalRequireGPUUpdate(false, true); // the GPU buffer is about to be changed
		}
	}

	bool ParticleAllocation::IsVisible() const
	{
		return visible;
	}

	size_t ParticleAllocation::GetParticleSize() const
	{
		return 0;
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

	bool ParticleAllocation::AddParticles(size_t extra_count)
	{
		return Resize(extra_count + GetParticleCount());
	}

	void ParticleAllocation::SetEmptyCallbackAutoRemove()
	{
		static DisableLastReferenceLost<ParticleAllocationAutoRemoveEmptyCallback> auto_remove_callback;
		SetEmptyCallback(&auto_remove_callback);
	}

	//
	// XXX : SpawnParticle(...) returns a raw pointer on ParticleAllocation
	//       the user has to possibilities
	//         - he ignores the result => the allocation is stored inside the layer and will be destroyed when the layer is destroyed
	//         - he stores the result into an intrusive_ptr<...>
	//              -> that means that he wants to override the allocation lifetime
	//              -> as soon as this intrusive_ptr<...> is destroyed, we want to destroy the ParticleAllocation even if there still is 1 reference
	//                 from the layer
	//
	void ParticleAllocation::SubReference(SharedPointerPolicy policy)
	{
		if (layer == nullptr)
		{
			if (--shared_count == 0) // the ParticleAllocation is no more in a layer and there is no reference on it => destroy it
				OnLastReferenceLost();							
		}
		else
		{
			if (--shared_count == 1) // the last reference is the one from the layer. Destroy it
				RemoveFromLayer();		
		}				
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

	bool ParticleLayerDesc::AreVerticesDynamic() const
	{
		return false;
	}

	bool ParticleLayerDesc::AreParticlesDynamic() const
	{
		return false;
	}

	GPUVertexDeclaration ParticleLayerDesc::GetVertexDeclaration() const
	{
		return GPUVertexDeclaration();
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

	void ParticleLayer::DetachAllParticleAllocations()
	{
		// faster to do that from end to begin
		while (particles_allocations.size())
			RemoveParticleAllocation(particles_allocations[particles_allocations.size() - 1].get());
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
				allocation->OnRemovedFromLayer();
				particles_allocations.erase(particles_allocations.begin() + index);
				return;
			}
		}
	}

	ClassTools::ClassRegistration const * ParticleLayer::GetParticleClass() const
	{
		return layer_desc->GetParticleClass();
	}

	bool ParticleLayer::DoTick(double delta_time)
	{
		// update the particles themselves
		if (AreParticlesDynamic())
			require_GPU_update |= UpdateParticles((float)delta_time);
		return true;
	}

	bool ParticleLayer::UpdateParticles(float delta_time)
	{
		bool result = false;
		
		// store allocations that want to be notified of their emptyness here,
		// to be handled after the main loop
		std::vector<ParticleAllocation *> empty_allocations; 

		// main loop
		size_t count = particles_allocations.size();
		for (size_t i = 0 ; i < count; ++i)
		{
			ParticleAllocation * allocation = particles_allocations[i].get();
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

			// test whether the allocation becomes empty
			if (remaining_particles == 0)
				if (allocation->empty_callback != nullptr)
					empty_allocations.push_back(allocation);
			
			// particles have changed ... so must it be for vertices
			result = true;
		}

		// handle allocation that wanted to react whenever they become empty
		// XXX : this is done outside the loop because this could callback can create or destroy some allocations 
		//       causing iteration in 'particles_allocations' dangerous

		size_t empty_count = empty_allocations.size();
		for (size_t i = 0 ; i < empty_count; ++i)
			empty_allocations[i]->empty_callback->OnAllocationEmpty(empty_allocations[i]);

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

	int ParticleLayer::DoDisplay(GPUProgramProviderBase const * uniform_provider, RenderParams const & render_params) const
	{
		// early exit
		if (vertices_count == 0)
			return 0;
		// search the material
		GPURenderMaterial const * final_material = render_params.GetMaterial(this, render_material.get());
		// prepare rendering state
		UpdateRenderingStates(true);
		// update uniform provider with atlas, and do the rendering
		chaos::GPUProgramProviderChain main_uniform_provider(uniform_provider);
		if (atlas != nullptr)
			main_uniform_provider.AddVariableTexture("material", atlas->GetTexture());

		int result = DoDisplayHelper(vertices_count, final_material, (atlas == nullptr)? uniform_provider : &main_uniform_provider, render_params.instancing);
		// restore rendering states
		UpdateRenderingStates(false);
		return result;
	}

	bool ParticleLayer::DoUpdateGPUResources() const
	{
		// update the vertex declaration
		UpdateVertexDeclaration();
		// return the number of vertices from the previous call
		if (!require_GPU_update && !AreVerticesDynamic())
			return true; 
		// create the vertex buffer if necessary
		if (vertex_buffer == nullptr)
		{
			vertices_count = 0; // no vertices inside fot the moment

			vertex_buffer = new GPUVertexBuffer();
			if (vertex_buffer == nullptr || !vertex_buffer->IsValid())
			{
				vertex_buffer = nullptr;
				return false;
			}
		}

		bool dynamic_buffer = (AreVerticesDynamic() || AreParticlesDynamic());

		// reserve memory (for the maximum number of vertices possible)
		size_t vertex_buffer_size = GetVertexSize() * GetVerticesCountPerParticles() * ComputeMaxParticleCount();
		if (vertex_buffer_size == 0)
		{
			vertex_buffer->SetBufferData(nullptr, 0, dynamic_buffer, GPUBufferResizePolicy()); // empty the buffer : for some reason, we cannot just kill the buffer
			vertices_count = 0;
			return true;
		}
		
		if (!vertex_buffer->SetBufferData(nullptr, vertex_buffer_size, dynamic_buffer, GPUBufferDoublingResizePolicy()))
		{
			vertices_count = 0;
			return false;
		}
		// map the vertex buffer
		char * buffer = vertex_buffer->MapBuffer(0, vertex_buffer_size, false, true);
		if (buffer == nullptr)
		{
			vertices_count = 0;
			return false;
		}
		// update the buffer
		vertices_count = DoUpdateGPUBuffers(buffer, vertex_buffer_size);
		// unmap the buffer
		vertex_buffer->UnMapBuffer();

		// no more update required
		require_GPU_update = false;

		return true;
	}

	int ParticleLayer::DoDisplayHelper(size_t vertex_count, GPURenderMaterial const * final_material, GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing) const
	{
		// no vertices, no rendering
		if (vertex_count == 0)
			return 0;
		// get the vertex array
		GPUVertexArray const * vertex_array = vertex_array_cache.FindOrCreateVertexArray(final_material->GetEffectiveProgram(), vertex_buffer.get(), nullptr, vertex_declaration, 0);
		if (vertex_array == nullptr)
			return 0;
		// use the material
		final_material->UseMaterial(uniform_provider);
		// bind the vertex array
		glBindVertexArray(vertex_array->GetResourceID());
		// one draw call for the whole buffer
		DrawPrimitive primitive;
		primitive.primitive_type = GL_TRIANGLES;
		primitive.indexed = false;
		primitive.count = (int)vertex_count;
		primitive.start = 0;
		primitive.base_vertex_index = 0;

		primitive.Render(instancing);
		return 1; // 1 DrawCall
	}

	void ParticleLayer::UpdateVertexDeclaration() const
	{
		// is the vertex declaration already filled
		if (vertex_declaration.entries.size() > 0)
			return;
		// fill the vertex declaration
		vertex_declaration = layer_desc->GetVertexDeclaration();
	}

	size_t ParticleLayer::DoUpdateGPUBuffers(char * buffer, size_t vertex_buffer_size) const
	{
		size_t result = 0;

		size_t vertex_size = GetVertexSize();

		size_t count = particles_allocations.size();
		for (size_t i = 0; i < count; ++i)
		{
			// get the allocation, ignore if invisible
			ParticleAllocation * allocation = particles_allocations[i].get();
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
			ParticleAllocation * allocation = particles_allocations[i].get();
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

	void ParticleLayer::UpdateRenderingStates(bool begin) const
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

	ParticleLayer * ParticleLayer::CreateParticleLayer(ParticleLayerDesc * layer_desc, GPURenderMaterial * render_material)
	{
		ParticleLayer * result = new ParticleLayer(layer_desc);
		if (result == nullptr)
			return nullptr;
		result->SetRenderMaterial(render_material);
		return result;
	}

	ParticleLayer * ParticleLayer::CreateParticleLayer(ParticleLayerDesc * layer_desc, char const * material_name)
	{
		// find the optional GPURenderMaterial
		GPURenderMaterial * render_material = nullptr;
		if (material_name != nullptr)
		{
			GPUResourceManager * resource_manager = MyGLFW::SingleWindowApplication::GetGPUResourceManagerInstance();
			if (resource_manager == nullptr)
				return nullptr;
			render_material = resource_manager->FindRenderMaterial(material_name);
			if (render_material == nullptr)
				return nullptr;
		}
		// create the layer
		return CreateParticleLayer(layer_desc, render_material);
	}

	size_t ParticleLayer::GetAllocationCount() const
	{
		return particles_allocations.size();
	}
	ParticleAllocation * ParticleLayer::GetAllocation(size_t index)
	{
		return particles_allocations[index].get();
	}
	ParticleAllocation const * ParticleLayer::GetAllocation(size_t index) const
	{
		return particles_allocations[index].get();
	}

}; // namespace chaos

