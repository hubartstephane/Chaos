#include <chaos/GLProgramVariableProvider.h>
#include <chaos/GLTools.h>

namespace chaos
{

  bool GLProgramVariableProviderTexture::ProcessAction(char const * name, GLProgramVariableAction & action) const
  {
    if (handled_name != name)
      return false;
    return action.Process(name, value.get());
  }

  bool GLProgramVariableProviderChain::ProcessAction(char const * name, GLProgramVariableAction & action) const
  {
    // handle children providers
    size_t count = children_providers.size();
    for (size_t i = 0; i < count; ++i)
      if (children_providers[i]->ProcessAction(name, action))
        return true;
    // handle the next provider in the chain
    if (next_provider != nullptr)
      return next_provider->ProcessAction(name, action);
    // failure
    return false;
  }
}; // namespace chaos
