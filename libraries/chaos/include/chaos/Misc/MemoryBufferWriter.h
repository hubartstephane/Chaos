namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class MemoryBufferWriter;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* MemoryBufferWriter : used to write untyped data in a buffer
	*/

	class CHAOS_API MemoryBufferWriter
	{
	public:

		/** default constructor */
		MemoryBufferWriter() = default;
		/** copy constructor */
		MemoryBufferWriter(MemoryBufferWriter const& src) = default;

		/** constructor */
		MemoryBufferWriter(void* in_buffer, size_t in_bufsize) :
			buffer(in_buffer),
			bufsize(in_bufsize),
			position(in_buffer) {}

		/** get the size of the buffer */
		size_t GetBufferSize() const { return bufsize; }
		/** get the remaining size of the buffer */
		size_t GetRemainingBufferSize() const
		{
			return bufsize - GetWrittenCount();
		}

		/** insert data inside the buffer */
		template<typename T>
		MemoryBufferWriter& operator << (T const& data)
		{
			Write(&data, sizeof(T));
			return *this;
		}

		/** insert raw data inside buffer */
		void Write(void const* data, size_t size)
		{
			assert(buffer != nullptr);
			assert(GetRemainingBufferSize() >= size);

			if (data != nullptr)
				memcpy(position, data, size);
			position = ((char*)position) + size;
		}

		/** returns the number of bytes written */
		size_t GetWrittenCount() const
		{
			return (char*)position - (char*)buffer;
		}

	protected:

		/** the buffer where writing is done */
		void* buffer = nullptr;
		/** the size of the buffer */
		size_t bufsize = 0;
		/** the current position in the buffer */
		void* position = nullptr;
	};

#endif

}; // namespace chaos