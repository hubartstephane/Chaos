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

};

}; // namespace chaos
