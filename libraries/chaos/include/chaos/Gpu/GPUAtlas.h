namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUAtlas;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API GPUAtlas : public BitmapAtlas::AtlasBase
	{
	public:

		/** the clearing method */
		virtual void Clear() override;
		/** load an atlas from an index file */
		bool LoadAtlas(FilePathParam const& path);
		/** generate a texture atlas from a standard atlas */
		bool LoadFromBitmapAtlas(BitmapAtlas::Atlas const& atlas);
		/** generate a texture atlas from a standard atlas */
		bool LoadFromBitmapAtlas(BitmapAtlas::Atlas&& atlas);

		/* get the array texture */
		GPUTexture* GetTexture() { return texture.get(); }
		/* get the array texture */
		GPUTexture const* GetTexture() const { return texture.get(); }

	protected:

		/** generate a texture atlas from a standard atlas */
		bool DoLoadFromBitmapAtlas(BitmapAtlas::Atlas const& atlas);
		/** generate a texture atlas from a standard atlas */
		bool DoLoadFromBitmapAtlas(BitmapAtlas::Atlas&& atlas);
		/** copy src folder into dst folder */
		bool DoCopyFolder(BitmapAtlas::FolderInfo* dst_folder_info, BitmapAtlas::FolderInfo const* src_folder_info);
		/** generate a texture array */
		bool DoGenerateTextureArray(BitmapAtlas::Atlas const& atlas);

	protected:

		/** the texture array that will be generated */
		shared_ptr<GPUTexture> texture;
	};

#endif

}; // namespace chaos