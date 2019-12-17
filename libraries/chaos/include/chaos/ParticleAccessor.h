#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{



		// ==============================================================
		// ParticleAccessorBase
		// ==============================================================

    template<typename PARTICLE_TYPE, typename BUFFER_TYPE>
    class ParticleAccessorBase
    {
    public:

        using type = PARTICLE_TYPE;

        /** default constructor */
        ParticleAccessorBase() = default;
        /** copy constructor */
        ParticleAccessorBase(ParticleAccessorBase const& src) = default;
        /** constructor */
        ParticleAccessorBase(BUFFER_TYPE* in_buffer, size_t in_count, size_t in_particle_size) :
            buffer(in_buffer), count(in_count), particle_size(in_particle_size)
        {
            assert((count > 0) ^ (buffer == nullptr));
        }
        /** pointer indirection */
        type& operator * () const
        {
            assert(count > 0);
            return *((type*)buffer);
        }
        /** pointer inference */
        type* operator -> () const
        {
            if (count == 0)
                return nullptr;
            return ((type*)buffer);
        }
        /** array accessor */
        type& operator [](size_t index) const
        {
            assert(index < count);
            char* b = (char*)buffer;
            return *((type*)(b + index * particle_size));
        }

        /** returns the raw buffer of the accessor */
        BUFFER_TYPE* GetBuffer() const
        {
            return buffer;
        }
        /** gets the number of particle */
        size_t GetCount() const
        {
            return count;
        }
        /** get the particle size */
        size_t GetParticleSize() const
        {
            return particle_size;
        }

    protected:

        /** the start of the buffer */
        BUFFER_TYPE* buffer = nullptr;
        /** the number of particles in that buffer */
        size_t count = 0;
        /** the real particle size (not PARTICLE_TYPE) */
        size_t particle_size = 0;
    };

    // ==============================================================
    // ParticleAccessor / ParticleConstAccessor
    // ==============================================================

    template<typename PARTICLE_TYPE>
    class ParticleAccessor : public ParticleAccessorBase<PARTICLE_TYPE, void>
    {
    public:

        /** default constructor */
        ParticleAccessor() = default;
        /** copy (+downcast operator) */
        template<typename OTHER_PARTICLE_TYPE>
        ParticleAccessor(ParticleAccessor<OTHER_PARTICLE_TYPE> const& src, std::enable_if_t<std::is_base_of_v<PARTICLE_TYPE, OTHER_PARTICLE_TYPE>, int> = 0) :
            ParticleAccessorBase(src.GetBuffer(), src.GetCount(), src.GetParticleSize())
        {
        }
        /** constructor */
        ParticleAccessor(void* in_buffer, size_t in_count, size_t in_particle_size) :
            ParticleAccessorBase(in_buffer, in_count, in_particle_size)
        {
        }
    };

    template<typename PARTICLE_TYPE>
    class ParticleConstAccessor : public ParticleAccessorBase<PARTICLE_TYPE const, void const>
    {
    public:

        /** default constructor */
        ParticleConstAccessor() = default;
        /** copy (+downcast operator) */
        template<typename OTHER_PARTICLE_TYPE>
        ParticleConstAccessor(ParticleConstAccessor<OTHER_PARTICLE_TYPE> const& src, std::enable_if_t<std::is_base_of_v<PARTICLE_TYPE, OTHER_PARTICLE_TYPE>, int> = 0) :
            ParticleAccessorBase(src.GetBuffer(), src.GetCount(), src.GetParticleSize())
        {
        }
        /** copy from non const (+downcast operator) */
        template<typename OTHER_PARTICLE_TYPE>
        ParticleConstAccessor(ParticleAccessor<OTHER_PARTICLE_TYPE> const& src, std::enable_if_t<std::is_base_of_v<PARTICLE_TYPE, OTHER_PARTICLE_TYPE>, int> = 0) :
            ParticleAccessorBase(src.GetBuffer(), src.GetCount(), src.GetParticleSize())
        {
        }
        /** constructor */
        ParticleConstAccessor(void const* in_buffer, size_t in_count, size_t in_particle_size) :
            ParticleAccessorBase(in_buffer, in_count, in_particle_size)
        {
        }
    };



    // ==============================================================
    // AutoCastedParticleConstAccessor
    // ==============================================================

    class AutoCastedParticleConstAccessor
    {
    public:

        /** constructor */
        AutoCastedParticleConstAccessor() = default;
        /** copy constructor */
        AutoCastedParticleConstAccessor(AutoCastedParticleConstAccessor const& src) = default;
        /** initialization contructor */
        AutoCastedParticleConstAccessor(class ParticleAllocationBase const* in_allocation_base) : allocation_base(in_allocation_base)
        {
            assert(in_allocation_base != nullptr);
        }
        /** copy from non const instance */
      //  AutoCastedParticleConstAccessor(AutoCastedParticleAccessor const & src) : allocation_base(src.allocation_base) {}

        /** the conversion method */
        template<typename PARTICLE_TYPE>
        operator ParticleConstAccessor<PARTICLE_TYPE>() const
        {
            return (allocation_base != nullptr) ?
                allocation_base->GetParticleConstAccessor<PARTICLE_TYPE>() :
                ParticleConstAccessor<PARTICLE_TYPE>();
        }

    protected:

        /** the allocation we are using */
        ParticleAllocationBase const * allocation_base = nullptr;
    };

    // ==============================================================
    // AutoCastedParticleAccessor
    // ==============================================================

    class AutoCastedParticleAccessor
    {
        friend class AutoCastedParticleConstAccessor;

    public:

        /** constructor */
        AutoCastedParticleAccessor() = default;
        /** copy constructor */
        AutoCastedParticleAccessor(AutoCastedParticleAccessor const& src) = default;
        /** initialization contructor */
        AutoCastedParticleAccessor(class ParticleAllocationBase* in_allocation_base) : allocation_base(in_allocation_base)
        {
            assert(in_allocation_base != nullptr);
        }
        /** the conversion method */
        template<typename PARTICLE_TYPE>
        operator ParticleAccessor<PARTICLE_TYPE>() const
        {
            return (allocation_base != nullptr) ?
                allocation_base->GetParticleAccessor<PARTICLE_TYPE>() :
                ParticleAccessor<PARTICLE_TYPE>();
        }

        operator AutoCastedParticleConstAccessor () const
        {
            return AutoCastedParticleConstAccessor(allocation_base);
        }

    protected:

        /** the allocation we are using */
        ParticleAllocationBase* allocation_base = nullptr;
    };

}; // namespace chaos

