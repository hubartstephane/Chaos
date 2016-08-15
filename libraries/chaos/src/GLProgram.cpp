#include <chaos/GLProgram.h>


namespace chaos
{

  GLProgram::GLProgram(GLuint in_id) : program_id(in_id)  
  {
    if (in_id != 0)
      program_data = GLProgramData::GetData(in_id);
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
