#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUTexture.h>
#include <chaos/GPURenderbuffer.h>
#include <chaos/GPUFramebuffer.h>

namespace chaos
{

	class GPUFramebufferGeneratorAttachmentInfo : public GPUFramebufferAttachmentInfo
	{
	public:

		/** the format for the generation (valid if texture & renderbuffer are null) */
		PixelFormat pixel_format;
		/** the size for the generation (valid if texture & renderbuffer are null) */
		glm::ivec2 size = glm::ivec2(0, 0);
	};

	class GPUFramebufferGenerator : public GPUAttachmentOwner<GPUFramebufferGeneratorAttachmentInfo>
	{

	public:

		/** generate the framebuffer */
		GPUFramebuffer * GenerateFramebuffer();

		/** Clear the generator */
		void Clear();

		/** depth stencil attachment method */
		bool AddDepthStencilAttachment(GPUTexture * texture, int mipmap = 0, char const * name = nullptr);
		/** depth stencil attachment method */
		bool AddDepthStencilAttachment(GPURenderbuffer * renderbuffer, char const * name = nullptr);
		/** depth stencil attachment method */
		bool AddDepthStencilAttachment(glm::ivec2 const & in_size, char const * name = nullptr);

		/** color attachment method */
		bool AddColorAttachment(int color_index, GPUTexture * texture, int mipmap = 0, char const * name = nullptr);
		/** color attachment method */
		bool AddColorAttachment(int color_index, GPURenderbuffer * renderbuffer, char const * name = nullptr);
		/** color attachment method */
		bool AddColorAttachment(int color_index, PixelFormat const & pixel_format, glm::ivec2 const & in_size, char const * name = nullptr);

		/** get the dimension of the framebuffer that will be generated */
		glm::ivec2 GetSize() const;

	protected:

		/** test whether the size of the surface correspond to what is already stored */
		bool IsSurfaceSizeCompatible(GPUSurface * surface, int mipmap = 0) const;
		/** test whether the size of the surface correspond to what is already stored */
		bool IsSurfaceSizeCompatible(glm::ivec2 const & in_size) const;

		/** test whether the color attachment is valid */
		bool IsColorAttachmentValid(int color_index) const;
		/** test whether the color attachment is valid */
		bool IsColorAttachmentValid(int color_index, char const * name) const;



		/** insert a new attachment */
		void CompleteAndInsertAttachment(GPUFramebufferGeneratorAttachmentInfo & info, char const * name);

		/** initialize the framebuffer content */
		bool InitializeFramebuffer(GPUFramebuffer * framebuffer);

	protected:

		/** the cached number of color attachments */
		mutable GLint max_color_attachment = -1;
		/** the size of the future framebuffer */
		glm::ivec2 size = glm::ivec2(0, 0);
	};




#if 0
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

#endif


}; // namespace chaos
