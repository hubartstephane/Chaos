#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUTexture.h>
#include <chaos/GPURenderbuffer.h>
#include <chaos/GPUFramebuffer.h>
#include <chaos/GLTextureTools.h>

namespace chaos
{

	class GPUFramebufferGeneratorAttachmentInfo : public GPUFramebufferAttachmentInfo
	{
	public:

		/** the format for the generation (valid if texture & renderbuffer are null) */
		PixelFormat pixel_format;
		/** the size for the generation (valid if texture & renderbuffer are null) */
		glm::ivec2 size = glm::ivec2(0, 0);
		/** the texture generation parameter */
		GenTextureParameters gen_texture_parameters;
	};

	class GPUFramebufferGenerator : public GPUAttachmentOwner<GPUFramebufferGeneratorAttachmentInfo>
	{

	public:

		/** generate the framebuffer */
		GPUFramebuffer * GenerateFramebuffer(glm::ivec2 const & wanted_size = glm::ivec2(0, 0));

		/** Clear the generator */
		void Clear();

		/** depth stencil attachment method */
		bool AddDepthStencilAttachment(GPUTexture * texture, int mipmap = 0, char const * name = nullptr);
		/** depth stencil attachment method */
		bool AddDepthStencilAttachment(GPURenderbuffer * renderbuffer, char const * name = nullptr);
		/** depth stencil attachment method */
		bool AddDepthStencilAttachment(glm::ivec2 const & in_size, char const * name = nullptr, GenTextureParameters const & gen_texture_parameters = GenTextureParameters());

		/** color attachment method */
		bool AddColorAttachment(int color_index, GPUTexture * texture, int mipmap = 0, char const * name = nullptr);
		/** color attachment method */
		bool AddColorAttachment(int color_index, GPURenderbuffer * renderbuffer, char const * name = nullptr);
		/** color attachment method */
		bool AddColorAttachment(int color_index, PixelFormat const & pixel_format, glm::ivec2 const & in_size, char const * name = nullptr, GenTextureParameters const & gen_texture_parameters = GenTextureParameters());

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
		bool InitializeFramebuffer(GPUFramebuffer * framebuffer, glm::ivec2 const & final_size);

	protected:

		/** the cached number of color attachments */
		mutable GLint max_color_attachment = -1;
		/** the size of the future framebuffer */
		glm::ivec2 size = glm::ivec2(0, 0);
	};

}; // namespace chaos
