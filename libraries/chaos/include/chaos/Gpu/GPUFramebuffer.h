#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GPUFramebufferAttachmentInfo;
	class GPUFramebuffer;

}; // namespace chaos

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


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

	class GPUFramebuffer : public GPUResource, public GPUFramebufferAttachmentOwner<GPUFramebufferAttachmentInfo>
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

		/** override */
		virtual void Release() override;

	protected:

		/** the resource id */
		GLuint framebuffer_id = 0;
		/** the size of the framebuffer */
		glm::ivec2 size = glm::ivec2(0, 0);
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION

