#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

    // ==============================================================
    // ParticleAccessorIteratorBase
    // ==============================================================

    // XXX : use CONST/NON CONST for PARTICLE_TYPE for const_iterator/non_const_iterator

    template<typename PARTICLE_TYPE, int DIRECTION>
    class ParticleAccessorIteratorBase
    {
        using type = PARTICLE_TYPE;

        using buffer_type = std::conditional_t<
            std::is_const_v<PARTICLE_TYPE>, const char*, char*>;

    public:

        /** default constructor */
        ParticleAccessorIteratorBase() = default;
        /** copy constructor */
        ParticleAccessorIteratorBase(ParticleAccessorIteratorBase const& src) = default;

        /** reference operator */
        type& operator * () const
        {
            return *(type*)buffer;
        }
        /** iterator unreferencing */
        type* operator -> () const
        {
            return (type*)buffer;

        }
        /** pre-increment */
        ParticleAccessorIteratorBase& operator ++()
        {
            if constexpr (DIRECTION > 0)
                buffer = buffer + particle_size;
            else if constexpr (DIRECTION < 0)
                buffer = buffer - particle_size;
            return *this;
        }
        /** post-increment */
        ParticleAccessorIteratorBase operator ++(int)
        {
            ParticleAccessorIteratorBase result = *this;
            if constexpr (DIRECTION > 0)
                buffer = buffer + particle_size;
            else if constexpr (DIRECTION < 0)
                buffer = buffer - particle_size;
            return result;
        }
        /** pre-decrement */
        ParticleAccessorIteratorBase& operator --()
        {
            if constexpr (DIRECTION > 0)
                buffer = buffer - particle_size;
            else if constexpr (DIRECTION < 0)
                buffer = buffer + particle_size;
            return *this;
        }
        /** post-decrement */
        ParticleAccessorIteratorBase operator --(int)
        {
            ParticleAccessorIteratorBase result = *this;
            if constexpr (DIRECTION > 0)
                buffer = buffer - particle_size;
            else if constexpr (DIRECTION < 0)
                buffer = buffer + particle_size;
            return result;
        }
        /** increment and set */
        ParticleAccessorIteratorBase& operator += (size_t count)
        {
            if constexpr (DIRECTION > 0)
                buffer = buffer + count * particle_size;
            else if constexpr (DIRECTION < 0)
                buffer = buffer - count * particle_size;
            return *this;
        }
        /** decrement and set */
        ParticleAccessorIteratorBase& operator -= (size_t count)
        {
            if constexpr (DIRECTION > 0)
                buffer = buffer - count * particle_size;
            else if constexpr (DIRECTION < 0)
                buffer = buffer + count * particle_size;
            return *this;
        }

    protected:

        /** the start of the buffer */
        buffer_type buffer = nullptr;
        /** the real particle size (not PARTICLE_TYPE) */
        size_t particle_size = 0;
    };

    template<typename PARTICLE_TYPE, int DIRECTION>
    bool operator == (ParticleAccessorIteratorBase<PARTICLE_TYPE, DIRECTION> const& src1, ParticleAccessorIteratorBase<PARTICLE_TYPE, DIRECTION> const& src2)
    {
        return (src1.buffer == src2.buffer);
    }

    template<typename PARTICLE_TYPE, int DIRECTION>
    bool operator != (ParticleAccessorIteratorBase<PARTICLE_TYPE, DIRECTION> const& src1, ParticleAccessorIteratorBase<PARTICLE_TYPE, DIRECTION> const& src2)
    {
        return !(src1 == src2);
    }

    template<typename PARTICLE_TYPE, int DIRECTION>
    ParticleAccessorIteratorBase<PARTICLE_TYPE, DIRECTION> operator + (ParticleAccessorIteratorBase<PARTICLE_TYPE, DIRECTION> const& src, size_t count)
    {
        ParticleAccessorIteratorBase<PARTICLE_TYPE, DIRECTION> result = src;
        return src += count;
    }

    template<typename PARTICLE_TYPE, int DIRECTION>
    ParticleAccessorIteratorBase<PARTICLE_TYPE, DIRECTION> operator - (ParticleAccessorIteratorBase<PARTICLE_TYPE, DIRECTION> const& src, size_t count)
    {
        ParticleAccessorIteratorBase<PARTICLE_TYPE, DIRECTION> result = src;
        return src -= count;
    }

    template<typename PARTICLE_TYPE>
    using ParticleAccessorIterator = ParticleAccessorIteratorBase<PARTICLE_TYPE, 1>;
    template<typename PARTICLE_TYPE>
    using ParticleAccessorReverseIterator = ParticleAccessorIteratorBase<PARTICLE_TYPE, -1>;
    template<typename PARTICLE_TYPE>
    using ParticleAccessorConstIterator = ParticleAccessorIteratorBase<PARTICLE_TYPE const, 1>;
    template<typename PARTICLE_TYPE>
    using ParticleAccessorConstReverseIterator = ParticleAccessorIteratorBase<PARTICLE_TYPE const, -1>;


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
        ParticleAccessorBase(BUFFER_TYPE in_buffer, size_t in_count, size_t in_particle_size) :
            buffer(in_buffer), count(in_count), particle_size(in_particle_size)
        {
            assert((count > 0) ^ (buffer == nullptr));
        }

        /** array accessor */
        type & operator [](size_t index) const
        {
            assert(index < count);
            char const * b = (char const *)buffer;
            return *((type*)(b + index * particle_size));
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
        BUFFER_TYPE buffer = nullptr;
        /** the number of particles in that buffer */
        size_t count = 0;
        /** the real particle size (not PARTICLE_TYPE) */
        size_t particle_size = 0;
    };

    template<typename PARTICLE_TYPE>
    using ParticleAccessor = ParticleAccessorBase<PARTICLE_TYPE, void *>;
    template<typename PARTICLE_TYPE>
    using ParticleConstAccessor = ParticleAccessorBase<PARTICLE_TYPE const, void const *>;


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
        ParticleAllocationBase* allocation_base = nullptr;
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
        AutoCastedParticleConstAccessor(AutoCastedParticleAccessor const& src) : allocation_base(src.allocation_base) {}

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
        ParticleAllocationBase const* allocation_base = nullptr;
    };



}; // namespace chaos

