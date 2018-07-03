#include <chaos/GLPixelFormat.h>

namespace chaos
{

	GLPixelFormat::GLPixelFormat(GLenum in_format, GLenum in_internal_format) :
		format(in_format),
		internal_format(in_internal_format) {}

	bool GLPixelFormat::IsValid() const
	{
		if (internal_format == GL_DEPTH24_STENCIL8)
			return (format == GL_NONE);

		if (
			format != GL_RED &&
			format != GL_BGR &&
			format != GL_BGRA &&
			format != GL_RGB &&
			format != GL_RGBA)
			return false;

		if (
			internal_format != GL_R8 &&
			internal_format != GL_RGB8 &&
			internal_format != GL_RGBA8 &&
			internal_format != GL_R32F &&
			internal_format != GL_RGB32F &&
			internal_format != GL_RGBA32F)
			return false;

		return true;
	}

}; // namespace chaos
