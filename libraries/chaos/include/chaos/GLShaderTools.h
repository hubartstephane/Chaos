#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Buffer.h>
#include <chaos/GPUProgram.h>


namespace chaos
{
	namespace GLShaderTools
	{
		/** transform a program into a binary chunk (the binary_format is encoded in the buffer) */
		Buffer<char> GetProgramBinary(GLuint program);
		/** create a program from its binary format */
		GLuint GetProgramFromBinary(Buffer<char> const & buffer);
		/** create a program from its binary format */
		GPUProgram * GetProgramObjectFromBinary(Buffer<char> const & buffer);
		/** a utility function to display some diagnostic */
		void DisplayProgramDiagnostic(GLuint program);
		/** a utility function to check some status for a program and display error logs */
		GLint CheckProgramStatus(GLuint program, GLenum status, char const * format);

	}; // namespace GLShaderTools

}; // namespace chaos
