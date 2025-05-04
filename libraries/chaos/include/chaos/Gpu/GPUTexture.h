namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUTexture;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API GPUTexture : public GPUSurface, public GPUDeviceResourceInterface
	{
		friend class GPUResourceManager;
		friend class GPUDevice;

	public:

		/** destructor */
		virtual ~GPUTexture();

		/** inherited */
		virtual SurfaceDescription const& GetSurfaceDescription() const override { return GetTextureDescription(); }
		/** get the description of the texture */
		TextureDescription const& GetTextureDescription() const { return texture_description; }
		/** returns true whether the resource is valid */
		bool IsValid() const { return glIsTexture(texture_id) == GL_TRUE; }
		/** returns the GL name of the resource */
		GLuint GetResourceID() const { return texture_id; }

		/** get the number of mipmaps */
		int GetMipmapCount() const;

		/** populate image data from gpu (for arrays, use offset.z for the slice index) */
		bool SetSubImage(ImageDescription const& image_description, glm::ivec3 const& offset = { 0, 0, 0 }, int mipmap_level = 0);
		/** populate image data from gpu (for cubemaps) */
		bool SetSubImageCubeMap(ImageDescription const& image_description, CubeMapFaceType face, glm::ivec3 const & offset = {0, 0, 0}, int mipmap_level = 0);

		/** set the mignification filter */
		void SetMinificationFilter(TextureMinificationFilter min_filter);
		/** set the magnification filter */
		void SetMagnificationFilter(TextureMagnificationFilter mag_filter);
		/** change wrapping methods */
		void SetWrapMethods(TextureWrapMethods const & wrap_methods);
		/** generate mipmaps */
		void GenerateMipmaps();

		/** override */
		virtual void Release() override;



	protected:

		/** constructor */
		GPUTexture(GPUDevice * in_device, GLuint in_id, TextureDescription const& in_texture_description);

		/** checks for some parameters before a SubImage operation */
		bool CheckSubImageCommonParameters(ImageDescription const& image_description, glm::ivec3 const& offset, int mipmap_level) const;

	protected:

		/** the resource id */
		GLuint texture_id = 0;
		/** the description of the texture */
		TextureDescription texture_description;
	};

#endif

}; // namespace chaos