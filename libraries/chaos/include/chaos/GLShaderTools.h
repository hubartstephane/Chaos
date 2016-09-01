#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Buffer.h>
#include <chaos/GLProgram.h>


namespace chaos
{

/**
 * GLShaderTools : used to have some generic functions for OpenGL
 **/

class GLShaderTools
{
public:

  /** transform a program into a binary chunk (the binary_format is encoded in the buffer) */
  static Buffer<char> GetProgramBinary(GLuint program);
  /** create a program from its binary format */
  static GLuint GetProgramFromBinary(Buffer<char> const & buffer);
  /** create a program from its binary format */
  static boost::intrusive_ptr<GLProgram> GetProgramObjectFromBinary(Buffer<char> const & buffer);
  /** a utility function to display some diagnostic */
  static void DisplayProgramDiagnostic(GLuint program);
  /** a utility function to check some status for a program and display error logs */
  static GLint CheckProgramStatus(GLuint program, GLenum status, char const * format);
};

}; // namespace chaos
