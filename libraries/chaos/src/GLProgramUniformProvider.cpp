#include <chaos/GLProgramUniformProvider.h>
#include <chaos/GLTools.h>

namespace chaos
{
  bool GLProgramUniformProviderChain::BindUniform(class GLUniformInfo const & uniform) const
  {
    if (!BindUniformImpl(uniform))
      if (previous_provider != nullptr)
        return previous_provider->BindUniform(uniform);
    return false;
  }

  bool GLProgramUniformProviderChain::BindUniformImpl(class GLUniformInfo const & uniform) const
  {
    // try textures
    auto texture_it = texture_map.find(uniform.name);
    if (texture_it != texture_map.end())
      if (uniform.SetUniform(texture_it->second))
        return true;
    // try uniforms
    auto uniform_it = uniform_map.find(uniform.name);
    if (uniform_it != uniform_map.end())
      if (uniform_it->second->BindUniform(uniform))
        return true;
    // everything failed
    return false;
  }

}; // namespace chaos
