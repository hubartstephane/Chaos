namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUTexturePoolTextureInfo;
	class GPUTexturePoolTextureInfoGroup;
	class GPUTexturePool;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GPUTexturePoolTextureInfo: a structure containing all information for a texture in GPU memory
	*/
	
	class CHAOS_API GPUTexturePoolTextureInfo
	{

	public:

		/** the resource id */
		GLuint texture_id = 0;
		/** the texture description */
		TextureDescription texture_description;
	};

	/**
	* GPUTexturePoolTextureInfoGroup: an entry that match several GPUTexturePoolTextureInfo to a GPUFence
	*/

	class CHAOS_API GPUTexturePoolTextureInfoGroup
	{
	public:

		/** the fence until which the textures are in used */
		shared_ptr<GPUFence> fence;
		/** the GPUTexturePoolTextureInfo attached to the fence */
		std::vector<GPUTexturePoolTextureInfo> textures_info;
	};

	/**
	* GPUTexturePool: a cache that contains several GPU textures with a pending fence (the resource is not valid until the fence is over)
	*/

	class CHAOS_API GPUTexturePool : public Tickable, public GPUDeviceResourceInterface
	{
	public:

		/** constructor */
		GPUTexturePool(GPUDevice * in_gpu_device);
		/** destructor */
		virtual ~GPUTexturePool();

		/** create a texture */
		GPUTexture * CreateTexture(TextureDescription const& in_texture_description);

		/** called whenever a texture is not required anymore */
		void OnTextureUnused(GPUTexture * in_texture);

		/** destroy all cached resources */
		void ClearPool();

	protected:

		/** search a texture in the pool */
		GPUTexture * DoFindCachedTexture(TextureDescription const& in_texture_description);
		/** create a new texture */
		GPUTexture * DoCreateTexture(TextureDescription const& in_texture_description);
		/** check whether GPUTexturePoolEntryData match requested texture */
		bool DoMatchRequestedTextureInfo(GPUTexturePoolTextureInfo & in_texture_info, TextureDescription const& in_texture_description) const;
		/** get the group corresponding to a given fence */
		GPUTexturePoolTextureInfoGroup * FindOrCreateTextureInfoGroup(GPUFence * in_fence);
		/** release texture */
		void ReleaseTexture(GLuint & in_texture_id) const;
		/** called whenever a texture resource is destroyed */
		void OnTextureDestroyed(GLuint in_texture_id) const;
		/** returns true whether we want to reuse the texture */
		bool WantToReuseTexture(GPUTexture * in_texture) const;

		/** override */
		virtual bool DoTick(float delta_time) override;

	protected:

		/** some GPUTextures per GPUFence */
		std::vector<GPUTexturePoolTextureInfoGroup> texture_info_groups;
	};

#endif

}; // namespace chaos