#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	GPURenderbuffer::GPURenderbuffer(GLuint in_id, SurfaceDescription const & in_surface_description) :
		renderbuffer_id(in_id),
		surface_description(in_surface_description)
	{
	}

	GPURenderbuffer::~GPURenderbuffer()
	{
		Release();
	}

	void GPURenderbuffer::Release()
	{
		if (renderbuffer_id != 0)
			glDeleteRenderbuffers(1, &renderbuffer_id);
		renderbuffer_id = 0;
	}

}; // namespace chaos
