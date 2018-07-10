#include <chaos/GPUFramebuffer.h>

namespace chaos
{

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
		attachment_info.clear();
		framebuffer_id = 0;
		return true;	
	}

	bool GPUFramebuffer::BeginRendering()
	{
		if (!IsValid())
			return false;
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
		return true;
	}

	bool GPUFramebuffer::EndRendering(bool generate_mipmaps)
	{
		if (!IsValid())
			return false;
		if (generate_mipmaps)
		{
			for (GPUFramebufferAttachmentInfo & info : attachment_info)
				if (info.texture != nullptr)
					glGenerateTextureMipmap(info.texture->GetResourceID());
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
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
