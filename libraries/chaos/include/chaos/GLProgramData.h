#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/GLProgramData.h>
#include <chaos/GLProgramAttributeProvider.h>
#include <chaos/GLTools.h>

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
