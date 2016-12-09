#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>

namespace chaos
{

  class GraphicResource : public ReferencedObject
  {
  public:
    /** destructor */
    virtual ~GraphicResource() = default;
    /** cleaning the resource */
    virtual void Release(){}
  };


}; // namespace chaos
