#include <chaos/GLProgramUniformProvider.h>
#include <chaos/GLTools.h>

namespace chaos
{
  bool GLProgramUniformProvider::BindUniform(class GLUniformInfo const & uniform) const
  {
    if (!BindUniformImpl(uniform))
      if (previous_provider != nullptr)
        return previous_provider->BindUniform(uniform);
    return false;
  }

  bool GLProgramUniformProvider::BindUniformImpl(class GLUniformInfo const & uniform) const
  {
    auto it = uniform_map.find(uniform.name);
    if (it == uniform_map.end())
      return false;
    it->second->BindUniform(uniform);
    return true;
  }

}; // namespace chaos
