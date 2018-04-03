#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUFileResource.h>
#include <chaos/GPUProgramData.h>

namespace chaos
{

class GPUProgram : public GPUFileResource
{
public:

  /** constructor */
  GPUProgram(GLuint in_id = 0);
  /** destructor */
  virtual ~GPUProgram();

  /** cleaning the object */
  virtual void Release() override;

  /** use the program */
  bool UseProgram(class GPUProgramProviderBase const * uniform_provider) const;

  /** returns the GL name of the resource */
  GLuint GetResourceID() const { return program_id; }
  /** returns true whether the resource is valid */
  bool IsValid() const { return glIsProgram(program_id) == GL_TRUE;}
  /** get the program data */
  GPUProgramData const & GetProgramData() const { return program_data; }

protected:

  /** the resource id */
  GLuint program_id;
  /** the program data */
  GPUProgramData program_data;
};

}; // namespace chaos
