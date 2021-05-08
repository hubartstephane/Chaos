namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPURenderbuffer;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class GPURenderbuffer : public GPUSurface
	{
	public:

		/** constructor */
		GPURenderbuffer(GLuint in_id = 0, SurfaceDescription const& in_surface_description = {});
		/** destructor */
		virtual ~GPURenderbuffer();

		/** returns true whether the resource is valid */
		bool IsValid() const { return glIsRenderbuffer(renderbuffer_id) == GL_TRUE; }
		/** returns the GL name of the resource */
		GLuint GetResourceID() const { return renderbuffer_id; }
		/** get the description of the surface */
		SurfaceDescription const& GetSurfaceDescription() const override { return surface_description; }

		/** override */
		virtual void Release() override;

	protected:

		/** the resource id */
		GLuint renderbuffer_id = 0;
		/** the description of the framebuffer */
		SurfaceDescription surface_description;
	};

#endif

}; // namespace chaos