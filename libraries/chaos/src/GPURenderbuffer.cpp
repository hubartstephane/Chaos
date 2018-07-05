#include <chaos/GPURenderbuffer.h>

namespace chaos
{
	GPURenderbuffer::GPURenderbuffer(GLuint in_id, SurfaceDescription const & in_surface_description) :
		renderbuffer_id(in_id),
		surface_description(in_surface_description)
	{
	}

	GPURenderbuffer::~GPURenderbuffer()
	{
		DoRelease();
	}

	bool GPURenderbuffer::DoRelease()
	{
		if (renderbuffer_id == 0)
			return false;
		glDeleteRenderbuffers(1, &renderbuffer_id);
		renderbuffer_id = 0;
		return true;	
	}

}; // namespace chaos
