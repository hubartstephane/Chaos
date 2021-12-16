namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUTextureLoader;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GPUTextureLoader : used to have some loading texture functions for OpenGL
	**/

	class GPUTextureLoader : public ResourceManagerLoader<GPUTexture, GPUResourceManager>
	{
	public:

		/** constructor */
		GPUTextureLoader(GPUResourceManager* in_resource_manager = nullptr) :
			ResourceManagerLoader<GPUTexture, GPUResourceManager>(in_resource_manager) {}

		/** load an object from JSON */
		virtual GPUTexture* LoadObject(char const* name, nlohmann::json const& json, GenTextureParameters const& parameters = {}) const;
		/** texture loading from path */
		virtual GPUTexture* LoadObject(FilePathParam const& path, char const* name = nullptr, GenTextureParameters const& parameters = {}) const;

		/** Generate a texture from a json content */
		virtual GPUTexture* GenTextureObject(nlohmann::json const& json, GenTextureParameters const& parameters = {}) const;
		/** Generate a 1D/2D/rectangle texture from an file */
		virtual GPUTexture* GenTextureObject(FilePathParam const& path, GenTextureParameters const& parameters = {}) const;
		/** Generate a 1D/2D/rectangle texture from an image */
		virtual GPUTexture* GenTextureObject(ImageDescription const& image, GenTextureParameters const& parameters = {}) const;
		/** Generate a 1D/2D/rectangle texture from an image */
		virtual GPUTexture* GenTextureObject(FIBITMAP* image, GenTextureParameters const& parameters = {}) const;
		/** Generate a cube texture from a skybox */
		virtual GPUTexture* GenTextureObject(SkyBoxImages const* skybox, PixelFormatMergeParams const& merge_params = {}, GenTextureParameters const& parameters = {}) const;

		/** Generate a texture from lambda */
		template<typename T, typename GENERATOR>
		GPUTexture* GenTextureObject(int width, int height, GENERATOR const& generator, GenTextureParameters const& parameters = {}) const
		{
			GPUTexture* result = nullptr;

			PixelFormat pixel_format = PixelFormat::GetPixelFormat<T>();

			int buffer_size = ImageTools::GetMemoryRequirementForAlignedTexture(pixel_format, width, height);

			char* buffer = new char[buffer_size];
			if (buffer != nullptr)
			{
				ImageDescription desc = ImageTools::GetImageDescriptionForAlignedTexture(pixel_format, width, height, buffer);
				generator(desc);
				result = GenTextureObject(desc, parameters);
				delete[](buffer);
			}
			return result;
		}

	protected:

		/** for cubemap texture, returns a layer index depending on the face considered */
		static int GetCubeMapLayerValueFromSkyBoxFace(SkyBoxImageType face, int level = 0);

		/** search whether the path is already in used in the manager */
		virtual bool IsPathAlreadyUsedInManager(FilePathParam const& path) const override;
		/** search whether the name is already in used in the manager */
		virtual bool IsNameAlreadyUsedInManager(ObjectRequest request) const override;
	};

#endif

}; // namespace chaos