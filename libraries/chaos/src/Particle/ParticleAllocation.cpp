#include "chaos/ChaosPCH.h"
#include "chaos/Chaos.h"

namespace chaos
{
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

	AutoCastedParticleAccessor ParticleAllocationBase::Resize(size_t new_count) 
	{ 
		return AutoCastedParticleAccessor(this, 0, 0); 
	}

	AutoCastedParticleAccessor ParticleAllocationBase::GetParticleAccessor(size_t start, size_t count)
	{
		return AutoCastedParticleAccessor(this, start, count);
	}

	AutoCastedParticleConstAccessor ParticleAllocationBase::GetParticleAccessor(size_t start, size_t count) const
	{
		return AutoCastedParticleConstAccessor(this, start, count);
	}

	AutoCastedParticleConstAccessor ParticleAllocationBase::GetParticleConstAccessor(size_t start, size_t count) const
	{
		return AutoCastedParticleConstAccessor(this, start, count);
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
	void ParticleAllocationBase::SubReference()
	{
        // the ParticleAllocation is handled as usual
		if (layer == nullptr)
			Object::SubReference(); 
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

}; // namespace chaos

