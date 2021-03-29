#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
    template<typename TYPE>
    class RawDataBufferAccessorBase;

    template<typename TYPE>
    using RawDataBufferAccessor = RawDataBufferAccessorBase<TYPE>;
    template<typename TYPE>
    using RawDataBufferConstAccessor = RawDataBufferAccessorBase<TYPE const>;

}; // namespace chaos

#else 

namespace chaos
{

	// ==============================================================
	// RawDataBufferAccessorBase
	// ==============================================================

    template<typename TYPE>
    class RawDataBufferAccessorBase
    {
    public:

        using type = TYPE;

        using buffer_type = std::conditional_t<
            std::is_const_v<type>, const void*, void*>;

        /** default constructor */
        RawDataBufferAccessorBase() = default;
        /** copy constructor */
        RawDataBufferAccessorBase(RawDataBufferAccessorBase const& src) = default;

        template<typename OTHER_TYPE>
        RawDataBufferAccessorBase(RawDataBufferAccessorBase<OTHER_TYPE> const& src, std::enable_if_t<std::is_base_of_v<TYPE, OTHER_TYPE>, int> = 0) :
            buffer(src.GetBuffer()), data_count(src.GetDataCount()), data_size(src.GetDataSize())
        {
            assert((data_count > 0) ^ (buffer == nullptr));
            assert(data_size > 0);
        }

        /** constructor */
        RawDataBufferAccessorBase(buffer_type in_buffer, size_t in_data_count, size_t in_data_size) :
            buffer(in_buffer), data_count(in_data_count), data_size(in_data_size)
        {
            assert((data_count > 0) ^ (buffer == nullptr));
            if (data_size == 0)
                data_size = sizeof(TYPE);
        }

        /** array accessor */
        type & operator [](size_t index) const
        {
            assert(index < data_count);
            char const * b = (char const *)buffer;
            return *((type*)(b + index * data_size));
        }
        /** gets the number of objects */
        size_t GetDataCount() const
        {
            return data_count;
        }
        /** gets the object size */
        size_t GetDataSize() const
        {
            return data_size;
        }
        /** gets the buffer */
        buffer_type GetBuffer() const
        {
            return buffer;
        }
        /** get the begining of the buffer */
        auto begin() const
        {
            if (data_count == 0)
                return RawDataBufferIterator<type>();
            return RawDataBufferIterator<type>(buffer, data_count, data_size);
        }
        /** get the end of the buffer */
        auto end() const
        {
            return RawDataBufferIterator<type>();
        }
        /** get the beginning of the buffer in reverse direction */
        auto rbegin() const
        {
            if (data_count == 0)
                return RawDataBufferReverseIterator<type>();
            return RawDataBufferReverseIterator<type>(buffer, data_count, data_size);
        }
        /** get the end of the buffer in reverse direction */
        auto rend() const
        {
            return RawDataBufferReverseIterator<type>();
        }

        /** gets the index of the particle in the accessor */
        template<typename OTHER_TYPE>
        size_t GetDataIndex(OTHER_TYPE const* data) const
        {
            assert(data != nullptr);
            assert(data_count != 0);
            assert((char const*)data >= (char const*)buffer);
            assert((char const*)data < (char const*)buffer + data_size * data_count);

            char const* p = (char const*)data;
            char const* b = (char const*)buffer;
            return (p - b) / data_size;
        }

        /** check whether the accessor has elements */
        bool IsValid() const
        {
            return (data_count > 0);
        }

    protected:

        /** the start of the buffer */
        buffer_type buffer = nullptr;
        /** the number of object in that buffer */
        size_t data_count = 0;
        /** the real object size (not TYPE) */
        size_t data_size = 0;
    };

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION



