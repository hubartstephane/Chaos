#include <chaos/Chaos.h>

namespace chaos
{
	ParticleLayerBase::~ParticleLayerBase()
	{
		DetachAllParticleAllocations();
	}

	void ParticleLayerBase::DetachAllParticleAllocations()
	{
		// faster to do that from end to begin
		while (particles_allocations.size())
			RemoveParticleAllocation(particles_allocations[particles_allocations.size() - 1].get());
	}

	void ParticleLayerBase::RemoveParticleAllocation(ParticleAllocationBase * allocation)
	{
		assert(allocation != nullptr);
		assert(allocation->layer == this);

		for (size_t i = particles_allocations.size(); i > 0; --i) // from end to beginning because DetachAllParticleAllocations(...) is from end to the beginning => make the code much faster
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

	bool ParticleLayerBase::DoTick(float delta_time)
	{
		// update the particles themselves
		if (AreParticlesDynamic())
			require_GPU_update |= TickAllocations(delta_time);
		return true;
	}

	bool ParticleLayerBase::TickAllocations(float delta_time)
	{
		bool result = false;

		// store allocations that want to be notified of their emptyness here,
		// to be handled after the main loop
		std::vector<ParticleAllocationBase*> to_destroy_allocations;

		// main loop
		size_t count = particles_allocations.size();
		for (size_t i = 0; i < count; ++i)
		{
			ParticleAllocationBase * allocation = particles_allocations[i].get();
			if (allocation == nullptr)
				continue;
			// tick or destroy the allocation
			bool destroy_allocation = false;
			if (allocation->GetParticleCount() == 0 && allocation->GetDestroyWhenEmpty()) // XXX: if the TRAIT is not particle_dynamic, this will never be called
				destroy_allocation = true;
			else
				destroy_allocation = TickAllocation(delta_time, allocation); // tick this single allocation

			// register as an allocation to be destroyed
			if (destroy_allocation)
				to_destroy_allocations.push_back(allocation);
			// particles have changed ... so must it be for vertices
			result = true;
		}

		// handle allocation that wanted to react whenever they become empty
		// XXX : this is done outside the loop because this could callback can create or destroy some allocations 
		//       causing iteration in 'particles_allocations' dangerous
		size_t empty_count = to_destroy_allocations.size();
		for (size_t i = 0; i < empty_count; ++i)
			to_destroy_allocations[i]->RemoveFromLayer();

		return result;
	}

	ParticleAllocationBase * ParticleLayerBase::SpawnParticles(size_t count, bool new_allocation)
	{
		ParticleAllocationBase* result = nullptr;

		// create an allocation is mandatory
		if (new_allocation || particles_allocations.size() == 0)
		{
			result = DoCreateParticleAllocation();
			if (result == nullptr)
				return nullptr;
			particles_allocations.push_back(result); // register the allocation
		}
		// get the very first allocation
		else
		{
			result = particles_allocations[0].get();
		}

		// increase the particle count for that allocation
		result->AddParticles(count);

		return result;
	}

	int ParticleLayerBase::DoDisplay(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params)
	{
        // early exit
        if (dynamic_mesh.IsEmpty())
            return 0;
		// search the material
		GPURenderMaterial const * final_material = render_params.GetMaterial(this, render_material.get());
        if (final_material == nullptr)
            return 0;
		// prepare rendering state
		UpdateRenderingStates(renderer, true);
		// update uniform provider with atlas, and do the rendering
		GPUProgramProviderChain main_uniform_provider(uniform_provider);
		if (atlas != nullptr)
			main_uniform_provider.AddTexture("material", atlas->GetTexture());
		int result = DoDisplayHelper(renderer, final_material, (atlas == nullptr) ? uniform_provider : &main_uniform_provider, render_params);
		// restore rendering states
		UpdateRenderingStates(renderer, false);
		return result;
	}

    int ParticleLayerBase::DoDisplayHelper(GPURenderer* renderer, GPURenderMaterial const* final_material, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params)
    {
        // create a new GPURenderParams that override the Material for inside the GPUDynamicMesh
        DisableReferenceCount<GPUConstantMaterialProvider> material_provider(final_material);  // while on stack, use DisableReferenceCount<...>

        GPURenderParams other_render_params = render_params;
        other_render_params.material_provider = &material_provider;
        // let the dynamic mesh render itself
        return dynamic_mesh.Display(renderer, uniform_provider, other_render_params);
    }

    void ParticleLayerBase::UpdateVertexDeclaration()
    {
        // is the vertex declaration already filled
        if (vertex_declaration != nullptr)
            return;
        // fill the vertex declaration
        vertex_declaration = GetVertexDeclaration();
    }

    size_t ParticleLayerBase::EvaluateGPUVertexMemoryRequirement() const
    {
        size_t result = GetDynamicMeshVertexCount(dynamic_mesh); 
        if (result == 0) // happens whenever the mesh is empty (first call for example)
        {
            // XXX : for strip and fans, we take 6 as an empiric value
            size_t real_vertices_per_particle = GetRealVerticesPerParticle();
            if (real_vertices_per_particle == 0)
                real_vertices_per_particle = 6;
            result = GetParticleCount() * real_vertices_per_particle; // suppose 1 particle => 1 primitive (this is not mandatory)
        }
        return result;
    }

    bool ParticleLayerBase::DoUpdateGPUResources(GPURenderer* renderer)
    {
        // update the vertex declaration
        UpdateVertexDeclaration();
        // ensure their is some reason to update the rendering data
        if (!require_GPU_update && !AreVerticesDynamic())
            return true;
        // evaluate how much memory should be allocated for buffers (count in vertices)
        size_t vertex_requirement_evaluation = EvaluateGPUVertexMemoryRequirement();
        // clear previous dynamic mesh (and give buffers back for further usage)       
        if (particle_manager != nullptr)
        {
            dynamic_mesh.Clear(&particle_manager->GetBufferCache());
            dynamic_mesh.SetVertexArrayCache(particle_manager->GetVertexArrayCache());
        }
        else
        {
            if (vertex_array_cache == nullptr)
                vertex_array_cache = new GPUVertexArrayCache;
            dynamic_mesh.Clear(&buffer_cache);
            dynamic_mesh.SetVertexArrayCache(vertex_array_cache.get());
        }
        // select PrimitiveOutput and collect vertices        
        DoUpdateGPUBuffers(renderer, vertex_requirement_evaluation);
        // mark as up to date
        require_GPU_update = false;

        return true;
    }

	size_t ParticleLayerBase::ComputeMaxParticleCount() const
	{
		size_t result = 0;

		size_t count = particles_allocations.size();
		for (size_t i = 0; i < count; ++i)
		{
			// get the allocation, ignore if invisible
			ParticleAllocationBase * allocation = particles_allocations[i].get();
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

	void ParticleLayerBase::UpdateRenderingStates(GPURenderer* renderer, bool begin) const
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

	size_t ParticleLayerBase::GetAllocationCount() const
	{
		return particles_allocations.size();
	}
	ParticleAllocationBase * ParticleLayerBase::GetAllocation(size_t index)
	{
		return particles_allocations[index].get();
	}
	ParticleAllocationBase const * ParticleLayerBase::GetAllocation(size_t index) const
	{
		return particles_allocations[index].get();
	}

	void ParticleLayerBase::ClearAllAllocations()
	{
		size_t count = particles_allocations.size();
		for (size_t i = count ; i > 0 ; --i)
			particles_allocations[i - 1]->Resize(0);
	}

    size_t ParticleLayerBase::GetDynamicMeshVertexCount(GPUDynamicMesh const& mesh) const
    {
        size_t result = 0;

        size_t count = mesh.GetMeshElementCount();
        for (size_t i = 0 ; i < count; ++i)
        {
            GPUDynamicMeshElement const& element = mesh.GetMeshElement(i);
            for (GPUDrawPrimitive const& primitive : element.primitives)
                result += primitive.count;
        }
        return result;
    }

}; // namespace chaos

