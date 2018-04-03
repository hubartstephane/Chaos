#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>

namespace chaos
{

  // ==============================================================
  // CALLBACKS
  // ==============================================================

  class GPUResourceCallbacks : public ReferencedObject
  {
    friend class GPUResource;

  protected:

    /** called whenever the object is being released */
    virtual void OnReleased(GPUResource const * object);
  };

  class GPUResourceAutoCallbacks : public GPUResourceCallbacks
  {

  public:

    /** default constructor */
    GPUResourceAutoCallbacks() = default;
    /** assignation constructor */
    template<typename U>
    GPUResourceAutoCallbacks(U & in_released_func) :
      released_func(in_released_func)
    {
    }

  protected:

    /** called whenever a sound is finished */
    virtual void OnReleased(GPUResource const * object) override;

  public:

    /** the callbacks function */
    std::function<void(GPUResource const *)> released_func;
  };

  // ==============================================================
  // GPUResource
  // ==============================================================

  class GPUResource : public ReferencedObject
  {
  public:

    /** destructor */
    virtual ~GPUResource() = default;
    /** cleaning the resource */
    virtual void Release(){}

  protected:

    

  };

}; // namespace chaos
