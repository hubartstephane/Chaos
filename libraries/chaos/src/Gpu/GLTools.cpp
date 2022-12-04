#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	namespace Arguments
	{
		CHAOS_APPLICATION_ARG(bool, GLDebugNotifications);
#if _WIN32 || _WIN64
		CHAOS_APPLICATION_ARG(bool, GLDebugBreak);
#endif
	};

	void GLTools::SetViewport(aabox2 const& viewport)
	{
		glViewport(viewport.position.x, viewport.position.y, viewport.size.x, viewport.size.y);
	}

	bool GLTools::IsMatrixType(GLenum type)
	{
		// XXX : samplers type, maybe some error or some missing elements
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

	static int GetEnumVectorArityImpl(GLenum type, GLenum const* values)
	{
		for (int i = 0; i < 4; ++i)
			if (type == values[i])
				return (i + 1);
		return 0;
	}

	int GLTools::GetEnumVectorArity(GLenum type)
	{
		if (int result = GetEnumVectorArityBool(type))
			return result;
		if (int result = GetEnumVectorArityFloat(type))
			return result;
		if (int result = GetEnumVectorArityDouble(type))
			return result;
		if (int result = GetEnumVectorArityInt(type))
			return result;
		if (int result = GetEnumVectorArityUnsignedInt(type))
			return result;
		return 0;
	}

	int GLTools::GetEnumVectorArityBool(GLenum type)
	{
		GLenum const values[] = { GL_BOOL, GL_BOOL_VEC2, GL_BOOL_VEC3, GL_BOOL_VEC4 };
		return GetEnumVectorArityImpl(type, values);
	}

	int GLTools::GetEnumVectorArityFloat(GLenum type)
	{
		GLenum const values[] = { GL_FLOAT, GL_FLOAT_VEC2, GL_FLOAT_VEC3, GL_FLOAT_VEC4 };
		return GetEnumVectorArityImpl(type, values);
	}

	int GLTools::GetEnumVectorArityDouble(GLenum type)
	{
		GLenum const values[] = { GL_DOUBLE, GL_DOUBLE_VEC2, GL_DOUBLE_VEC3, GL_DOUBLE_VEC4 };
		return GetEnumVectorArityImpl(type, values);
	}

	int GLTools::GetEnumVectorArityInt(GLenum type)
	{
		GLenum const values[] = { GL_INT, GL_INT_VEC2, GL_INT_VEC3, GL_INT_VEC4 };
		return GetEnumVectorArityImpl(type, values);
	}

	int GLTools::GetEnumVectorArityUnsignedInt(GLenum type)
	{
		GLenum const values[] = { GL_UNSIGNED_INT, GL_UNSIGNED_INT_VEC2, GL_UNSIGNED_INT_VEC3, GL_UNSIGNED_INT_VEC4 };
		return GetEnumVectorArityImpl(type, values);
	}

	void GLTools::DisplayGenericInformation()
	{
		GLubyte const * renderer = glGetString(GL_RENDERER);
		if (renderer != nullptr)
			Log::Message("OpenGL RENDERER        : %s", renderer);

		GLubyte const * vendor = glGetString(GL_VENDOR);
		if (vendor != nullptr)
			Log::Message("OpenGL VENDOR          : %s", vendor);

		GLubyte const * version = glGetString(GL_VERSION);
		if (version != nullptr)
			Log::Message("OpenGL VERSION         : %s", version);

		GLubyte const * shading_version = glGetString(GL_SHADING_LANGUAGE_VERSION);
		if (shading_version != nullptr)
			Log::Message("OpenGL SHADING LANGAGE : %s", shading_version);
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
		if (severity != GL_DEBUG_SEVERITY_NOTIFICATION || Arguments::GLDebugNotifications.Get())
		{
			char const* source_str = GLTools::GLenumToString(source);
			char const * type_str = GLTools::GLenumToString(type);
			char const * severity_str = GLTools::GLenumToString(severity);

			Log::Message("OpenGL message  [%s]", msg);
			Log::Message("       severity [%s]", severity_str);
			Log::Message("       source   [%s]", source_str);
			Log::Message("       type     [%s]", type_str);
			Log::Message("       id       [%08x]", id);

			//DebugTools::DisplayCallStack(std::cout);

#if _WIN32 || _WIN64
			if (Arguments::GLDebugBreak.Get())
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
