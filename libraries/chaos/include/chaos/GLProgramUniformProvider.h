#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GLProgramData.h>

namespace chaos
{

  /**
   * GLProgramUniformProvider : used to fill GLProgram binding for uniforms
   */

class GLProgramUniformProvider
{
public:

  /** constructor */
  GLProgramUniformProvider(){}
  /** destructor */
  virtual ~GLProgramUniformProvider(){}

  /** the main method */
  virtual bool BindUniform(class GLUniformInfo const & uniform) const;

  /** remove all uniforms for binding */
  void Clear();

  /** register a uniform */
  void AddUniform(char const * name, glm::mat4  const & value) { uniform_mat4[name] = value; }
  void AddUniform(char const * name, glm::dmat4 const & value) { uniform_dmat4[name] = value; }

  void AddUniform(char const * name, glm::tvec2<GLfloat> const & value) { uniform_vec2[name] = value; }
  void AddUniform(char const * name, glm::tvec3<GLfloat> const & value) { uniform_vec3[name] = value; }
  void AddUniform(char const * name, glm::tvec4<GLfloat> const & value) { uniform_vec4[name] = value; }

  void AddUniform(char const * name, glm::tvec2<GLdouble> const & value) { uniform_dvec2[name] = value; }
  void AddUniform(char const * name, glm::tvec3<GLdouble> const & value) { uniform_dvec3[name] = value; }
  void AddUniform(char const * name, glm::tvec4<GLdouble> const & value) { uniform_dvec4[name] = value; }

  void AddUniform(char const * name, glm::tvec2<GLint> const & value) { uniform_ivec2[name] = value; }
  void AddUniform(char const * name, glm::tvec3<GLint> const & value) { uniform_ivec3[name] = value; }
  void AddUniform(char const * name, glm::tvec4<GLint> const & value) { uniform_ivec4[name] = value; }

  void AddUniform(char const * name, glm::tvec2<GLuint> const & value) { uniform_uivec2[name] = value; }
  void AddUniform(char const * name, glm::tvec3<GLuint> const & value) { uniform_uivec3[name] = value; }
  void AddUniform(char const * name, glm::tvec4<GLuint> const & value) { uniform_uivec4[name] = value; }

  void AddUniform(char const * name, GLfloat  value) { uniform_float[name] = value; }
  void AddUniform(char const * name, GLdouble value) { uniform_double[name] = value; }
  void AddUniform(char const * name, GLint    value) { uniform_int[name] = value; }
  void AddUniform(char const * name, GLuint   value) { uniform_uint[name] = value; }

protected:

  std::map<std::string, glm::mat4>            uniform_mat4;
  std::map<std::string, glm::dmat4>           uniform_dmat4;
  std::map<std::string, glm::tvec2<GLfloat>>  uniform_vec2;
  std::map<std::string, glm::tvec3<GLfloat>>  uniform_vec3;
  std::map<std::string, glm::tvec4<GLfloat>>  uniform_vec4;
  std::map<std::string, glm::tvec2<GLdouble>> uniform_dvec2;
  std::map<std::string, glm::tvec3<GLdouble>> uniform_dvec3;
  std::map<std::string, glm::tvec4<GLdouble>> uniform_dvec4;
  std::map<std::string, glm::tvec2<GLint>>    uniform_ivec2;
  std::map<std::string, glm::tvec3<GLint>>    uniform_ivec3;
  std::map<std::string, glm::tvec4<GLint>>    uniform_ivec4;
  std::map<std::string, glm::tvec2<GLuint>>   uniform_uivec2;
  std::map<std::string, glm::tvec3<GLuint>>   uniform_uivec3;
  std::map<std::string, glm::tvec4<GLuint>>   uniform_uivec4;
  std::map<std::string, GLfloat>              uniform_float;
  std::map<std::string, GLdouble>             uniform_double;
  std::map<std::string, GLint>                uniform_int;
  std::map<std::string, GLuint>               uniform_uint;
};

}; // namespace chaos
