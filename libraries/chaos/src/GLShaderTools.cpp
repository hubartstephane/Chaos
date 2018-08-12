#include <chaos/GLShaderTools.h>
#include <chaos/LogTools.h>
#include <chaos/GPUProgramData.h>
#include <chaos/Buffer.h>


namespace chaos
{

	Buffer<char> GLShaderTools::GetProgramBinary(GLuint program)
	{
		Buffer<char> result;

		GLenum binary_format = GL_NONE;

		// get the size of the binary chunk
		GLsizei binary_size = 0; 
		glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &binary_size);

		// allocate the buffer and get the binary (the format is a prefix in the buffer)
		if (binary_size > 0)
		{
			result = SharedBufferPolicy<char>::NewBuffer(binary_size + sizeof(GLenum));
			if (result != nullptr)
			{      
				glGetProgramBinary(program, binary_size, &binary_size, &binary_format, result.data + sizeof(GLenum));
				memcpy(result.data, &binary_format, sizeof(GLenum));
			}
		}
		return result;
	}

	GLuint GLShaderTools::GetProgramFromBinary(Buffer<char> const & buffer)
	{
		if (buffer.bufsize < sizeof(GLenum))
			return 0;

		GLuint result = glCreateProgram();
		if (result != 0)
		{
			GLenum binary_format = GL_NONE;
			memcpy(&binary_format, buffer.data, sizeof(GLenum));

			glProgramBinary(result, binary_format, buffer.data + sizeof(GLenum), (GLsizei)buffer.bufsize - sizeof(GLenum));
			if (CheckProgramStatus(result, GL_LINK_STATUS, "Program from binary failure") != GL_TRUE)
				glDeleteProgram(result);
		}
		return result;
	}

	GPUProgram * GLShaderTools::GetProgramObjectFromBinary(Buffer<char> const & buffer)
	{
		GLuint program_id = GetProgramFromBinary(buffer);
		return (program_id == 0)? nullptr : new GPUProgram(program_id);
	}

	GLint GLShaderTools::CheckProgramStatus(GLuint program, GLenum status, char const * format)
	{
		GLint result = 0;
		glGetProgramiv(program, status, &result);
		if (result == GL_FALSE)
		{
			GLint log_len = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);
			if (log_len > 0)
			{
				if (format == nullptr)
					format = "%s";

				GLchar log_buffer[2048];
				log_buffer[0] = 0;
				glGetProgramInfoLog(program, sizeof(log_buffer) - 1, nullptr, log_buffer);
				LogTools::Error(format, log_buffer);
			}
		}
		return result;
	}

	void GLShaderTools::DisplayProgramDiagnostic(GLuint program)
	{
		GPUProgramData::GetData(program).DisplayProgramDiagnostic();
	}

}; // namespace chaos
