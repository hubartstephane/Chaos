#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GLProgramData.h>

namespace chaos
{

  /**
   * GLProgramAttributeProvider : used to fill GLProgram binding for unknown semantic
   */

class GLProgramAttributeProvider
{
public:

  /** constructor */
  GLProgramAttributeProvider(){}
  /** destructor */
  virtual ~GLProgramAttributeProvider(){}

  /** the main method */
  virtual bool BindAttribute(class GLAttributeInfo const & attribute) const;

};

}; // namespace chaos
