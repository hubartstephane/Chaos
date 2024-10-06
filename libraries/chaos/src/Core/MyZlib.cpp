#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	Buffer<char> MyZLib::Encode(Buffer<char> const & src)
	{
		static constexpr int CHUNK_SIZE = 1024 * 1;

		SparseWriteBuffer<> writer(CHUNK_SIZE);

		z_stream strm;
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		strm.avail_in = 0;
		strm.next_in = Z_NULL;

		if (deflateInit(&strm, Z_DEFAULT_COMPRESSION) == Z_OK)
		{
			unsigned char chunk[CHUNK_SIZE];

			strm.avail_in = (uInt)src.bufsize;
			strm.next_in  = (unsigned char*)src.data; // prepare input

			do
			{
				strm.avail_out = CHUNK_SIZE;
				strm.next_out  = chunk;

				if (deflate(&strm, Z_FINISH) == Z_STREAM_ERROR) // the whole buffer is given so => Z_FINISH
				{
					deflateEnd(&strm);
					return Buffer<char>();
				}

				int data_to_copy = CHUNK_SIZE - strm.avail_out;
				if (data_to_copy > 0)
					writer.Write(chunk, data_to_copy);

			} while (strm.avail_out == 0); // all data in inputs consummed ?

			deflateEnd(&strm);
		}

		Buffer<char> result = SharedBufferPolicy<char>::NewBuffer(writer.GetWrittenSize());
		writer.CopyToBuffer(result.data, result.bufsize);
		return result;
	}

	Buffer<char> MyZLib::Decode(Buffer<char> const & src)
	{
		static constexpr int CHUNK_SIZE = 1024 * 1;

		SparseWriteBuffer<> writer(CHUNK_SIZE);

		z_stream strm;
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		strm.avail_in = 0;
		strm.next_in = Z_NULL;

		if (inflateInit(&strm) == Z_OK)
		{
			unsigned char chunk[CHUNK_SIZE];

			strm.avail_in = (uInt)src.bufsize;
			strm.next_in  = (unsigned char*)src.data; // prepare input

			do
			{
				strm.avail_out = CHUNK_SIZE;
				strm.next_out  = chunk;

				if (inflate(&strm, Z_NO_FLUSH) < 0) // the whole buffer is given so => Z_FINISH
				{
					char const * msg = strm.msg;
					inflateEnd(&strm);
					return Buffer<char>();
				}

				int data_to_copy = CHUNK_SIZE - strm.avail_out;
				if (data_to_copy > 0)
					writer.Write(chunk, data_to_copy);

			} while (strm.avail_out == 0); // all data in inputs consummed ?

			inflateEnd(&strm);
		}

		Buffer<char> result = SharedBufferPolicy<char>::NewBuffer(writer.GetWrittenSize());
		writer.CopyToBuffer(result.data, result.bufsize);
		return result;
	}


}; // namespace chaos
