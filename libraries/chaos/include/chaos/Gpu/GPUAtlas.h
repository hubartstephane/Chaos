namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUAtlas;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * GPUAtlas: an atlas with its GPUTexture
	 */

	class CHAOS_API GPUAtlas : public AtlasBase
	{
		friend class GPUAtlasGenerator;

	public:

		/** override */
		virtual void Clear() override;

		/* get the array texture */
		GPUTexture* GetTexture() { return texture.get(); }
		/* get the array texture */
		GPUTexture const* GetTexture() const { return texture.get(); }

	protected:

		/** the texture array that will be generated */
		shared_ptr<GPUTexture> texture;
	};

#endif

}; // namespace chaos