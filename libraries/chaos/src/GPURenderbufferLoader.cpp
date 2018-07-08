#include <chaos/GPURenderbufferLoader.h>
#include <chaos/GLTextureTools.h>
#include <chaos/GLPixelFormat.h>

namespace chaos
{

	GPURenderbuffer * GPURenderbufferLoader::GenRenderbufferObject(PixelFormat const & pixel_format, glm::ivec2 const & size)
	{
		// test arguments
		if (!pixel_format.IsValid())
			return nullptr;
		if (size.x <= 0 || size.y <= 0)
			return nullptr;

		// create the OpenGL resource
		GLuint renderbuffer_id = 0;
		glGenRenderbuffers(1, &renderbuffer_id);
		if (renderbuffer_id == 0)
			return nullptr;

		// choose format and internal format
		GLPixelFormat gl_formats = GLTextureTools::GetGLPixelFormat(pixel_format);
		assert(gl_formats.IsValid());

		glNamedRenderbufferStorage(renderbuffer_id, gl_formats.internal_format, size.x, size.y);

		// initialize the surface description
		SurfaceDescription description;
		description.width  = size.x;
		description.height = size.y;
		description.depth  = 1;

		description.pixel_format = pixel_format;

		// create the renderbuffer
		GPURenderbuffer * result = new GPURenderbuffer(renderbuffer_id, description);
		if (result != nullptr)
			return result;

		// in case of failure, release OpenGL resource
		glDeleteRenderbuffers(1, &renderbuffer_id);

		return nullptr;
	}

}; // namespace chaos
