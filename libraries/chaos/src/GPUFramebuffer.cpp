#include <chaos/GPUFramebuffer.h>

namespace chaos
{

	// ==========================================================================


	GPURenderbuffer::GPURenderbuffer(GLuint in_id) :
		renderbuffer_id(in_id)
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


	// ==========================================================================



	GPUFramebuffer::GPUFramebuffer(GLuint in_id) :
		framebuffer_id(in_id)
	{
	}

	GPUFramebuffer::~GPUFramebuffer()
	{
		DoRelease();
	}

	bool GPUFramebuffer::DoRelease()
	{
		if (framebuffer_id == 0)
			return false;
		glDeleteFramebuffers(1, &framebuffer_id);
		framebuffer_id = 0;
		return true;	
	}

	// ==========================================================================


	GPUFramebuffer * GPUFramebufferGenerator::GenerateFramebuffer()
	{
		GPUFramebuffer * result = nullptr;





		return result;
	}

}; // namespace chaos
