#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>

namespace chaos
{

  class GPUResource : public ReferencedObject
  {
  public:
    /** destructor */
    virtual ~GPUResource() = default;
    /** cleaning the resource */
    virtual void Release(){}
  };

}; // namespace chaos
