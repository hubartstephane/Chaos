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
		//for (AttachmentInfo const & info : attachment_info)
		{








		}
		return true;
	}

	void GPUFramebufferGenerator::Clear()
	{
		attachment_info.clear();
		size = glm::ivec2(0, 0);
	}

	bool GPUFramebufferGenerator::IsSurfaceSizeCompatible(GPUSurface * surface, int mipmap) const
	{
		assert(surface != nullptr);
		assert(mipmap >= 0);
		return IsSurfaceSizeCompatible(surface->GetSurfaceDescription().GetSize(mipmap));
	}

	bool GPUFramebufferGenerator::IsSurfaceSizeCompatible(glm::ivec2 const & in_size) const
	{
		// size is not known yet
		if (size == glm::ivec2(0, 0))
			return true;
		if (in_size == glm::ivec2(0, 0))
			return true;
		// test whether the size are compatible
		return (size == in_size);
	}

	bool GPUFramebufferGenerator::IsColorAttachmentValid(int color_index) const
	{
		assert(color_index >= 0);
		if (max_color_attachment < 0)
			glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachment);
		assert(max_color_attachment >= 0);
		return (color_index < max_color_attachment);
	}

	bool GPUFramebufferGenerator::IsColorAttachmentValid(int color_index, char const * name) const
	{
		if (IsAttachmentNameInUse(name))
			return false;
		if (IsColorAttachmentInUse(color_index))
			return false;
		if (!IsColorAttachmentValid(color_index))
			return false;
		return true;
	}

	void GPUFramebufferGenerator::CompleteAndInsertAttachment(GPUFramebufferGeneratorAttachmentInfo & info, char const * name)
	{
		// complete the name if required
		if (name != nullptr)
			info.name = name;
		// compute size if required
		if (size == glm::ivec2(0, 0)) 
		{
			glm::ivec2 new_size = glm::ivec2(0, 0);
			if (info.texture != nullptr)
				new_size = info.texture->GetSurfaceDescription().GetSize(info.texture_mipmap);
			else if (info.renderbuffer != nullptr)
				new_size = info.renderbuffer->GetSurfaceDescription().GetSize(0);
			else
				new_size = info.size;

			if (new_size != glm::ivec2(0, 0))
				size = new_size;
		}
		// insert the attachment
		attachment_info.push_back(std::move(info));
	}

	bool GPUFramebufferGenerator::AddDepthStencilAttachment(GPUTexture * texture, int mipmap, char const * name)
	{
		assert(texture != nullptr);
		assert(mipmap >= 0);

		if (!IsSurfaceSizeCompatible(texture, mipmap))
			return false;
		if (IsAttachmentNameInUse(name))
			return false;

		GPUFramebufferGeneratorAttachmentInfo info;
		info.attachment_point = GL_DEPTH24_STENCIL8;
		info.texture = texture;
		info.texture_mipmap = mipmap;
		CompleteAndInsertAttachment(info, name);
		return true;
	}

	bool GPUFramebufferGenerator::AddDepthStencilAttachment(GPURenderbuffer * renderbuffer, char const * name)
	{
		assert(renderbuffer != nullptr);

		if (!IsSurfaceSizeCompatible(renderbuffer))
			return false;
		if (IsAttachmentNameInUse(name))
			return false;

		GPUFramebufferGeneratorAttachmentInfo info;
		info.attachment_point = GL_DEPTH24_STENCIL8;
		info.renderbuffer = renderbuffer;
		CompleteAndInsertAttachment(info, name);
		return true;
	}

	bool GPUFramebufferGenerator::AddDepthStencilAttachment(glm::ivec2 const & in_size, char const * name)
	{
		if (!IsSurfaceSizeCompatible(in_size))
			return false;
		if (IsAttachmentNameInUse(name))
			return false;

		GPUFramebufferGeneratorAttachmentInfo info;
		info.attachment_point = GL_DEPTH24_STENCIL8;
		info.size = in_size;
		CompleteAndInsertAttachment(info, name);
		return true;
	}



	bool GPUFramebufferGenerator::AddColorAttachment(int color_index, GPUTexture * texture, int mipmap, char const * name)
	{
		assert(texture != nullptr);
		assert(mipmap >= 0);

		if (!IsSurfaceSizeCompatible(texture, mipmap))
			return false;
		if (!IsColorAttachmentValid(color_index, name))
			return false;

		GPUFramebufferGeneratorAttachmentInfo info;
		info.attachment_point = GL_COLOR_ATTACHMENT0 + color_index;
		info.texture = texture;
		info.texture_mipmap = mipmap;
		CompleteAndInsertAttachment(info, name);
		return true;
	}

	bool GPUFramebufferGenerator::AddColorAttachment(int color_index, GPURenderbuffer * renderbuffer, char const * name)
	{
		assert(renderbuffer != nullptr);

		if (!IsSurfaceSizeCompatible(renderbuffer))
			return false;
		if (!IsColorAttachmentValid(color_index, name))
			return false;

		GPUFramebufferGeneratorAttachmentInfo info;
		info.attachment_point = GL_COLOR_ATTACHMENT0 + color_index;
		info.renderbuffer = renderbuffer;
		CompleteAndInsertAttachment(info, name);
		return true;
	}

	bool GPUFramebufferGenerator::AddColorAttachment(int color_index, PixelFormat const & pixel_format, glm::ivec2 const & in_size, char const * name)
	{
		if (!IsSurfaceSizeCompatible(in_size))
			return false;
		if (!IsColorAttachmentValid(color_index, name))
			return false;
		if (!pixel_format.IsValid())
			return false;
		if (!pixel_format.IsColorPixel())
			return false;

		GPUFramebufferGeneratorAttachmentInfo info;
		info.attachment_point = GL_COLOR_ATTACHMENT0 + color_index;
		info.pixel_format = pixel_format;
		info.size = in_size;
		CompleteAndInsertAttachment(info, name);
		return true;
	}

	glm::ivec2 GPUFramebufferGenerator::GetSize() const
	{
		return size;
	}

#if 0
	GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT or GL_DEPTH_STENCIL_ATTACHMENT.
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
		glNamedFramebufferTextureLayer
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
