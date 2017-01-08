#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Buffer.h>

namespace chaos
{

	class BufferReader
	{
	public:

		/** constructor */
		BufferReader(Buffer<char> const & in_buffer) : buffer(in_buffer) {}
		/** test whether eof is reached */
		bool IsEOF() const { return position >= buffer.bufsize; }
		/** returns true whether there is enough data in buffer to read wanted value */
		bool IsEnoughData(size_t size) const { return (buffer.bufsize - position) >= size; }
		/** read data for one POD object */
		template<typename T>
		bool Read(T & result)
		{
			return ReadN(&result, 1);
		}
		/** read data for several POD object */
		template<typename T>
		bool ReadN(T * result, size_t count = 1)
		{
			if (!IsEnoughData(sizeof(T) * count))
				return false;
			memcpy(result, &buffer[position], sizeof(T) * count);
			position += sizeof(T) * count;
			return true;
		}
		/** get a pointer on the current buffer at position */
		char const * GetCurrentPosition() const { return &buffer.data[position]; }

		/** advance current pointer */
		void Advance(size_t offset)
		{
			position += offset;
			assert(position <= buffer.bufsize);
		}

	protected:

		/** the position in the buffer */
		size_t position{ 0 };
		/** the reference of the buffer where to stream data */
		Buffer<char> const & buffer;
	};

}; // namespace chaos

