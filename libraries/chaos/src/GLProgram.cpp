#include <chaos/GLProgram.h>


namespace chaos
{

  GLProgram::GLProgram() : program_id(0)
  {
  
  }

  GLProgram::~GLProgram()
  {
    Release();
  }

  void GLProgram::Release()
  {
    if (program_id != 0)
    {
      glDeleteProgram(program_id);
      program_id = 0;    
    }  
  }
}; // namespace chaos
