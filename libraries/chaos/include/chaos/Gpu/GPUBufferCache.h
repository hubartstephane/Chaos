#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
    class GPUBufferCacheEntries;
    class GPUBufferCache;

}; // namespace chaos

#else 

namespace chaos
{

    /**
     * GPUBufferCacheEntries : an entry that match several GPUBuffers to a GPUFence
     */

    class GPUBufferCacheEntries
    {
        friend class GPUBufferCache;

    protected:

        /** returns the best cached buffer for the given size (returns true whether their is no need to search for another buffer) */
         bool GetBuffer(size_t required_size, size_t max_accepted_size, shared_ptr<GPUBuffer> & result);

    protected:

        /** the fence until which the buffers are in used */
        shared_ptr<GPUFence> fence;
        /** the buffers attached to the fence */
        std::vector<shared_ptr<GPUBuffer>> buffers;
    };

    /**
     * GPUBufferCache : a cache that contains several GPU buffers with a pending fence (the resource is not valid until the fence is over)
     */

    class GPUBufferCache
    {
    public:

        /** get a buffer of required size (looking for cached resources first) */
        bool GetBuffer(size_t required_size, shared_ptr<GPUBuffer>& result);
        /** give back a buffer to this cache to be used later */
        bool GiveBuffer(shared_ptr<GPUBuffer>& buffer, GPUFence* fence);
        /** create a buffer */
        static bool CreateBuffer(size_t required_size, shared_ptr<GPUBuffer>& result);

    protected:

        /** get the cache entry for given fence */
        GPUBufferCacheEntries* GetCacheEntryForFence(GPUFence* fence);

    public:

        /** percentage of requested size above which a candidate buffer is rejected (to avoid waste) (0 if ignored) */
        size_t rejected_size_percentage = 50;

    protected:

        /** some GPUBuffers per GPUFence */
        std::vector<GPUBufferCacheEntries> entries;
    };

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION


