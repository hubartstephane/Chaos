#include <chaos/Chaos.h>

namespace chaos
{

    // ==============================================================
    // AutoCastedParticleAccessor
    // ==============================================================

    template<typename PARTICLE_TYPE>
    operator AutoCastedParticleAccessor::ParticleAccessor<PARTICLE_TYPE>() const
    {
        return (allocation_base != nullptr) ?
            allocation_base->GetParticleAccessor<PARTICLE_TYPE>(start, count) :
            ParticleAccessor<PARTICLE_TYPE>();
    }

    template<typename PARTICLE_TYPE>
    operator AutoCastedParticleAccessor::ParticleConstAccessor<PARTICLE_TYPE>() const
    {
        return (allocation_base != nullptr) ?
            allocation_base->GetParticleConstAccessor<PARTICLE_TYPE>(start, count) :
            ParticleConstAccessor<PARTICLE_TYPE>();
    }

    // ==============================================================
    // AutoCastedParticleConstAccessor
    // ==============================================================

    template<typename PARTICLE_TYPE>
    operator AutoCastedParticleConstAccessor::ParticleConstAccessor<PARTICLE_TYPE>() const
    {
        return (allocation_base != nullptr) ?
            allocation_base->GetParticleConstAccessor<PARTICLE_TYPE>(start, count) :
            ParticleConstAccessor<PARTICLE_TYPE>();
    }



}; // namespace chaos

