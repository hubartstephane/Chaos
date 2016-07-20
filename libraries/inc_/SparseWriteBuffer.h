#ifndef CHAOS_SPARSEWRITEBUFFER_H
#define CHAOS_SPARSEWRITEBUFFER_H

#include <cstdint>
#include <assert.h>
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
  typedef uintptr_t alloc_type;
  typedef ALLOCATOR allocator_type;

  static int const BLOCK_SIZE = 0;
  static int const USED_SIZE  = 1;
  static int const NEXT_CHUNK = 2;
  static int const PREV_CHUNK = 3;

public:

  /** constructor */
  inline SparseWriteBuffer(size_t in_default_chunk_size, allocator_type in_allocator = allocator_type()) : 
    first_buffer(NULL),       
    current_buffer(NULL), 
    written_size(0), 
    reserved_size(0), 
    current_written(0), 
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
    assert(data != NULL);
    assert(size > 0);

    // no block ever allocated. the very first allocation
    if (first_buffer == NULL)
    {
      first_buffer = current_buffer = CreateChunkAndWrite(data, size);
    }
    else 
    {
      // block large enough to receive whole data
      if (current_written + size < current_size)
        DoWriteToBuffer(data, size);
      else 
      {
        alloc_type * old  = current_buffer;
        alloc_type * tmp  = (alloc_type *)current_buffer[NEXT_CHUNK];
        // try to find an existing buffer large enough to contains all data
        while ((tmp != NULL) && (tmp[BLOCK_SIZE] < size))
          tmp  = (alloc_type *)tmp[NEXT_CHUNK];

        // no block found large enough for data, must allocate one
        if (tmp == NULL) 
          tmp = CreateChunkAndWrite(data, size);
        else
        {
          ChainBuffers((alloc_type *)tmp[PREV_CHUNK], (alloc_type *)tmp[NEXT_CHUNK]); // extract tmp from its list
          
          current_buffer = tmp;
          current_buffer[USED_SIZE] = 0; // lazy reinitialization
          DoWriteToBuffer(data, size);
        }

        // select if tmp must be before or after old, depending on the usage of any data in old buffer
        if (old[USED_SIZE] == 0) 
        {
          ChainBuffers((alloc_type *)old[PREV_CHUNK], tmp);
          ChainBuffers(tmp, old);
        }
        else
        {
          ChainBuffers(tmp, (alloc_type *)old[NEXT_CHUNK]);
          ChainBuffers(old, tmp);
        }
      }
    }    
  }

  /** deallocate memory or reuse object */
  void Clear(bool freememory = true)
  {
      if (freememory)
      {
        alloc_type * node = first_buffer;
        while (node != NULL)
        {
          alloc_type * next_node = (alloc_type *)node[NEXT_CHUNK];
          allocator.Free(node);
          node = next_node;
        }
        first_buffer  = NULL;
        reserved_size = 0;
      }
      else if (first_buffer != NULL)
      {
        first_buffer[USED_SIZE] = 0; // only USED_SIZE for the very first CHUNK is ok (beware, lazy reinitialization for the others)
      }

      current_buffer  = first_buffer;
      written_size    = 0;
      current_written = 0;
  }

  /** iterate over data */
  template<typename FUNCTOR>
  void ProcessData(FUNCTOR & func) const
  {
    alloc_type * node = first_buffer;
    while (node != NULL)
    {
      if (!func(node + 4, node[USED_SIZE]))
        break;
      node = (alloc_type *)node[NEXT_CHUNK];
    }
  }

  /** copy into a buffer */
  void CopyToBuffer(void * dst, size_t size) const
  {
    assert(dst != NULL);

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

    CopyFunc func(dst, size);
    ProcessData(func);
  }

  /** returns the number of bytes written in the buffer */
  size_t GetWrittenSize() const { return written_size; }
  /** returns the size of all memory allocated */
  size_t GetReservedSize() const { return reserved_size; }

protected:
  /** allocate a buffer and make it current */
  alloc_type * CreateChunkAndWrite(void const * data, size_t wanted_size)
  {
    assert(data != NULL);
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
    result[NEXT_CHUNK] = (alloc_type)NULL;
    result[PREV_CHUNK] = (alloc_type)NULL;

    reserved_size += count * sizeof(alloc_type);

    // whenever a chunk is allocated, it becomes the current one
    current_buffer  = result;
    current_size    = block_size;
    current_written = 0;

    DoWriteToBuffer(data, wanted_size);

    return result;
  }

  /** write data into current buffer */
  inline void DoWriteToBuffer(void const * data, size_t size)
  {
    assert(data != NULL);
    assert(size > 0);
    assert(current_written + size <= current_size);
    memcpy(((char *)current_buffer) + current_written + 4 * sizeof(alloc_type), data, size);
    current_written += size;
    written_size    += size;
    current_buffer[USED_SIZE] += size;
  }

  /** chain 2 buffers alltogether */
  inline void ChainBuffers(alloc_type * a, alloc_type * b)
  {
    if (a != NULL)
      a[NEXT_CHUNK] = (alloc_type)b;
    else
      first_buffer = b;
    if (b != NULL)
      b[PREV_CHUNK] = (alloc_type)a;
  }
protected:

    /** the first buffer of the 'linked list' */
    alloc_type * first_buffer;
    /** the current buffer of the 'linked list' */
    alloc_type * current_buffer;    
    /** the total size of data that has been written */
    size_t       written_size;
    /** the total size of data that has been reserved */
    size_t       reserved_size;
    /** the reserved size of the current chunk (avoid cache miss) */
    size_t       current_size;
    /** the written size of the current chunk (avoid cache miss) */
    size_t       current_written;   
    /** the default size of chunk used for reservation */
    size_t       default_chunk_size;
    /** the allocator */
    allocator_type allocator;
};

}; // namespace chaos

#endif // CHAOS_SPARSEWRITEBUFFER_H