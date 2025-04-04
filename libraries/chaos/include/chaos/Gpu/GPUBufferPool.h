namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

    class GPUBufferPoolEntries;
    class GPUBufferPool;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

    /**
      * GPUBufferPoolEntries : an entry that match several GPUBuffers to a GPUFence
      */

    class CHAOS_API GPUBufferPoolEntries
    {
        friend class GPUBufferPool;

    protected:

        /** returns the best cached buffer for the given size (returns true whether their is no need to search for another buffer) */
        bool GetBuffer(size_t required_size, size_t max_accepted_size, shared_ptr<GPUBuffer>& result);

    protected:

        /** the fence until which the buffers are in used */
        shared_ptr<GPUFence> fence;
        /** the buffers attached to the fence */
        std::vector<shared_ptr<GPUBuffer>> buffers;
    };

    /**
     * GPUBufferPool : a cache that contains several GPU buffers with a pending fence (the resource is not valid until the fence is over)
     */

    class CHAOS_API GPUBufferPool : public Object
    {
    public:

        /** get a buffer of required size (looking for cached resources first) */
        bool GetBuffer(size_t required_size, shared_ptr<GPUBuffer>& result);
        /** give back a buffer to this cache to be used later */
        bool GiveBuffer(GPUBuffer * buffer, GPUFence* fence);
        /** create a buffer */
        static bool CreateBuffer(size_t required_size, shared_ptr<GPUBuffer>& result);

    protected:

        /** get the cache entry for given fence */
        GPUBufferPoolEntries* GetCacheEntryForFence(GPUFence* fence);

    public:

        /** percentage of requested size above which a candidate buffer is rejected (to avoid waste) (0 if ignored) */
        size_t rejected_size_percentage = 50;

    protected:

        /** some GPUBuffers per GPUFence */
        std::vector<GPUBufferPoolEntries> entries;
    };

#endif

}; // namespace chaos