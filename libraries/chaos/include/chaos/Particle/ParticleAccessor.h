#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{


}; // namespace chaos

#else 

namespace chaos
{

    // ==============================================================
    // AutoCastedParticleAccessor
    // ==============================================================

    template<typename PARTICLE_TYPE>
    AutoCastedParticleAccessor::operator ParticleAccessor<PARTICLE_TYPE>() const
    {
        return (allocation_base != nullptr) ?
            allocation_base->GetParticleAccessor<PARTICLE_TYPE>(start, count) :
            ParticleAccessor<PARTICLE_TYPE>();
    }

    template<typename PARTICLE_TYPE>
    AutoCastedParticleAccessor::operator ParticleConstAccessor<PARTICLE_TYPE>() const
    {
        return (allocation_base != nullptr) ?
            allocation_base->GetParticleConstAccessor<PARTICLE_TYPE>(start, count) :
            ParticleConstAccessor<PARTICLE_TYPE>();
    }

    // ==============================================================
    // AutoCastedParticleConstAccessor
    // ==============================================================

    template<typename PARTICLE_TYPE>
    AutoCastedParticleConstAccessor::operator ParticleConstAccessor<PARTICLE_TYPE>() const
    {
        return (allocation_base != nullptr) ?
            allocation_base->GetParticleConstAccessor<PARTICLE_TYPE>(start, count) :
            ParticleConstAccessor<PARTICLE_TYPE>();
    }

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION



