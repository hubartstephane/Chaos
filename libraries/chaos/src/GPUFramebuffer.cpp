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
		framebuffer_id = 0;
		return true;	
	}

	GPUFramebuffer::AttachmentInfo const * GPUFramebuffer::GetAttachment(char const * name) const
	{
		assert(name != nullptr);
		for (AttachmentInfo const & attachment : attachments)
			if (attachment.name == name)
				return &attachment;
		return nullptr;
	}

	GPUFramebuffer::AttachmentInfo const * GPUFramebuffer::GetAttachment(GLenum type) const
	{
		for (AttachmentInfo const & attachment : attachments)
			if (attachment.type == type)
				return &attachment;
		return nullptr;
	}

	GPUFramebuffer::AttachmentInfo const * GPUFramebuffer::GetDepthStencilAttachment() const
	{
		return GetAttachment(GL_DEPTH24_STENCIL8);
	}

	GPUFramebuffer::AttachmentInfo const * GPUFramebuffer::GetColorAttachment(int color_index) const
	{
#if _DEBUG
		GLint max_color_attachment = 0;
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachment);
		assert(color_index - GL_COLOR_ATTACHMENT0 < max_color_attachment);
#endif			
		return GetAttachment(color_index + GL_COLOR_ATTACHMENT0);
	}

}; // namespace chaos
