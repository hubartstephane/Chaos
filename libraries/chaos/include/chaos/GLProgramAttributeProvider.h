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

  /** destructor */
  virtual ~GLProgramAttributeProvider() = default;
  /** the main method */
  virtual bool BindAttribute(class GLAttributeInfo const & attribute) const;
};

}; // namespace chaos
