namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	namespace GLShaderTools
	{
		/** transform a program into a binary chunk (the binary_format is encoded in the buffer) */
		CHAOS_API Buffer<char> GetProgramBinary(GLuint program);
		/** create a program from its binary format */
		CHAOS_API GLuint GetProgramFromBinary(Buffer<char> const & buffer);
		/** create a program from its binary format */
		CHAOS_API GPUProgram * GetProgramObjectFromBinary(Buffer<char> const & buffer);
		/** a utility function to display some diagnostic */
		CHAOS_API void DisplayProgramDiagnostic(GLuint program);
		/** a utility function to check some status for a program and display error logs */
		CHAOS_API GLint CheckProgramStatus(GLuint program, GLenum status, char const * format);

	}; // namespace GLShaderTools

#endif

}; // namespace chaos