#include "chaos/Chaos.h"

namespace chaos
{

	GPUFramebuffer::GPUFramebuffer(GLuint in_id) :
		framebuffer_id(in_id)
	{
	}

	GPUFramebuffer::~GPUFramebuffer()
	{
		Release();
	}

	void GPUFramebuffer::Release()
	{
		if (framebuffer_id != 0)
			glDeleteFramebuffers(1, &framebuffer_id);
		attachment_info.clear();
		framebuffer_id = 0;
	}

	bool GPUFramebuffer::CheckCompletionStatus() const
	{
		if (!IsValid())
			return false;
		// check the status 
		GLenum status = glCheckNamedFramebufferStatus(framebuffer_id, GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
			return false;
		return true;
	}

}; // namespace chaos
