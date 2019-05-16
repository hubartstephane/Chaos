#include <chaos/GPUFence.h>

namespace chaos
{

#if 0

	GPUFence::GPUFence(GLuint in_id, SurfaceDescription const & in_surface_description) :
		renderbuffer_id(in_id),
		surface_description(in_surface_description)
	{
	}

	GPUFence::~GPUFence()
	{
		DoRelease();
	}

	bool GPUFence::DoRelease()
	{
		if (renderbuffer_id == 0)
			return false;
		glDeleteRenderbuffers(1, &renderbuffer_id);
		renderbuffer_id = 0;
		return true;	
	}
#endif

}; // namespace chaos
