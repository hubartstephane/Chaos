namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

    template<typename PARTICLE_TYPE>
    using ParticleAccessor = RawDataBufferAccessor<PARTICLE_TYPE>;
    template<typename PARTICLE_TYPE>
    using ParticleConstAccessor = RawDataBufferConstAccessor<PARTICLE_TYPE>;

    template<typename ALLOCATION_TYPE>
    class AutoCastedParticleAccessorBase;

    class AutoCastedParticleAccessor;
    class AutoCastedParticleConstAccessor;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

    // ==============================================================
    // AutoCastedParticleAccessorBase
    // ==============================================================

    template<typename ALLOCATION_TYPE>
    class /*CHAOS_API*/ AutoCastedParticleAccessorBase
    {
    public:

        /** constructor */
        AutoCastedParticleAccessorBase() = default;
        /** copy constructor */
        AutoCastedParticleAccessorBase(AutoCastedParticleAccessorBase<ALLOCATION_TYPE> const& src) = default;
        /** initialization contructor */
        AutoCastedParticleAccessorBase(ALLOCATION_TYPE* in_allocation_base, size_t in_start = 0, size_t in_count = 0) :
            allocation_base(in_allocation_base),
            start(in_start),
            count(in_count)
        {
            assert(in_allocation_base != nullptr);
        }

        /** returns the number of particles inside the accessor */
        size_t GetCount() const
        {
            return count;
        }
        /** returns the start of particles inside the accessor */
        size_t GetStart() const
        {
            return start;
        }
        /** implicit conversion to bool whether the accessor has elements */
        operator bool() const
        {
            return (count > 0);
        }

    protected:

        /** the allocation we are using */
        ALLOCATION_TYPE* allocation_base = nullptr;
        /** particle to start iteration on */
        size_t start = 0;
        /** number of particles concerned */
        size_t count = 0;
};

    // ==============================================================
    // AutoCastedParticleAccessor
    // ==============================================================

    class CHAOS_API AutoCastedParticleAccessor : public AutoCastedParticleAccessorBase<ParticleAllocationBase>
    {
        friend class AutoCastedParticleConstAccessor;

    public:

        /** constructor */
        AutoCastedParticleAccessor() = default;
        /** copy constructor */
        AutoCastedParticleAccessor(AutoCastedParticleAccessor const& src) = default;
        /** initialization contructor */
        AutoCastedParticleAccessor(ParticleAllocationBase* in_allocation_base, size_t in_start = 0, size_t in_count = 0) :
            AutoCastedParticleAccessorBase<ParticleAllocationBase>(in_allocation_base, in_start, in_count)
        {
        }

        /** the conversion method */
        template<typename PARTICLE_TYPE>
        operator ParticleAccessor<PARTICLE_TYPE>() const;

        /** the conversion method */
        template<typename PARTICLE_TYPE>
        operator ParticleConstAccessor<PARTICLE_TYPE>() const;
    };

    // ==============================================================
    // AutoCastedParticleConstAccessor
    // ==============================================================

    class CHAOS_API AutoCastedParticleConstAccessor : public AutoCastedParticleAccessorBase<ParticleAllocationBase const>
    {
    public:

        /** constructor */
        AutoCastedParticleConstAccessor() = default;
        /** copy constructor */
        AutoCastedParticleConstAccessor(AutoCastedParticleConstAccessor const& src) = default;
        /** initialization contructor */
        AutoCastedParticleConstAccessor(ParticleAllocationBase const* in_allocation_base, size_t in_start = 0, size_t in_count = 0) :
            AutoCastedParticleAccessorBase<ParticleAllocationBase const>(in_allocation_base, in_start, in_count)
        {
        }

        /** the conversion method */
        template<typename PARTICLE_TYPE>
        operator ParticleConstAccessor<PARTICLE_TYPE>() const;
    };


#else

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

    template<typename PARTICLE_TYPE>
    AutoCastedParticleConstAccessor::operator ParticleConstAccessor<PARTICLE_TYPE>() const
    {
        return (allocation_base != nullptr) ?
            allocation_base->GetParticleConstAccessor<PARTICLE_TYPE>(start, count) :
            ParticleConstAccessor<PARTICLE_TYPE>();
    }

#endif

}; // namespace chaos