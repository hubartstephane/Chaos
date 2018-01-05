#include <chaos/GPUProgramVariableProvider.h>
#include <chaos/GLTools.h>

namespace chaos
{

  bool GPUProgramVariableProviderTexture::DoProcessAction(char const * name, GPUProgramVariableAction & action, GPUProgramVariableProvider const * top_provider) const
  {
    if (handled_name != name)
      return false;
    return action.Process(name, value.get());
  }

  void GPUProgramVariableProviderChain::Clear()
  {
    children_providers.clear();
  }

  void GPUProgramVariableProviderChain::AddVariableProvider(GPUProgramVariableProvider * provider)
  {
    if (provider != nullptr)
      children_providers.push_back(provider);
  }

  bool GPUProgramVariableProviderChain::DoProcessAction(char const * name, GPUProgramVariableAction & action, GPUProgramVariableProvider const * top_provider) const
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
