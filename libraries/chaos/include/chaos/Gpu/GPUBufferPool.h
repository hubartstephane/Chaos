namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPUBufferPoolBufferInfo;
	class GPUBufferPoolBufferInfoGroup;
	class GPUBufferPool;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* GPUBufferPoolBufferInfo: a structure containing all information for a buffer in GPU memory
	*/
	
	class CHAOS_API GPUBufferPoolBufferInfo
	{

	public:

		/** the resource id */
		GLuint buffer_id = 0;
		/** the size of the buffer */
		size_t buffer_size = 0;
		/** flags */
		GPUBufferFlags buffer_flags = GPUBufferFlags::None;
	};

	/**
	* GPUBufferPoolBufferInfoGroup: an entry that match several GPUBufferPoolBufferInfo to a GPUFence
	*/

	class CHAOS_API GPUBufferPoolBufferInfoGroup
	{
	public:

		/** the fence until which the buffers are in used */
		shared_ptr<GPUFence> fence;
		/** the GPUBufferPoolBufferInfo attached to the fence */
		std::vector<GPUBufferPoolBufferInfo> buffers_info;
	};

	/**
	* GPUBufferPool: a cache that contains several GPU buffers with a pending fence (the resource is not valid until the fence is over)
	*/

	class CHAOS_API GPUBufferPool : public Tickable, public GPUDeviceResourceInterface
	{
	public:

		/** constructor */
		GPUBufferPool(GPUDevice * in_gpu_device);
		/** destructor */
		virtual ~GPUBufferPool();

		/** create a buffer */
		GPUBuffer * CreateBuffer(size_t in_buffer_size, GPUBufferFlags in_flags);
		/** called whenever a buffer is not required anymore */
		void OnBufferUnused(GPUBuffer * in_buffer);

		/** destroy all cached resources */
		void ClearPool();

	protected:
		/** search a buffer in the pool */
		GPUBuffer * DoFindCachedBuffer(size_t in_buffer_size, GPUBufferFlags in_flags);
		/** create a new buffer */
		GPUBuffer * DoCreateBuffer(size_t in_buffer_size, GPUBufferFlags in_flags);
		/** check whether GPUBufferPoolEntryData match requested buffer */
		bool DoMatchRequestedBufferInfo(GPUBufferPoolBufferInfo & in_buffer_info, size_t in_buffer_size, GPUBufferFlags in_flags) const;
		/** get the group corresponding to a given fence */
		GPUBufferPoolBufferInfoGroup * FindOrCreateBufferInfoGroup(GPUFence * in_fence);
		/** release the buffer */
		void ReleaseBuffer(GLuint & in_buffer_id) const;
		/** called whenever a buffer resource is destroyed */
		void OnBufferDestroyed(GLuint in_buffer_id) const;
		/** returns true whether we want to reuse the buffer */
		bool WantToReuseBuffer(GPUBuffer * in_buffer) const;

		/** override */
		virtual bool DoTick(float delta_time) override;

	protected:

		/** some GPUBuffers per GPUFence */
		std::vector<GPUBufferPoolBufferInfoGroup> buffer_info_groups;

		/** percentage of requested size above which a candidate buffer is rejected (to avoid waste) (0 if ignored) */
		size_t rejected_size_percentage = 50;
	};

#endif

}; // namespace chaos