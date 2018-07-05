#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUTexture.h>
#include <chaos/GPURenderbuffer.h>
#include <chaos/GPUFramebuffer.h>

namespace chaos
{
	class GPUFramebufferGenerator
	{
	public:

		class AttachmentInfo
		{
		public:

			/** the point of attachment */
			GLenum attachment_point = GL_NONE;
			/** the mipmap of concern */
			int texture_mipmap = 0;
			/** the texture of concern */
			boost::intrusive_ptr<GPUTexture> texture;
			/** the texture of concern */
			boost::intrusive_ptr<GPURenderbuffer> renderbuffer;
		};

	public:

		/** generate the framebuffer */
		GPUFramebuffer * GenerateFramebuffer();

		/** Clear the generator */
		void Clear();

		/** attachment */
		bool AddColorAttachment(int color_index, GPURenderbuffer * render_buffer);
		bool AddColorAttachment(int color_index, GPUTexture * texture, int mipmap = 0);

		bool AddDepthStencilAttachment(GPURenderbuffer * render_buffer);		
		bool AddDepthStencilAttachment(GPUTexture * texture, int mipmap = 0);


		/** get the dimension of the framebuffer that will be generated */
		glm::ivec2 GetSize() const;

	protected:

		/** test whether the color attachment is valid */
		bool IsColorAttachmentValid(int color_index) const;
		/** test whether the color attachment is used */
		bool IsColorAttachmentInUse(int color_index) const;
		/** test whether there already is a depth stencil attachment */
		bool HasDepthStencilAttachment() const;

		/** initialize the framebuffer content */
		bool InitializeFramebuffer(GPUFramebuffer * framebuffer);

	protected:

		/** the cached number of color attachments */
		mutable GLint max_color_attachment = -1;
		/** the stored attachment info */
		std::vector<AttachmentInfo> attachment_info;
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
