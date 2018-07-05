#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUTexture.h>
#include <chaos/GPUResource.h>
#include <chaos/GPURenderbuffer.h>

namespace chaos
{
	class GPUFramebuffer : public GPUResource
	{
		friend class GPUFramebufferGenerator;

	public:

		class AttachmentInfo
		{
		public:

			/** the name of the attachment */
			std::string name;
			/** the type of attachment */
			GLenum type = GL_NONE;
			/** texture attachment */
			boost::intrusive_ptr<GPUTexture> texture;
			/** texture attachment */
			boost::intrusive_ptr<GPURenderbuffer> renderbuffer;
		};

		/** constructor */
		GPUFramebuffer(GLuint in_id = 0);
		/** destructor */
		virtual ~GPUFramebuffer();

		/** returns true whether the resource is valid */
		bool IsValid() const { return glIsFramebuffer(framebuffer_id) == GL_TRUE; }
		/** returns the GL name of the resource */
		GLuint GetResourceID() const { return framebuffer_id; }

		/** find an attachment by its name */
		AttachmentInfo const * GetAttachment(char const * name) const;
		/** find an attachment by its type */
		AttachmentInfo const * GetAttachment(GLenum type) const;
		/** find the (unique) depth stencil attachment */
		AttachmentInfo const * GetDepthStencilAttachment() const;
		/** find the color attachment */
		AttachmentInfo const * GetColorAttachment(int color_index) const;

	protected:

		/** cleaning the object */
		virtual bool DoRelease() override;

	protected:

		/** the resource id */
		GLuint framebuffer_id = 0;
		/** private and shared resources */
		std::vector<AttachmentInfo> attachments;
	};

}; // namespace chaos
