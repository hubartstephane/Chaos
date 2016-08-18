#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
  /**
   * MemoryBufferWriter : used to write untyped data in a buffer
   */

  class MemoryBufferWriter
  {
  public:

    /** default constructor */
    MemoryBufferWriter():
      buffer(nullptr),
      bufsize(0),
      position(nullptr){}

    /** constructor */
    MemoryBufferWriter(void * in_buffer, size_t in_bufsize):
      buffer(in_buffer),
      bufsize(in_bufsize),
      position(in_buffer){}

    /** copy constructor */
    MemoryBufferWriter(MemoryBufferWriter const & src):
      buffer(src.buffer),
      bufsize(src.bufsize),
      position(src.position){}

    /** get the size of the buffer */
    size_t GetBufferSize() const { return bufsize; }
    /** get the remaining size of the buffer */
    size_t GetRemainingBufferSize() const
    {
      return bufsize - ((char*)position - (char*)buffer);    
    }

    /** insert data inside the buffer */
    template<typename T>
    MemoryBufferWriter & operator << (T const & data)
    {
      Write(&data, sizeof(T));
      return *this;
    }

    /** insert raw data inside buffer */
    void Write(void const * data, size_t size)
    {
      assert(buffer != nullptr);
      assert(GetRemainingBufferSize() >= size);
      memcpy(buffer, &data, size);
      position = ((char *)position) + size;    
    }
  
    /** the buffer where writing is done */
    void * buffer;
    /** the size of the buffer */
    size_t bufsize;
    /** the current position in the buffer */
    void * position;    
  };
}; // namespace chaos
