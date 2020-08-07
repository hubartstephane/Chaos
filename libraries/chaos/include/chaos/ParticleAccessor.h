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
        /** initialization */
        ParticleAccessorIteratorBase(void const * in_buffer, size_t in_particle_count, size_t in_particle_size)
        {
            if (in_particle_count > 0 && in_particle_size > 0)
            {
                buffer_start = (buffer_type)in_buffer;
                buffer_end   = buffer_start + (in_particle_count - 1) * in_particle_size;
                particle_size = in_particle_size;
                if constexpr (DIRECTION > 0)
                    position = buffer_start;
                else if constexpr (DIRECTION < 0)
                    position = buffer_end;
            }
        }

        /** reference operator */
        type& operator * () const
        {
            assert(position != nullptr);
            return *(type*)position;
        }
        /** iterator unreferencing */
        type* operator -> () const
        {
            assert(position != nullptr);
            return (type*)position;
        }
        /** pre-increment */
        ParticleAccessorIteratorBase& operator ++()
        {
            if (position != nullptr)
            {
                if constexpr (DIRECTION > 0)
                    position = position + particle_size;
                else if constexpr (DIRECTION < 0)
                    position = position - particle_size;
                CheckEndOfIterator();
            }
            return *this;
        }
        /** post-increment */
        ParticleAccessorIteratorBase operator ++(int)
        {
            ParticleAccessorIteratorBase result = *this;
            operator ++();
            return result;
        }
        /** pre-decrement */
        ParticleAccessorIteratorBase& operator --()
        {
            if (position != nullptr)
            {
                if constexpr (DIRECTION > 0)
                    position = position - particle_size;
                else if constexpr (DIRECTION < 0)
                    position = position + particle_size;
                CheckEndOfIterator();
            }
            return *this;
        }
        /** post-decrement */
        ParticleAccessorIteratorBase operator --(int)
        {
            ParticleAccessorIteratorBase result = *this;
            operator --();
            return result;
        }
        /** increment and set */
        ParticleAccessorIteratorBase& operator += (size_t count)
        {
            if (position != nullptr)
            {
                if constexpr (DIRECTION > 0)
                    position = position + count * particle_size;
                else if constexpr (DIRECTION < 0)
                    position = position - count * particle_size;
                CheckEndOfIterator();
            }
            return *this;
        }
        /** decrement and set */
        ParticleAccessorIteratorBase& operator -= (size_t count)
        {
            if (position != nullptr)
            {
                if constexpr (DIRECTION > 0)
                    position = position - count * particle_size;
                else if constexpr (DIRECTION < 0)
                    position = position + count * particle_size;
                CheckEndOfIterator();
            }
            return *this;
        }
        /** comparaison */
        bool operator == (ParticleAccessorIteratorBase<PARTICLE_TYPE, DIRECTION> const& src) const
        {
            return (position == src.position);
        }
        /** comparaison */
        bool operator != (ParticleAccessorIteratorBase<PARTICLE_TYPE, DIRECTION> const& src) const
        {
            return (position != src.position);
        }

    protected:

        void CheckEndOfIterator()
        {
            if (position == nullptr) // already at the end
                return;
            if constexpr (DIRECTION > 0)
            {
                if (position > buffer_end)
                    position = buffer_start = buffer_end = nullptr;
            }
            else if constexpr (DIRECTION < 0)
            {
                if (position < buffer_start)
                    position = buffer_start = buffer_end = nullptr;
            }
        }

    protected:

        /** the start of the buffer */
        buffer_type buffer_start = nullptr;
        /** the end of the buffer */
        buffer_type buffer_end = nullptr;
        /** the current buffer position */
        buffer_type position = nullptr;
        /** the real particle size (not PARTICLE_TYPE) */
        size_t particle_size = 0;
    };

    template<typename PARTICLE_TYPE, int DIRECTION>
    ParticleAccessorIteratorBase<PARTICLE_TYPE, DIRECTION> operator + (ParticleAccessorIteratorBase<PARTICLE_TYPE, DIRECTION> const& src, size_t count)
    {
        ParticleAccessorIteratorBase<PARTICLE_TYPE, DIRECTION> result = src;
        return (result += count);
    }

    template<typename PARTICLE_TYPE, int DIRECTION>
    ParticleAccessorIteratorBase<PARTICLE_TYPE, DIRECTION> operator - (ParticleAccessorIteratorBase<PARTICLE_TYPE, DIRECTION> const& src, size_t count)
    {
        ParticleAccessorIteratorBase<PARTICLE_TYPE, DIRECTION> result = src;
        return (result -= count);
    }

    template<typename PARTICLE_TYPE>
    using ParticleAccessorIterator = ParticleAccessorIteratorBase<PARTICLE_TYPE, 1>;
    template<typename PARTICLE_TYPE>
    using ParticleAccessorReverseIterator = ParticleAccessorIteratorBase<PARTICLE_TYPE, -1>;

		// ==============================================================
		// ParticleAccessorBase
		// ==============================================================

    template<typename PARTICLE_TYPE>
    class ParticleAccessorBase
    {
    public:

        using type = PARTICLE_TYPE;

        using buffer_type = std::conditional_t<
            std::is_const_v<type>, const void*, void*>;

        /** default constructor */
        ParticleAccessorBase() = default;
        /** copy constructor */
        ParticleAccessorBase(ParticleAccessorBase const& src) = default;

        template<typename OTHER_PARTICLE_TYPE>
        ParticleAccessorBase(ParticleAccessorBase<OTHER_PARTICLE_TYPE> const& src, std::enable_if_t<std::is_base_of_v<PARTICLE_TYPE, OTHER_PARTICLE_TYPE>, int> = 0) :
            buffer(src.GetBuffer()), particle_count(src.GetCount()), particle_size(src.GetParticleSize())
        {
            assert((particle_count > 0) ^ (buffer == nullptr));
            assert(particle_size > 0);
        }

        /** constructor */
        ParticleAccessorBase(buffer_type in_buffer, size_t in_particle_count, size_t in_particle_size) :
            buffer(in_buffer), particle_count(in_particle_count), particle_size(in_particle_size)
        {
            assert((particle_count > 0) ^ (buffer == nullptr));
            assert(particle_size > 0);
        }

        /** array accessor */
        type & operator [](size_t index) const
        {
            assert(index < particle_count);
            char const * b = (char const *)buffer;
            return *((type*)(b + index * particle_size));
        }
        /** gets the number of particle */
        size_t GetCount() const
        {
            return particle_count;
        }
        /** gets the particle size */
        size_t GetParticleSize() const
        {
            return particle_size;
        }
        /** gets the buffer */
        buffer_type GetBuffer() const
        {
            return buffer;
        }
        /** get the begining of the buffer */
        auto begin() const
        {
            if (particle_count == 0)
                return ParticleAccessorIterator<type>();
            return ParticleAccessorIterator<type>(buffer, particle_count, particle_size);
        }
        /** get the end of the buffer */
        auto end() const
        {
            return ParticleAccessorIterator<type>();
        }
        /** get the beginning of the buffer in reverse direction */
        auto rbegin() const
        {
            if (particle_count == 0)
                return ParticleAccessorReverseIterator<type>();
            return ParticleAccessorReverseIterator<type>(buffer, particle_count, particle_size);
        }
        /** get the end of the buffer in reverse direction */
        auto rend() const
        {
            return ParticleAccessorReverseIterator<type>();
        }

        /** gets the index of the particle in the accessor */
        template<typename OTHER_PARTICLE_TYPE>
        size_t GetParticleIndex(OTHER_PARTICLE_TYPE const* particle) const
        {
            assert(particle != nullptr);
            assert(particle_count != 0);
            assert((char const*)particle >= (char const*)buffer);
            assert((char const*)particle  < (char const*)buffer + particle_size * particle_count);

            char const* p = (char const*)particle;
            char const* b = (char const*)buffer;
            return (p - b) / particle_size;
        }

        /** check whether the accessor has elements */
        bool IsValid() const
        {
            return (particle_count > 0);
        }

    protected:

        /** the start of the buffer */
        buffer_type buffer = nullptr;
        /** the number of particles in that buffer */
        size_t particle_count = 0;
        /** the real particle size (not PARTICLE_TYPE) */
        size_t particle_size = 0;
    };

    template<typename PARTICLE_TYPE>
    using ParticleAccessor = ParticleAccessorBase<PARTICLE_TYPE>;
    template<typename PARTICLE_TYPE>
    using ParticleConstAccessor = ParticleAccessorBase<PARTICLE_TYPE const>;


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
        AutoCastedParticleAccessor(class ParticleAllocationBase* in_allocation_base, size_t in_start = 0, size_t in_count = 0) :
            allocation_base(in_allocation_base),
            start(in_start),
            count(in_count)
        {
            assert(in_allocation_base != nullptr);
        }
        /** the conversion method */
        template<typename PARTICLE_TYPE>
        operator ParticleAccessor<PARTICLE_TYPE>() const
        {
            return (allocation_base != nullptr) ?
                allocation_base->GetParticleAccessor<PARTICLE_TYPE>(start, count) :
                ParticleAccessor<PARTICLE_TYPE>();
        }
        /** the conversion method */
        template<typename PARTICLE_TYPE>
        operator ParticleConstAccessor<PARTICLE_TYPE>() const
        {
            return (allocation_base != nullptr) ?
                allocation_base->GetParticleConstAccessor<PARTICLE_TYPE>(start, count) :
                ParticleConstAccessor<PARTICLE_TYPE>();
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
        ParticleAllocationBase* allocation_base = nullptr;
        /** particle to start iteration on */
        size_t start = 0;
        /** number of particles concerned */
        size_t count = 0;
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
        AutoCastedParticleConstAccessor(class ParticleAllocationBase const* in_allocation_base, size_t in_start = 0, size_t in_count = 0) : 
            allocation_base(in_allocation_base),
            start(in_start),
            count(in_count)
        {
            assert(in_allocation_base != nullptr);
        }
        /** copy from non const instance */
        AutoCastedParticleConstAccessor(AutoCastedParticleAccessor const& src) : 
            allocation_base(src.allocation_base),
            start(src.start),
            count(src.count)
        {}

        /** the conversion method */
        template<typename PARTICLE_TYPE>
        operator ParticleConstAccessor<PARTICLE_TYPE>() const
        {
            return (allocation_base != nullptr) ?
                allocation_base->GetParticleConstAccessor<PARTICLE_TYPE>(start, count) :
                ParticleConstAccessor<PARTICLE_TYPE>();
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
        ParticleAllocationBase const* allocation_base = nullptr;
        /** particle to start iteration on */
        size_t start = 0;
        /** number of particles concerned */
        size_t count = 0;
    };

}; // namespace chaos

