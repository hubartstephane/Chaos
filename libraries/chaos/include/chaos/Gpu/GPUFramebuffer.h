#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GPUFramebufferAttachmentInfo;
	class GPUFramebuffer;

	template<typename ATTACHMENT_TYPE>
	class GPUAttachmentOwner;

}; // namespace chaos

#else 

namespace chaos
{
	class GPUFramebufferAttachmentInfo
	{
	public:

		/** the name of the attachment */
		std::string name;
		/** the type of attachment */
		GLenum attachment_point = GL_NONE;
		/** the mipmap of concern */
		int texture_mipmap = 0;
		/** texture attachment */
		shared_ptr<GPUTexture> texture;
		/** texture attachment */
		shared_ptr<GPURenderbuffer> renderbuffer;
	};

	template<typename ATTACHMENT_TYPE>
	class GPUAttachmentOwner
	{
	public:

		/** returns an attachment by its type */
		ATTACHMENT_TYPE const * GetAttachment(char const * name) const
		{
			assert(name != nullptr);
			for (ATTACHMENT_TYPE const & attachment : attachment_info)
				if (attachment.name == name)
					return &attachment;
			return nullptr;
		}
		/** returns an attachment by its type */
		ATTACHMENT_TYPE const * GetAttachment(GLenum type) const
		{
			for (ATTACHMENT_TYPE const & attachment : attachment_info)
				if (attachment.attachment_point == type)
					return &attachment;
			return nullptr;
		}
		/** get the color attachment */
		ATTACHMENT_TYPE const * GetColorAttachment(int color_index) const
		{
#if _DEBUG
			GLint max_color_attachment = 0;
			glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachment);
			assert(color_index - GL_COLOR_ATTACHMENT0 < max_color_attachment);
#endif			
			return GetAttachment(color_index + GL_COLOR_ATTACHMENT0);
		}
		/** get the depth stencil attachment */
		ATTACHMENT_TYPE const * GetDepthStencilAttachment() const
		{
			return GetAttachment(GL_DEPTH_STENCIL_ATTACHMENT);
		}
		/** returns whether the name is already in use in an attachment */
		bool IsAttachmentNameInUse(char const * name) const
		{
			if (name == nullptr)
				return false;
			return (GetAttachment(name) != nullptr);
		}
		/** returns whether the surface is already in use in an attachment */
		bool IsSurfaceInUse(GPUSurface * surface) const
		{
			assert(surface != nullptr);
			for (ATTACHMENT_TYPE const & attachment : attachment_info)
			{
				if (attachment.texture == surface)
					return true;
				if (attachment.renderbuffer == surface)
					return true;
			}
			return false;
		}
		/** test whether the color attachment is used */
		bool IsColorAttachmentInUse(int color_index) const
		{
			return (GetColorAttachment(color_index) != nullptr);
		}
		/** test whether there already is a depth stencil attachment */
		bool HasDepthStencilAttachment() const
		{
			return (GetDepthStencilAttachment() != nullptr);
		}

	protected:

		/** private and shared resources */
		std::vector<ATTACHMENT_TYPE> attachment_info;
	};

	class GPUFramebuffer : public GPUResource, public GPUAttachmentOwner<GPUFramebufferAttachmentInfo>
	{
		friend class GPUFramebufferGenerator;
		friend class GPURenderer;

	public:

		/** constructor */
		GPUFramebuffer(GLuint in_id = 0);
		/** destructor */
		virtual ~GPUFramebuffer();

		/** returns true whether the resource is valid */
		bool IsValid() const { return glIsFramebuffer(framebuffer_id) == GL_TRUE; }
		/** returns the GL name of the resource */
		GLuint GetResourceID() const { return framebuffer_id; }

		/** get the completion status of the framebuffer */
		bool CheckCompletionStatus() const;

		/** a getter on size */
		glm::ivec2 const & GetSize() const { return size; }

	protected:

		/** cleaning the object */
		virtual bool DoRelease() override;

	protected:

		/** the resource id */
		GLuint framebuffer_id = 0;
		/** the size of the framebuffer */
		glm::ivec2 size = glm::ivec2(0, 0);
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION

