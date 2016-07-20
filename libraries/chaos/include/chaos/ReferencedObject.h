#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

/**
 * ReferencedObject is a base class that have a reference count
 */

class ReferencedObject
{
public:

  /** default constructor */  
  ReferencedObject() : reference_count(1){}

  /** copy constructor */  
  ReferencedObject(ReferencedObject const & src) : reference_count(1){}

  /** destructor */
  virtual ~ReferencedObject(){ assert(reference_count == 0);}

  /** copy operator (do not copy the reference count) */
  ReferencedObject & operator = (ReferencedObject const & src){ return *this; }

  /** Increment the reference count */
  void AddReference(){++reference_count;}

  /** Decrement the reference count */
  void SubReference()
  {
    if (--reference_count == 0)
      OnLastReferenceLost();
  }

protected:

    /** called whenever there are no more reference on the object */
    virtual void OnLastReferenceLost(){ delete(this); }

    /** the reference count of the object */
    boost::atomic<int> reference_count;
};

}; // namespace chaos

