#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GPUTexture;

}; // namespace chaos

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else 

namespace chaos
{

	class GPUTexture : public GPUSurface
	{
		friend class GPUResourceManager;

	public:

		/** constructor */
		GPUTexture(GLuint in_id = 0, TextureDescription const & in_texture_description = TextureDescription());
		/** destructor */
		virtual ~GPUTexture();

		/** inherited */
		virtual SurfaceDescription const & GetSurfaceDescription() const override { return GetTextureDescription(); }
		/** get the description of the texture */
		TextureDescription const & GetTextureDescription() const { return texture_description; }
		/** returns true whether the resource is valid */
		bool IsValid() const { return glIsTexture(texture_id) == GL_TRUE; }
		/** returns the GL name of the resource */
		GLuint GetResourceID() const { return texture_id; }

		/** override */
		virtual void Release() override;

	protected:

		/** the resource id */
		GLuint texture_id = 0;
		/** the description of the texture */
		TextureDescription texture_description;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION



