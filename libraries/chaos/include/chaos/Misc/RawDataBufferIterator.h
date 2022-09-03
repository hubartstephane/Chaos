namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

    template<typename TYPE, int DIRECTION>
    class RawDataBufferIteratorBase;

    template<typename TYPE>
    using RawDataBufferIterator = RawDataBufferIteratorBase<TYPE, 1>;
    template<typename TYPE>
    using RawDataBufferReverseIterator = RawDataBufferIteratorBase<TYPE, -1>;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

    // ==============================================================
     // RawDataBufferIteratorBase
     // ==============================================================

     // XXX : use CONST/NON CONST for TYPE for const_iterator/non_const_iterator

    template<typename TYPE, int DIRECTION>
    class /*CHAOS_API*/ RawDataBufferIteratorBase
    {
        using type = TYPE;

        using buffer_type = std::conditional_t<
            std::is_const_v<TYPE>, const char*, char*>;

    public:

        /** default constructor */
        RawDataBufferIteratorBase() = default;
        /** copy constructor */
        RawDataBufferIteratorBase(RawDataBufferIteratorBase const& src) = default;
        /** initialization */
        RawDataBufferIteratorBase(void const* in_buffer, size_t in_data_count, size_t in_data_size)
        {
            if (in_data_count > 0 && in_data_size > 0)
            {
                buffer_start = (buffer_type)in_buffer;
                buffer_end = buffer_start + (in_data_count - 1) * in_data_size;
                data_size = in_data_size;
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
        RawDataBufferIteratorBase& operator ++()
        {
            if (position != nullptr)
            {
                if constexpr (DIRECTION > 0)
                    position = position + data_size;
                else if constexpr (DIRECTION < 0)
                    position = position - data_size;
                CheckEndOfIterator();
            }
            return *this;
        }
        /** post-increment */
        RawDataBufferIteratorBase operator ++(int)
        {
            RawDataBufferIteratorBase result = *this;
            operator ++();
            return result;
        }
        /** pre-decrement */
        RawDataBufferIteratorBase& operator --()
        {
            if (position != nullptr)
            {
                if constexpr (DIRECTION > 0)
                    position = position - data_size;
                else if constexpr (DIRECTION < 0)
                    position = position + data_size;
                CheckEndOfIterator();
            }
            return *this;
        }
        /** post-decrement */
        RawDataBufferIteratorBase operator --(int)
        {
            RawDataBufferIteratorBase result = *this;
            operator --();
            return result;
        }
        /** increment and set */
        RawDataBufferIteratorBase& operator += (size_t count)
        {
            if (position != nullptr)
            {
                if constexpr (DIRECTION > 0)
                    position = position + count * data_size;
                else if constexpr (DIRECTION < 0)
                    position = position - count * data_size;
                CheckEndOfIterator();
            }
            return *this;
        }
        /** decrement and set */
        RawDataBufferIteratorBase& operator -= (size_t count)
        {
            if (position != nullptr)
            {
                if constexpr (DIRECTION > 0)
                    position = position - count * data_size;
                else if constexpr (DIRECTION < 0)
                    position = position + count * data_size;
                CheckEndOfIterator();
            }
            return *this;
        }
        /** comparaison */
        bool operator == (RawDataBufferIteratorBase<TYPE, DIRECTION> const& src) const
        {
            return (position == src.position);
        }
        /** comparaison */
        bool operator != (RawDataBufferIteratorBase<TYPE, DIRECTION> const& src) const
        {
            return (position != src.position);
        }

    protected:

        void CheckEndOfIterator()
        {
            if (position == nullptr) // already at the end
                return;
            if (position > buffer_end || position < buffer_start) // position is no more valid
                position = buffer_start = buffer_end = nullptr;
        }

    protected:

        /** the start of the buffer */
        buffer_type buffer_start = nullptr;
        /** the end of the buffer */
        buffer_type buffer_end = nullptr;
        /** the current buffer position */
        buffer_type position = nullptr;
        /** the real data size (not TYPE) */
        size_t data_size = 0;
    };

    // ==============================================================
    // RawDataBufferIteratorBase functions
    // ==============================================================

    template<typename TYPE, int DIRECTION>
    /*CHAOS_API*/ RawDataBufferIteratorBase<TYPE, DIRECTION> operator + (RawDataBufferIteratorBase<TYPE, DIRECTION> const& src, size_t count)
    {
        RawDataBufferIteratorBase<TYPE, DIRECTION> result = src;
        return (result += count);
    }

    template<typename TYPE, int DIRECTION>
    /*CHAOS_API*/ RawDataBufferIteratorBase<TYPE, DIRECTION> operator - (RawDataBufferIteratorBase<TYPE, DIRECTION> const& src, size_t count)
    {
        RawDataBufferIteratorBase<TYPE, DIRECTION> result = src;
        return (result -= count);
    }

#endif

}; // namespace chaos