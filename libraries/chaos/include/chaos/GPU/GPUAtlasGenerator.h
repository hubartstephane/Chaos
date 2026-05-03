namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUAtlasGenerator;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GPUAtlasGenerator: an utility class to generate a GPUAtlas
	*/

	class CHAOS_API GPUAtlasGenerator : public GPUDeviceResourceInterface
	{

	public:

		/** constructor */
		GPUAtlasGenerator(GPUDevice* in_gpu_device):
			GPUDeviceResourceInterface(in_gpu_device){}

		/** generate a GPUAtlas from inputs */
		GPUAtlas* GenerateAtlas(AtlasInput const& in_input, AtlasGeneratorParams const& in_params = {}) const;
		/** generate a GPUAtlas from a standard atlas */
		GPUAtlas* GenerateAtlas(Atlas const & in_atlas) const;
		/** generate a GPUAtlas from a standard atlas (+ move) */
		GPUAtlas* GenerateAtlas(Atlas&& in_atlas) const;

		/** fill a GPUAtlas from standard atlas */
		bool FillAtlasContent(GPUAtlas* result, Atlas const & in_atlas) const;
		/** fill a GPUAtlas from standard atlas (+ move) */
		bool FillAtlasContent(GPUAtlas* result, Atlas&& in_atlas) const;

		/** generate a texture from a standard atlas */
		GPUTexture* CreateTextureFromAtlas(Atlas const & in_atlas) const;

	protected:

		/** internal method to copy folder from an atlas to another */
		bool CopyAtlasFolders(AtlasFolderInfo* dst_folder_info, AtlasFolderInfo const* src_folder_info) const;
	};

#endif

}; // namespace chaos