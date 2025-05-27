namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class GPUBufferFlags;
	enum class GPUBufferMapFlags;

	class GPUBuffer;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * GPUBufferFlags: indicates the properties of the buffer
	 */

	enum class CHAOS_API GPUBufferFlags : int
	{
		None = 0,
		Dynamic = 1
	};

	CHAOS_DECLARE_ENUM_BITMASK_METHOD(GPUBufferFlags, CHAOS_API);

	/**
	 * GPUBufferMapFlags: flags for mapping buffers
	 */

	enum class CHAOS_API GPUBufferMapFlags : int
	{
		Read = 1,
		Write = 2
	};

	CHAOS_DECLARE_ENUM_BITMASK_METHOD(GPUBufferMapFlags, CHAOS_API);

	/** 
	 * GPUBuffer: self explaning
	 */

	class CHAOS_API GPUBuffer : public GPUResource, public GPUDeviceResourceInterface
	{
		friend class GPUBufferPool;

	public:

		/** destructor */
		virtual ~GPUBuffer();

		/** get the size of the buffer */
		size_t GetBufferSize() const;
		/** get the flags of the buffer */
		GPUBufferFlags GetBufferFlags() const;
		/** returns whether the buffer has been mapped */
		bool IsMapped() const { return mapped; }

		/** update the data of the buffer */
		bool SetBufferData(void const* in_data, size_t in_start, size_t in_size);
		/** map the buffer */
		char* MapBuffer(size_t in_start = 0, size_t in_size = 0, GPUBufferMapFlags in_flags = GPUBufferMapFlags::Write);
		/** unmap the buffer */
		void UnMapBuffer();

		/** returns the GL name of the resource */
		GLuint GetResourceID() const { return buffer_id; }
		/** returns true whether the resource is valid */
		bool IsValid() const;

		/** override */
		virtual void OnLastReferenceLost() override;

	protected:

		/** constructor */
		GPUBuffer(GPUDevice * in_gpu_device, GLuint in_buffer_id, size_t in_buffer_size, GPUBufferFlags in_flags);

		/** check whether mapping range is valid. update parameters if necessary */
		bool CheckAndComputeBufferRange(size_t & inout_start, size_t & inout_size) const;

	protected:

		/** the resource id */
		GLuint buffer_id = 0;
		/** the size of the buffer */
		size_t buffer_size = 0;
		/** flags */
		GPUBufferFlags flags = GPUBufferFlags::None;
		/** whether the buffer is mapped */
		bool mapped = false;
	};
#endif

}; // namespace chaos