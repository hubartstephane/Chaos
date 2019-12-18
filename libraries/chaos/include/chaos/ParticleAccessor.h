#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

    // ==============================================================
    // ParticleAccessorIteratorBase
    // ==============================================================

    template<typename PARTICLE_TYPE, int DIRECTION>
    class ParticleAccessorIteratorBase
    {
        using type = PARTICLE_TYPE;

    public:

        /** default constructor */
        ParticleAccessorIteratorBase() = default;
        /** copy constructor */
        ParticleAccessorIteratorBase(ParticleAccessorIteratorBase const& src) = default;


        /** reference operator */
        PARTICLE_TYPE& operator * ()
        {
            return *(PARTICLE_TYPE*)buffer;
        }
        /** reference operator */
        PARTICLE_TYPE const& operator * () const
        {
            return *(PARTICLE_TYPE const*)buffer;
        }
        /** iterator unreferencing */
        PARTICLE_TYPE* operator -> ()
        {
            return (PARTICLE_TYPE*)buffer;

        }
        /** iterator unreferencing */
        PARTICLE_TYPE const* operator -> () const
        {
            return (PARTICLE_TYPE const*)buffer;
        }



        /** pre-increment */
        ParticleAccessorIteratorBase& operator ++()
        {
            buffer = ((char*)buffer) + DIRECTION * particle_size;
            return *this;
        }
        /** post-increment */
        ParticleAccessorIteratorBase operator ++(int)
        {
            ParticleAccessorIteratorBase result = *this;
            buffer = ((char*)buffer) + DIRECTION * particle_size;
            return result;
        }
        /** pre-decrement */
        ParticleAccessorIteratorBase& operator --()
        {
            buffer = ((char*)buffer) - DIRECTION * particle_size;
            return *this;
        }
        /** post-decrement */
        ParticleAccessorIteratorBase operator --(int)
        {
            ParticleAccessorIteratorBase result = *this;
            buffer = ((char*)buffer) - DIRECTION * particle_size;
            return result;
        }



    protected:

        /** the start of the buffer */
        void* buffer = nullptr;
#if _DEBUG
        /** the number of particles in that buffer */
        size_t count = 0;
#endif
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

        /** array accessor */
        type const & operator [](size_t index) const
        {
            assert(index < count);
            char const * b = (char const *)buffer;
            return *((type*)(b + index * particle_size));
        }

        /** array accessor */
        type & operator [](size_t index)
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
        ParticleAccessor(ParticleAccessor<OTHER_PARTICLE_TYPE> const& src, std::enable_if_t<std::is_base_of_v<OTHER_PARTICLE_TYPE, PARTICLE_TYPE>, int> = 0) :
            ParticleAccessorBase(src.GetBuffer(), src.GetCount(), src.GetParticleSize())
        {
        }
        /** constructor */
        ParticleAccessor(void* in_buffer, size_t in_count, size_t in_particle_size) :
            ParticleAccessorBase(in_buffer, in_count, in_particle_size)
        {
        }

        /** begin iterator */
        auto begin()
        {
            if (count == 0)
                return ParticleAccessorIterator<PARTICLE_TYPE>();
            return ParticleAccessorIterator<PARTICLE_TYPE>(
                buffer, 
#if _DEBUG
                count,
#endif
                particle_size);
        }

        /** end iterator */
       auto end()
        {
            if (count == 0)
                return ParticleAccessorIterator<PARTICLE_TYPE>();
            return ParticleAccessorIterator<PARTICLE_TYPE>(
                ((char *)buffer) + count * particle_size, 
#if _DEBUG
                count,
#endif                
                particle_size);
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
        ParticleConstAccessor(ParticleConstAccessor<OTHER_PARTICLE_TYPE> const& src, std::enable_if_t<std::is_base_of_v<OTHER_PARTICLE_TYPE, PARTICLE_TYPE>, int> = 0) :
            ParticleAccessorBase(src.GetBuffer(), src.GetCount(), src.GetParticleSize())
        {
        }
        /** copy from non const (+downcast operator) */
        template<typename OTHER_PARTICLE_TYPE>
        ParticleConstAccessor(ParticleAccessor<OTHER_PARTICLE_TYPE> const& src, std::enable_if_t<std::is_base_of_v<OTHER_PARTICLE_TYPE, PARTICLE_TYPE>, int> = 0) :
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

