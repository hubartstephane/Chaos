#include <chaos/GPUResource.h>

namespace chaos
{

  void GPUResourceCallbacks::OnReleased(GPUResource const * object)
  {
    assert(object != nullptr);
  }

  void GPUResourceAutoCallbacks::OnReleased(GPUResource const * object)
  {
    assert(object != nullptr);
    if (released_func)
      released_func(object);
  }

}; // namespace chaos
