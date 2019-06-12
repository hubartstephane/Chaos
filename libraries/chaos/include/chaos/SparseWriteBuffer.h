#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Allocators.h>

namespace chaos
{

	// ============================================================================
	// SparseWriteBuffer:
	// ============================================================================
	//   This object deserve to write data in a sparse buffer (an array of buffers)
	//   All data written in a single call are to be contigus into memory.
	//   The buffers may be concatenated into a single buffer.
	//   The buffers can be overwritten for allocation efficiency
	//   The first 4 uint of each chunked are reserved :
	//     -[0] : size of the chunk
	//     -[1] : memory already used in the chunk
	//     -[2] : pointer on the next chunk
	//     -[3] : pointer on the previous chunk
	// ============================================================================

	template<typename ALLOCATOR = ArrayAllocator<uintptr_t> >
	class SparseWriteBuffer
	{
		using alloc_type = uintptr_t;
		using allocator_type = ALLOCATOR;

		static int const BLOCK_SIZE = 0;
		static int const USED_SIZE  = 1;
		static int const NEXT_CHUNK = 2;
		static int const PREV_CHUNK = 3;

	public:

		/** constructor */
		inline SparseWriteBuffer(size_t in_default_chunk_size, allocator_type in_allocator = allocator_type()) : 
			default_chunk_size(in_default_chunk_size),
			allocator(in_allocator)
		{

		}

		/** destructor */     
		inline ~SparseWriteBuffer()
		{
			Clear(true);
		}

		/** write an object into buffer */
		template<typename T> 
		inline SparseWriteBuffer & operator << (T const & src)
		{
			Write(src);
			return *this;    
		}

		/** write an object into buffer */
		template<typename T> 
		inline void Write(T const & src)
		{
			Write(&src, sizeof(T));
		}

		/** write data into buffer */
		void Write(void const * data, size_t size)
		{
			assert(data != nullptr);
			assert(size > 0);

			// no block ever allocated. the very first allocation
			if (first_buffer == nullptr)
			{
				first_buffer = current_buffer = CreateChunkAndWrite(data, size);
				return;
			}

			// write in existing blocks
			alloc_type * block      = current_buffer;
			alloc_type * last_block = nullptr;
			while (block != nullptr && size > 0)
			{
				last_block = block;

				size_t block_size = (size_t)block[BLOCK_SIZE];
				size_t used_size  = (size_t)block[USED_SIZE];

				if (block_size == used_size)
					break;

				size_t to_write = min(size, block_size - used_size);

				DoWriteToBuffer(block, data, to_write);

				size = size - to_write;
				data = ((char *)data) + to_write;

				block = (alloc_type*)block[NEXT_CHUNK];
			}

			// allocate a last new block if necessary
			if (size > 0)
			{
				current_buffer = CreateChunkAndWrite(data, size);
				ChainBuffers(last_block, current_buffer);
			}
		}

		/** deallocate memory or reuse object */
		void Clear(bool freememory = true)
		{
			if (freememory)
			{
				alloc_type * node = first_buffer;
				while (node != nullptr)
				{
					alloc_type * next_node = (alloc_type *)node[NEXT_CHUNK];
					allocator.Free(node);
					node = next_node;
				}
				first_buffer  = nullptr;
				reserved_size = 0;
			}
			else if (first_buffer != nullptr)
			{
				alloc_type * node = first_buffer;
				while (node != nullptr)
				{
					node[USED_SIZE] = 0; 
					node = (alloc_type *)node[NEXT_CHUNK];
				}
			}

			current_buffer  = first_buffer;
			written_size    = 0;
		}

		/** iterate over data */
		template<typename FUNCTOR>
		void ProcessData(FUNCTOR & func) const
		{
			alloc_type * node = first_buffer;
			while (node != nullptr)
			{
				size_t size = (size_t)node[USED_SIZE];
				if (size == 0)
					return;
				if (!func(node + 4, node[USED_SIZE]))
					break;
				node = (alloc_type *)node[NEXT_CHUNK];
			}  
		}

		/** copy into a buffer */
		void CopyToBuffer(void * dst, size_t size) const
		{
			assert(dst != nullptr);

			class CopyFunc
			{
			public:
				CopyFunc(void * in_dst, size_t in_size) : dst(in_dst), size(in_size){}

				bool operator ()(void const * p, size_t s)
				{
					if (s > size)
						s = size;
					memcpy(dst, p, s);
					dst   = ((char*)dst) + s;
					size -= s;
					return size > 0;
				}

				void * dst;
				size_t size;
			};

			if (size > written_size)
				size = written_size;

			CopyFunc func(dst, size);
			ProcessData(func);
		}

		/** returns the number of bytes written in the buffer */
		size_t GetWrittenSize() const { return written_size; }
		/** returns the size of all memory allocated */
		size_t GetReservedSize() const { return reserved_size; }

	protected:


		/** allocate a chunk */
		alloc_type * CreateChunk(size_t wanted_size)
		{
			assert(wanted_size > 0);

			size_t final_size = wanted_size;
			if (final_size < default_chunk_size)
				final_size = default_chunk_size;
			size_t count = (final_size + sizeof(alloc_type) - 1) / sizeof(alloc_type);
			count += 4; // reserve 4 extra uint ([0] = reserved size, [1] = used count [2] = pointer upon next block   [3] = previous chunk => 4 uint are good for 16 alignment)

			size_t block_size = (count - 4) * sizeof(alloc_type);

			alloc_type * result = allocator.Allocate(count);
			result[BLOCK_SIZE] = block_size;
			result[USED_SIZE]  = 0;
			result[NEXT_CHUNK] = (alloc_type)nullptr;
			result[PREV_CHUNK] = (alloc_type)nullptr;

			reserved_size += count * sizeof(alloc_type);

			return result;
		}

		/** allocate a buffer and make it current */
		alloc_type * CreateChunkAndWrite(void const * data, size_t wanted_size)
		{
			assert(data != nullptr);
			assert(wanted_size > 0);

			alloc_type * result = CreateChunk(wanted_size);
			if (result != nullptr)
				DoWriteToBuffer(result, data, wanted_size); // the chunk is big enougth to contain all the data
			return result;
		}

		/** write data into current buffer */
		inline void DoWriteToBuffer(alloc_type * buffer, void const * data, size_t size)
		{
			assert(data != nullptr);
			assert(size > 0);

			size_t block_size = buffer[BLOCK_SIZE];
			size_t used_size  = buffer[USED_SIZE];

			assert(used_size + size <= block_size);

			memcpy(((char *)buffer) + used_size + 4 * sizeof(alloc_type), data, size);

			written_size      += size;
			buffer[USED_SIZE] += size;
		}

		/** chain 2 buffers alltogether */
		static inline void ChainBuffers(alloc_type * prev, alloc_type * next)
		{
			if (prev != nullptr)
				prev[NEXT_CHUNK] = (alloc_type)next;
			if (next != nullptr)
				next[PREV_CHUNK] = (alloc_type)prev;
		}

	protected:

		/** the first buffer of the 'linked list' */
		alloc_type * first_buffer{nullptr};
		/** the current buffer of the 'linked list' */
		alloc_type * current_buffer{nullptr};
		/** the total size of data that has been written */
		size_t       written_size{0};
		/** the total size of data that has been reserved */
		size_t       reserved_size{0};
		/** the default size of chunk used for reservation */
		size_t       default_chunk_size{0};
		/** the allocator */
		allocator_type allocator;
	};

}; // namespace chaos
