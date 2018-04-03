#include <chaos/GPUProgramProvider.h>
#include <chaos/GLTools.h>

namespace chaos
{

  bool GPUProgramProviderTexture::DoProcessAction(char const * name, GPUProgramAction & action, GPUProgramProviderBase const * top_provider) const
  {
    if (handled_name != name)
      return false;
    return action.Process(name, value.get());
  }

  void GPUProgramProvider::Clear()
  {
    children_providers.clear();
  }

  void GPUProgramProvider::AddVariableProvider(GPUProgramProviderBase * provider)
  {
    if (provider != nullptr)
      children_providers.push_back(provider);
  }

  bool GPUProgramProvider::DoProcessAction(char const * name, GPUProgramAction & action, GPUProgramProviderBase const * top_provider) const
  {
    // handle children providers
    size_t count = children_providers.size();
    for (size_t i = 0; i < count; ++i)
      if (children_providers[i].get()->DoProcessAction(name, action, top_provider))
        return true;
    // failure
    return false;
  }
}; // namespace chaos
