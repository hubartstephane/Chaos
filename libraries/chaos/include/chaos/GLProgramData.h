#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/Texture.h>

namespace chaos
{

  /**
  * GLVariableInfo : a base class for uniforms and attributes
  */

  class GLVariableInfo
  {
  public:

    /** self descriptive */
    std::string name;
    /** self descriptive */
    GLint  array_size;
    /** self descriptive */
    GLint  location;
    /** self descriptive */
    GLenum type;
  };

  /**
  * GLUniformInfo : definition of an uniform in the program
  */

  class GLUniformInfo : public GLVariableInfo
  {
  public:
  
    bool SetUniform(glm::mat2x3 const & value) const;
    bool SetUniform(glm::mat2x4 const & value) const;
    bool SetUniform(glm::mat3x2 const & value) const;
    bool SetUniform(glm::mat3x4 const & value) const;
    bool SetUniform(glm::mat4x2 const & value) const;
    bool SetUniform(glm::mat4x3 const & value) const;
    bool SetUniform(glm::mat2   const & value) const;
    bool SetUniform(glm::mat3   const & value) const;
	  bool SetUniform(glm::mat4   const & value) const;

    bool SetUniform(glm::dmat2x3 const & value) const;
    bool SetUniform(glm::dmat2x4 const & value) const;
    bool SetUniform(glm::dmat3x2 const & value) const;
    bool SetUniform(glm::dmat3x4 const & value) const;
    bool SetUniform(glm::dmat4x2 const & value) const;
    bool SetUniform(glm::dmat4x3 const & value) const;
    bool SetUniform(glm::dmat2   const & value) const;
    bool SetUniform(glm::dmat3   const & value) const;
    bool SetUniform(glm::dmat4   const & value) const;

    bool SetUniform(glm::tvec1<GLfloat> const & value) const;
    bool SetUniform(glm::tvec2<GLfloat> const & value) const;
    bool SetUniform(glm::tvec3<GLfloat> const & value) const;
    bool SetUniform(glm::tvec4<GLfloat> const & value) const;

    bool SetUniform(glm::tvec1<GLdouble> const & value) const;
    bool SetUniform(glm::tvec2<GLdouble> const & value) const;
    bool SetUniform(glm::tvec3<GLdouble> const & value) const;
    bool SetUniform(glm::tvec4<GLdouble> const & value) const;

    bool SetUniform(glm::tvec1<GLint> const & value) const;
    bool SetUniform(glm::tvec2<GLint> const & value) const;
    bool SetUniform(glm::tvec3<GLint> const & value) const;
    bool SetUniform(glm::tvec4<GLint> const & value) const;

    bool SetUniform(glm::tvec1<GLuint> const & value) const;
    bool SetUniform(glm::tvec2<GLuint> const & value) const;
    bool SetUniform(glm::tvec3<GLuint> const & value) const;
    bool SetUniform(glm::tvec4<GLuint> const & value) const;

    bool SetUniform(GLfloat  value) const;
    bool SetUniform(GLdouble value) const;
    bool SetUniform(GLint    value) const;
    bool SetUniform(GLuint   value) const;

    bool SetUniform(boost::intrusive_ptr<Texture> const & texture) const;

  public:

    /** the number of the sampler in the program */
    GLuint sampler_index{0};
  };

  /**
  * GLAttributeInfo : definition of an attribute in the program
  */

class GLAttributeInfo : public GLVariableInfo
{
public:

  /** see VertexDeclaration */
  int semantic;
  /** see VertexDeclaration */
  int semantic_index;
};

  /**
   * GLProgramData : used to register attributes, uniforms in a given program
   */

class GLProgramData
{
public:

  /** generate the program data from a program */
  static GLProgramData GetData(GLuint program);

  /** display information on program */
  void DisplayProgramDiagnostic() const;

  /** get the location for a given semantic/semantic_index */
  GLint GetLocation(int semantic, int semantic_index) const;

  /** bind the attributes */
  void BindAttributes(GLuint vertex_array, VertexDeclaration const & declaration, class GLProgramAttributeProvider const * provider = nullptr) const;

  /** find a uniform */
  GLUniformInfo * FindUniform(char const * name);
  /** find an uniform */
  GLUniformInfo const * FindUniform(char const * name) const;

  /** set an uniform by its name */
  template<typename T>
  bool SetUniform(char const * name, T const & value) const
  {
    assert(name != nullptr);
    GLUniformInfo const * uniform = FindUniform(name);
    if (uniform == nullptr)
      return false;
    GLTools::SetUniform(uniform->location, value); // beware, there is no verification of data coherence
    return true;  
  }
  /** try to bind all uniforms */
  void BindUniforms(class GLProgramUniformProviderChain const * provider) const;

protected:

  /** compute the semantic and semantic index from a name */
  static std::string ExtractSemanticDataAndName(char const * attrib_name, std::pair<int, int> & semantic_data, bool & is_array);
  /** remove the '[' part from a variable name et returns if it is an array*/
  static std::string ExtractVariableName(char const * name, bool & is_array);

public:

  /** the attributes used in the program */
  std::vector<GLAttributeInfo> attributes;
  /** the uniforms used in the program */
  std::vector<GLUniformInfo> uniforms;
};

}; // namespace chaos
