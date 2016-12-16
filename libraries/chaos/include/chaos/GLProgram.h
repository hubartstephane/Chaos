#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GraphicResource.h>
#include <chaos/GLProgramData.h>

namespace chaos
{

class GLProgram : public GraphicResource
{
public:

  /** constructor */
  GLProgram(GLuint in_id = 0);
  /** destructor */
  virtual ~GLProgram();

  /** cleaning the object */
  virtual void Release();

  /** use the program */
  bool UseProgram(class GLProgramUniformProviderChain * uniform_provider, class GLProgramAttributeProvider * attribute_provider);

  /** returns the GL name of the resource */
  GLuint GetResourceID() const { return program_id; }
  /** returns true whether the resource is valid */
  bool IsValid() const { return glIsProgram(program_id) == GL_TRUE;}
  /** get the program data */
  GLProgramData const & GetProgramData() const { return program_data; }

protected:

  /** the resource id */
  GLuint program_id;
  /** the program data */
  GLProgramData program_data;
};

}; // namespace chaos
