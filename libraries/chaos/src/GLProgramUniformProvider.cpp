#include <chaos/GLProgramUniformProvider.h>
#include <chaos/GLTools.h>

namespace chaos
{

  void GLProgramUniformProvider::Clear()
  {
    uniform_mat4.clear();
    uniform_dmat4.clear();
    uniform_vec2.clear();
    uniform_vec3.clear();
    uniform_vec4.clear();
    uniform_dvec2.clear();
    uniform_dvec3.clear();
    uniform_dvec4.clear();
    uniform_ivec2.clear();
    uniform_ivec3.clear();
    uniform_ivec4.clear();
    uniform_uivec2.clear();
    uniform_uivec3.clear();
    uniform_uivec4.clear();
    uniform_float.clear();
    uniform_double.clear();
    uniform_int.clear();
    uniform_uint.clear();
  }

  template<typename T>
  static bool BindUniformImpl(GLUniformInfo const & uniform, std::map<std::string, T> const & uniform_map)
  {
    auto it = uniform_map.find(uniform.name);
    if (it == uniform_map.end())
      return false;
    GLTools::SetUniform(uniform.location, it->second);
    return true;
  }

  bool GLProgramUniformProvider::BindUniform(GLUniformInfo const & uniform) const
  {
    if (BindUniformImpl(uniform, uniform_mat4))
      return true;
    if (BindUniformImpl(uniform, uniform_dmat4))
      return true;
    if (BindUniformImpl(uniform, uniform_vec2))
      return true;
    if (BindUniformImpl(uniform, uniform_vec3))
      return true;
    if (BindUniformImpl(uniform, uniform_vec4))
      return true;
    if (BindUniformImpl(uniform, uniform_dvec2))
      return true;
    if (BindUniformImpl(uniform, uniform_dvec3))
      return true;
    if (BindUniformImpl(uniform, uniform_dvec4))
      return true;
    if (BindUniformImpl(uniform, uniform_ivec2))
      return true;
    if (BindUniformImpl(uniform, uniform_ivec3))
      return true;
    if (BindUniformImpl(uniform, uniform_ivec4))
      return true;
    if (BindUniformImpl(uniform, uniform_uivec2))
      return true;
    if (BindUniformImpl(uniform, uniform_uivec3))
      return true;
    if (BindUniformImpl(uniform, uniform_uivec4))
      return true;
    if (BindUniformImpl(uniform, uniform_float))
      return true;
    if (BindUniformImpl(uniform, uniform_double))
      return true;
    if (BindUniformImpl(uniform, uniform_int))
      return true;
    if (BindUniformImpl(uniform, uniform_uint))
      return true;   
    return false;
  }

}; // namespace chaos
