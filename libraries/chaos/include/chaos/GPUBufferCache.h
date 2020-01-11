#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUClasses.h>
#include <chaos/GPUResource.h>

namespace chaos
{
    /**
     * GPUBufferCacheEntries : an entry that match several GPUBuffers to a GPUFence
     */

    class GPUBufferCacheEntries
    {
        friend class GPUBufferCache;

        static size_t const BUFFER_ACCEPT_RATIO = 25; // if the buffer considered is greater less than 25% of required size, we can keep it

    protected:

        /** returns the best cached buffer for the given size (returns true whether their is no need to search for another buffer) */
         bool GetBuffer(size_t required_size, shared_ptr<GPUBuffer> & result);

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

    protected:

        /** get the cache entry for given fence */
        GPUBufferCacheEntries* GetCacheEntryForFence(GPUFence* fence);
        /** create a buffer and register it for a given fence */
        bool CreateBuffer(size_t required_size, shared_ptr<GPUBuffer>& result);

    protected:

        /** some GPUBuffers per GPUFence */
        std::vector<GPUBufferCacheEntries> entries;
    };


}; // namespace chaos
