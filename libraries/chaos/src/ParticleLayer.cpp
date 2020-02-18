#include <chaos/ParticleManager.h>

#include <chaos/GPURenderParams.h>
#include <chaos/GLTools.h>
#include <chaos/GPUDrawPrimitive.h>
#include <chaos/GPUResourceManager.h>
#include <chaos/MyGLFWSingleWindowApplication.h>
#include <chaos/ParticleTools.h>

namespace chaos
{

	// ==============================================================
	// ParticleAllocationBase
	// ==============================================================

	ParticleAllocationBase::ParticleAllocationBase(ParticleLayerBase * in_layer) :
		layer(in_layer)
	{
		assert(in_layer != nullptr);
	}

	ParticleAllocationBase::~ParticleAllocationBase()
	{
		assert(layer == nullptr);
	}

	void ParticleAllocationBase::RemoveFromLayer()
	{
		if (layer == nullptr)
			return;
		layer->RemoveParticleAllocation(this);
	}

	void ParticleAllocationBase::OnRemovedFromLayer()
	{
		ConditionalRequireGPUUpdate(true, true);
		layer = nullptr;
	}

	void ParticleAllocationBase::ConditionalRequireGPUUpdate(bool skip_if_invisible, bool skip_if_empty)
	{
		if (layer == nullptr)
			return;
		if (skip_if_invisible && !IsVisible())
			return;
		if (skip_if_empty && GetParticleCount() == 0)
			return;
		layer->require_GPU_update = true;
	}

	bool ParticleAllocationBase::IsAttachedToLayer() const
	{
		return (layer != nullptr);
	}

	void ParticleAllocationBase::Pause(bool in_paused)
	{
		paused = in_paused;
	}

	bool ParticleAllocationBase::IsPaused() const
	{
		return paused;
	}

	void ParticleAllocationBase::Show(bool in_visible)
	{
		if (visible != in_visible)
		{
			visible = in_visible;
			ConditionalRequireGPUUpdate(false, true); // the GPU buffer is about to be changed
		}
	}

	bool ParticleAllocationBase::IsVisible() const
	{
		return visible;
	}

    AutoCastedParticleAccessor ParticleAllocationBase::AddParticles(size_t extra_count)
	{
		return Resize(extra_count + GetParticleCount());
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
	void ParticleAllocationBase::SubReference(SharedPointerPolicy policy)
	{
        // the ParticleAllocation is handled as usual
		if (layer == nullptr)
			ReferencedObject::SubReference(policy); 
        // the last reference is the one from the layer. Destroy it
		else if (--shared_count == 1) 
            RemoveFromLayer();
	}

    void const* ParticleAllocationBase::GetAccessorEffectiveRanges(size_t& start, size_t& count, size_t& particle_size) const
    {
        size_t particle_count = GetParticleCount();
        if (particle_count == 0)
            return nullptr;
        if (start >= particle_count)
            return nullptr;
        if (count == 0) // 0 = map all
        {
            count = particle_count - start;
            if (count == 0)
                return nullptr; // nothing more to map
        }
        else if (start + count > particle_count) // map all what required or nothing
            return nullptr;
        // compute some other useful values
        particle_size = GetParticleSize();
        void const* buffer = GetParticleBuffer();        
        return ((char const*)buffer) + start * particle_size;
    }

	// ==============================================================
	// ParticleLayerBase
	// ==============================================================

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
		std::vector<ParticleAllocationBase *> to_destroy_allocations;

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
				destroy_allocation = allocation->TickAllocation(delta_time, GetLayerTrait());

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

	ParticleAllocationBase * ParticleLayerBase::SpawnParticles(size_t count)
	{
		// create an allocation
		ParticleAllocationBase * result = DoSpawnParticles();
		if (result == nullptr)
			return nullptr;
		// increase the particle count for that allocation
		result->Resize(count);
		// register the allocation
		particles_allocations.push_back(result);

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
		chaos::GPUProgramProviderChain main_uniform_provider(uniform_provider);
		if (atlas != nullptr)
			main_uniform_provider.AddVariableTexture("material", atlas->GetTexture());
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
        if (result == 0) // happens whenever the mesh is empty
        {
            // XXX : for strip and fans, we take 6 as an empiric value
            size_t real_vertices_per_particle = GetRealVerticesPerParticle();
            if (real_vertices_per_particle == 0)
                real_vertices_per_particle = 6;
            result = GetParticleCount() * real_vertices_per_particle;
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

	void ParticleLayerBase::UpdateRenderingStates(GPURenderer * renderer, bool begin) const
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

