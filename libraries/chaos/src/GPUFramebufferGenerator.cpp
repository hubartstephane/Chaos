#include <chaos/GPUFramebufferGenerator.h>
#include <chaos/GLTools.h>
#include <chaos/GLTextureTools.h>

namespace chaos
{
	GPUFramebuffer * GPUFramebufferGenerator::GenerateFramebuffer()
	{
		GPUFramebuffer * result = nullptr;

		GLuint framebuffer = 0;
		glGenFramebuffers(1, &framebuffer);

		if (framebuffer != 0)
		{
			result = new GPUFramebuffer(framebuffer);
			if (!InitializeFramebuffer(result))
			{
				delete(result);
				result = nullptr;
			}
		}
		return result;
	}

	bool GPUFramebufferGenerator::InitializeFramebuffer(GPUFramebuffer * framebuffer)
	{
		for (AttachmentInfo const & info : attachment_info)
		{

		}
		return true;
	}

	void GPUFramebufferGenerator::Clear()
	{
		attachment_info.clear();
	}

	glm::ivec2 GPUFramebufferGenerator::GetSize() const
	{
		glm::ivec2 result = glm::ivec2(-1, -1);
		for (AttachmentInfo const & attachment : attachment_info)
		{
			if (attachment.texture != nullptr)
				return attachment.texture->GetTextureDescription().GetSize(attachment.texture_mipmap);
			if (attachment.renderbuffer != nullptr)
				return attachment.renderbuffer->GetSurfaceDescription().GetSize(0);
		}
		return result;
	}

	bool GPUFramebufferGenerator::IsColorAttachmentValid(int color_index) const
	{
		assert(color_index >= 0);
		if (max_color_attachment < 0)
			glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachment);
		assert(max_color_attachment >= 0);
		return (color_index < max_color_attachment);		
	}

	
	bool GPUFramebufferGenerator::IsColorAttachmentInUse(int color_index) const
	{
		for (AttachmentInfo const & attachment : attachment_info)
			if (attachment.attachment_point == color_index + GL_COLOR_ATTACHMENT0)
				return true;
		return true;
	}

	bool GPUFramebufferGenerator::HasDepthStencilAttachment() const
	{
		for (AttachmentInfo const & attachment : attachment_info)
			if (attachment.attachment_point == GL_DEPTH_STENCIL_ATTACHMENT)
				return true;
		return true;
	}

	void AddDepthStencilAttachment(GPUTexture * texture, char const * name) {}

	void AddDepthStencilAttachment(GPURenderbuffer * renderbuffer, char const * name) {}

	void AddDepthStencilAttachment(glm::ivec2 const & size, char const * name){}

	//void AddColorAttachment(GPUTexture * texture)


	bool GPUFramebufferGenerator::AddColorAttachment(int color_index, GPURenderbuffer * render_buffer)
	{
		assert(render_buffer != nullptr);
		if (!IsColorAttachmentValid(color_index))
			return false;
		if (IsColorAttachmentInUse(color_index))
			return false;







		return true;
	}

	bool GPUFramebufferGenerator::AddColorAttachment(int color_index, GPUTexture * texture, int mipmap)
	{
		assert(texture != nullptr);
		assert(texture->GetTextureDescription().GetPixelFormat().IsColorPixel());

		if (!IsColorAttachmentValid(color_index))
			return false;
		if (IsColorAttachmentInUse(color_index))
			return false;

		// test size compatibility
		glm::ivec2 size = GetSize();
		if (size.x >= 0 && size.y > 0)
		{
			glm::ivec2 texture_size = texture->GetTextureDescription().GetSize(mipmap);		
			if (texture_size != size)
				return false;
		}

		// add an attachment
		AttachmentInfo attachment;
		attachment.attachment_point = color_index + GL_COLOR_ATTACHMENT0;
		attachment.texture = texture;
		attachment.texture_mipmap = mipmap;

		attachment_info.push_back(attachment);

		return true;
	}

	bool GPUFramebufferGenerator::AddDepthStencilAttachment(GPURenderbuffer * render_buffer)
	{
		assert(render_buffer != nullptr);





		return true;
	}
	
	bool GPUFramebufferGenerator::AddDepthStencilAttachment(GPUTexture * texture, int mipmap)
	{
		assert(texture != nullptr);
		assert(texture->GetTextureDescription().GetPixelFormat().IsDepthStencilPixel());

		if (HasDepthStencilAttachment())
			return false;

		// test size compatibility
		glm::ivec2 size = GetSize();
		if (size.x >= 0 && size.y > 0)
		{
			glm::ivec2 texture_size = texture->GetTextureDescription().GetSize(mipmap);
			if (texture_size != size)
				return false;
		}

		// add an attachment
		AttachmentInfo attachment;
		attachment.attachment_point = GL_DEPTH24_STENCIL8;
		attachment.texture = texture;
		attachment.texture_mipmap = mipmap;

		attachment_info.push_back(attachment);

		return true;
	}

#if 0
	
	GL_MAX_COLOR_ATTACHMENTS

		GLuint framebuffer = 0;
		glGenFramebuffers(1, &framebuffer);

		GLuint depth_renderbuffer = 0;
		glGenRenderbuffers(1, &depth_renderbuffer);

		glNamedRenderbufferStorage(depth_renderbuffer, GL_DEPTH24_STENCIL8, 512, 512);

		GLuint color_renderbuffer = 0;
		glGenRenderbuffers(1, &color_renderbuffer);
		glNamedRenderbufferStorage(color_renderbuffer, GL_RGBA, 512, 512);

		glNamedFramebufferRenderbuffer(framebuffer, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_renderbuffer);
		glNamedFramebufferTexture(framebuffer, GL_COLOR_ATTACHMENT0, texture_id, texture_level);
		glCheckNamedFramebufferStatus(framebuffer, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;

		GL_DEPTH_STENCIL_ATTACHMENT

			GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
	/*
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_COLOR_ATTACHMENT1 0x8CE1
#define GL_COLOR_ATTACHMENT2 0x8CE2
#define GL_COLOR_ATTACHMENT3 0x8CE3
#define GL_COLOR_ATTACHMENT4 0x8CE4
#define GL_COLOR_ATTACHMENT5 0x8CE5
#define GL_COLOR_ATTACHMENT6 0x8CE6
#define GL_COLOR_ATTACHMENT7 0x8CE7
#define GL_COLOR_ATTACHMENT8 0x8CE8
#define GL_COLOR_ATTACHMENT9 0x8CE9
#define GL_COLOR_ATTACHMENT10 0x8CEA
#define GL_COLOR_ATTACHMENT11 0x8CEB
#define GL_COLOR_ATTACHMENT12 0x8CEC
#define GL_COLOR_ATTACHMENT13 0x8CED
#define GL_COLOR_ATTACHMENT14 0x8CEE
#define GL_COLOR_ATTACHMENT15 0x8CEF
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_STENCIL_ATTACHMENT 0x8D20
		*/
#endif


}; // namespace chaos
