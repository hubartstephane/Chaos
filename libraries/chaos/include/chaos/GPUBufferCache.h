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



    public:

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
        bool GetBuffer(size_t required_size, GPURenderParams& render_params, shared_ptr<GPUBuffer>& result);


    protected:

        /** some GPUBuffers per GPUFence */
        std::vector<GPUBufferCacheEntries> entries;
    };


}; // namespace chaos
