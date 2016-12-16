#include <chaos/GLProgramUniformProvider.h>
#include <chaos/GLTools.h>

namespace chaos
{

  bool GLProgramUniformProviderTexture::BindUniform(GLUniformInfo const & uniform) const
  {
    if (uniform.name != name)
      return false;
    glBindTextureUnit(uniform.sampler_index, value->GetResourceID());
    glUniform1i(uniform.location, uniform.sampler_index);
    return true;
  }

  bool GLProgramUniformProviderChain::BindUniform(class GLUniformInfo const & uniform) const
  {
    // handle children providers
    size_t count = children_providers.size();
    for (size_t i = 0; i < count; ++i)
      if (children_providers[i]->BindUniform(uniform))
        return true;
    // handle the next provider in the chain
    if (next_provider != nullptr)
      return next_provider->BindUniform(uniform);
    // failure
    return false;
  }

}; // namespace chaos
