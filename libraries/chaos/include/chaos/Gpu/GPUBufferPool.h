#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GPUBufferPoolEntry;
	class GPUBufferPool;

}; // namespace chaos

#else 

namespace chaos
{


	class GPUBufferPoolEntry
	{
	public:

		/** the fence after which all buffers are available */
		weak_ptr<GPUFence> fence;
		/** the buffers */
		std::vector<weak_ptr<GPUBuffer>> buffers;

	};

	class GPUBufferPool : public Object
	{
	public:

		/** allocate a buffer and bind it to a fence */
		GPUBuffer * AllocateBuffer(GPURenderer * renderer, size_t min_buffer_size, size_t buffer_size);

	protected:

		/** internal method to find an available buffer */
		GPUBuffer * FindUnusedBuffer(GPURenderer * renderer, size_t min_buffer_size, size_t buffer_size);
		

	protected:

		/** the buffers that are stored */
		std::vector<GPUBufferPoolEntry> entries;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION



