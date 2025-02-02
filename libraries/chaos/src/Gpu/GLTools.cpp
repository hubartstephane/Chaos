#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	namespace GlobalVariables
	{
		CHAOS_GLOBAL_VARIABLE(bool, GLDebugNotifications, false);
#if _WIN32
		CHAOS_GLOBAL_VARIABLE(bool, GLDebugBreak, false);
#endif
	};

	void GLTools::SetViewport(aabox2 const& viewport)
	{
		glViewport(GLint(viewport.position.x), GLint(viewport.position.y), GLint(viewport.size.x), GLint(viewport.size.y));
	}

	void GLTools::SetScissorBox(aabox2 const& scissor)
	{
		glScissor(GLint(scissor.position.x), GLint(scissor.position.y), GLint(scissor.size.x), GLint(scissor.size.y));
	}

	bool GLTools::IsScalarType(GLenum type)
	{
		GLenum const scalar_types[] = { 
			GL_BOOL, GL_FLOAT, GL_DOUBLE, GL_INT, GL_UNSIGNED_INT,
			GL_NONE
		};

		for (int i = 0; scalar_types[i] != GL_NONE; ++i)
			if (type == scalar_types[i])
				return true;
		return false;
	}

	bool GLTools::IsVectorType(GLenum type)
	{
		GLenum const vector_types[] = {
			GL_BOOL_VEC2, GL_FLOAT_VEC2, GL_DOUBLE_VEC2, GL_INT_VEC2, GL_UNSIGNED_INT_VEC2,
			GL_BOOL_VEC3, GL_FLOAT_VEC3, GL_DOUBLE_VEC3, GL_INT_VEC3, GL_UNSIGNED_INT_VEC3,
			GL_BOOL_VEC4, GL_FLOAT_VEC4, GL_DOUBLE_VEC4, GL_INT_VEC4, GL_UNSIGNED_INT_VEC4,
			GL_NONE
		};

		for (int i = 0; vector_types[i] != GL_NONE; ++i)
			if (type == vector_types[i])
				return true;
		return false;
	}

	bool GLTools::IsMatrixType(GLenum type)
	{		
		GLenum const matrix_types[] = {
			GL_FLOAT_MAT2, GL_FLOAT_MAT3, GL_FLOAT_MAT4,
			GL_FLOAT_MAT2x3, GL_FLOAT_MAT2x4, GL_FLOAT_MAT3x2, GL_FLOAT_MAT3x4, GL_FLOAT_MAT4x2, GL_FLOAT_MAT4x3,
			GL_DOUBLE_MAT2, GL_DOUBLE_MAT3, GL_DOUBLE_MAT4,
			GL_DOUBLE_MAT2x3, GL_DOUBLE_MAT2x4, GL_DOUBLE_MAT3x2, GL_DOUBLE_MAT3x4, GL_DOUBLE_MAT4x2, GL_DOUBLE_MAT4x3,
			GL_NONE
		};

		for (int i = 0; matrix_types[i] != GL_NONE; ++i)
			if (type == matrix_types[i])
				return true;
		return false;
	}

	bool GLTools::IsSamplerType(GLenum type)
	{
		// XXX : samplers type, maybe some error or some missing elements
		GLenum const sampler_types[] = {
			GL_SAMPLER_1D, GL_SAMPLER_2D, GL_SAMPLER_3D, GL_SAMPLER_CUBE,
			GL_SAMPLER_1D_SHADOW, GL_SAMPLER_2D_SHADOW, GL_SAMPLER_1D_ARRAY,
			GL_SAMPLER_2D_ARRAY, GL_SAMPLER_1D_ARRAY_SHADOW, GL_SAMPLER_2D_ARRAY_SHADOW, GL_SAMPLER_CUBE_SHADOW,
			GL_INT_SAMPLER_1D, GL_INT_SAMPLER_2D, GL_INT_SAMPLER_3D, GL_INT_SAMPLER_CUBE, GL_INT_SAMPLER_1D_ARRAY,
			GL_INT_SAMPLER_2D_ARRAY, GL_UNSIGNED_INT_SAMPLER_1D, GL_UNSIGNED_INT_SAMPLER_2D, GL_UNSIGNED_INT_SAMPLER_3D,
			GL_UNSIGNED_INT_SAMPLER_CUBE, GL_UNSIGNED_INT_SAMPLER_1D_ARRAY, GL_UNSIGNED_INT_SAMPLER_2D_ARRAY,
			GL_SAMPLER_2D_RECT, GL_SAMPLER_2D_RECT_SHADOW, GL_SAMPLER_CUBE_MAP_ARRAY, GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW, GL_NONE
		};

		for (int i = 0; sampler_types[i] != GL_NONE; ++i)
			if (type == sampler_types[i])
				return true;
		return false;
	}

	void GLTools::DisplayGenericInformation()
	{
		GLLog::BeginTransaction(LogSeverity::Message);

		GLubyte const * renderer = glGetString(GL_RENDERER);
		if (renderer != nullptr)
			GLLog::TransactionConcatLN("OpenGL RENDERER        : %s", renderer);

		GLubyte const * vendor = glGetString(GL_VENDOR);
		if (vendor != nullptr)
			GLLog::TransactionConcatLN("OpenGL VENDOR          : %s", vendor);

		GLubyte const * version = glGetString(GL_VERSION);
		if (version != nullptr)
			GLLog::TransactionConcatLN("OpenGL VERSION         : %s", version);

		GLubyte const * shading_version = glGetString(GL_SHADING_LANGUAGE_VERSION);
		if (shading_version != nullptr)
			GLLog::TransactionConcatLN("OpenGL SHADING LANGAGE : %s", shading_version);

		GLLog::EndTransaction();
	}

	char const * GLTools::GLenumToString(GLenum type)
	{
#define CHAOS_GLENUM_TO_STRING(x) if (type == x) return #x
		CHAOS_GLENUM_TO_STRING(GL_BYTE);
		CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_BYTE);
		CHAOS_GLENUM_TO_STRING(GL_SHORT);
		CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_SHORT);
		CHAOS_GLENUM_TO_STRING(GL_INT);
		CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_INT);
		CHAOS_GLENUM_TO_STRING(GL_FLOAT);
		CHAOS_GLENUM_TO_STRING(GL_2_BYTES);
		CHAOS_GLENUM_TO_STRING(GL_3_BYTES);
		CHAOS_GLENUM_TO_STRING(GL_4_BYTES);
		CHAOS_GLENUM_TO_STRING(GL_DOUBLE);
		CHAOS_GLENUM_TO_STRING(GL_FLOAT_VEC2);
		CHAOS_GLENUM_TO_STRING(GL_FLOAT_VEC3);
		CHAOS_GLENUM_TO_STRING(GL_FLOAT_VEC4);
		CHAOS_GLENUM_TO_STRING(GL_INT_VEC2);
		CHAOS_GLENUM_TO_STRING(GL_INT_VEC3);
		CHAOS_GLENUM_TO_STRING(GL_INT_VEC4);
		CHAOS_GLENUM_TO_STRING(GL_BOOL);
		CHAOS_GLENUM_TO_STRING(GL_BOOL_VEC2);
		CHAOS_GLENUM_TO_STRING(GL_BOOL_VEC3);
		CHAOS_GLENUM_TO_STRING(GL_BOOL_VEC4);
		CHAOS_GLENUM_TO_STRING(GL_FLOAT_MAT2);
		CHAOS_GLENUM_TO_STRING(GL_FLOAT_MAT3);
		CHAOS_GLENUM_TO_STRING(GL_FLOAT_MAT4);
		CHAOS_GLENUM_TO_STRING(GL_SAMPLER_1D);
		CHAOS_GLENUM_TO_STRING(GL_SAMPLER_2D);
		CHAOS_GLENUM_TO_STRING(GL_SAMPLER_3D);
		CHAOS_GLENUM_TO_STRING(GL_SAMPLER_CUBE);
		CHAOS_GLENUM_TO_STRING(GL_SAMPLER_1D_SHADOW);
		CHAOS_GLENUM_TO_STRING(GL_SAMPLER_2D_SHADOW);
		CHAOS_GLENUM_TO_STRING(GL_SAMPLER_1D_ARRAY);
		CHAOS_GLENUM_TO_STRING(GL_SAMPLER_2D_ARRAY);
		CHAOS_GLENUM_TO_STRING(GL_SAMPLER_1D_ARRAY_SHADOW);
		CHAOS_GLENUM_TO_STRING(GL_SAMPLER_2D_ARRAY_SHADOW);
		CHAOS_GLENUM_TO_STRING(GL_SAMPLER_CUBE_SHADOW);
		CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_INT_VEC2);
		CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_INT_VEC3);
		CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_INT_VEC4);
		CHAOS_GLENUM_TO_STRING(GL_INT_SAMPLER_1D);
		CHAOS_GLENUM_TO_STRING(GL_INT_SAMPLER_2D);
		CHAOS_GLENUM_TO_STRING(GL_INT_SAMPLER_3D);
		CHAOS_GLENUM_TO_STRING(GL_INT_SAMPLER_CUBE);
		CHAOS_GLENUM_TO_STRING(GL_INT_SAMPLER_1D_ARRAY);
		CHAOS_GLENUM_TO_STRING(GL_INT_SAMPLER_2D_ARRAY);
		CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_INT_SAMPLER_1D);
		CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_INT_SAMPLER_2D);
		CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_INT_SAMPLER_3D);
		CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_INT_SAMPLER_CUBE);
		CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_INT_SAMPLER_1D_ARRAY);
		CHAOS_GLENUM_TO_STRING(GL_UNSIGNED_INT_SAMPLER_2D_ARRAY);

		CHAOS_GLENUM_TO_STRING(GL_DEBUG_SEVERITY_HIGH);
		CHAOS_GLENUM_TO_STRING(GL_DEBUG_SEVERITY_MEDIUM);
		CHAOS_GLENUM_TO_STRING(GL_DEBUG_SEVERITY_LOW);
		CHAOS_GLENUM_TO_STRING(GL_DEBUG_SEVERITY_NOTIFICATION);

		CHAOS_GLENUM_TO_STRING(GL_DEBUG_TYPE_ERROR);
		CHAOS_GLENUM_TO_STRING(GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR);
		CHAOS_GLENUM_TO_STRING(GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR);
		CHAOS_GLENUM_TO_STRING(GL_DEBUG_TYPE_PORTABILITY);
		CHAOS_GLENUM_TO_STRING(GL_DEBUG_TYPE_PERFORMANCE);
		CHAOS_GLENUM_TO_STRING(GL_DEBUG_TYPE_OTHER);
		CHAOS_GLENUM_TO_STRING(GL_DEBUG_TYPE_MARKER);
		CHAOS_GLENUM_TO_STRING(GL_DEBUG_TYPE_PUSH_GROUP);
		CHAOS_GLENUM_TO_STRING(GL_DEBUG_TYPE_POP_GROUP);

		CHAOS_GLENUM_TO_STRING(GL_DEBUG_SOURCE_API);
		CHAOS_GLENUM_TO_STRING(GL_DEBUG_SOURCE_WINDOW_SYSTEM);
		CHAOS_GLENUM_TO_STRING(GL_DEBUG_SOURCE_SHADER_COMPILER);
		CHAOS_GLENUM_TO_STRING(GL_DEBUG_SOURCE_THIRD_PARTY);
		CHAOS_GLENUM_TO_STRING(GL_DEBUG_SOURCE_APPLICATION);
		CHAOS_GLENUM_TO_STRING(GL_DEBUG_SOURCE_OTHER);

#undef CHAOS_GLENUM_TO_STRING

		return "Unknown";
	}

	static void GLAPIENTRY DebugMessageHandler(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * msg, const void * user_data)
	{
		if (severity != GL_DEBUG_SEVERITY_NOTIFICATION || GlobalVariables::GLDebugNotifications.Get())
		{
			char const* source_str = GLTools::GLenumToString(source);
			char const * type_str = GLTools::GLenumToString(type);
			char const * severity_str = GLTools::GLenumToString(severity);

			GLLog::Message("OpenGL message  [%s]", msg);
			GLLog::Message("       severity [%s]", severity_str);
			GLLog::Message("       source   [%s]", source_str);
			GLLog::Message("       type     [%s]", type_str);
			GLLog::Message("       id       [%08x]", id);

			//DebugTools::DisplayCallStack(std::cout);

#if _WIN32
			if (GlobalVariables::GLDebugBreak.Get())
				DebugBreak();
#endif
		}
	}

	void GLTools::SetDebugMessageHandler()
	{
		if (glDebugMessageCallback == nullptr) // some low profile cannot use debug features
			return;

		glDebugMessageCallback(DebugMessageHandler, nullptr);

#if _DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB); // this make debug events called immediately (Can BREAK in visual studio)
#endif
	}

}; // namespace chaos
