#include <chaos/GPUBufferCache.h>
#include <chaos/GPUFence.h>
#include <chaos/GPUBuffer.h>
#include <chaos/GPURenderer.h>

namespace chaos
{

    bool GPUBufferCacheEntries::GetBuffer(size_t required_size, shared_ptr<GPUBuffer> & result)
    {
        assert(required_size > 0);

        size_t max_accepted_size = (size_t)((1.0f + (float)BUFFER_ACCEPT_RATIO) * (float)required_size);

        size_t count = buffers.size();
        for (size_t i = 0; i < count; ++i)
        {
            GPUBuffer* buffer = buffers[i].get();
            // buffer too small ? or too big ?
            size_t buffer_size = buffer->GetBufferSize();
            if (buffer_size < required_size || buffer_size > max_accepted_size) // we do not want to waste to much memory => that why we use a max_accepted_size
                continue;
            // buffer found            
            result = buffer;
            buffers.erase(buffers.begin() + i);
            return true;
        }
        return false;
    }

    bool GPUBufferCache::GetBuffer(size_t required_size, GPURenderer* renderer, shared_ptr<GPUBuffer>& result)
    {
        assert(required_size > 0);

        // the buffer is ordered from older FENCE to youngest FENCE
        for (size_t i = 0; i < entries.size(); ++i)
        {
            GPUBufferCacheEntries& entry = entries[i];
            if (entry.fence != nullptr)
            {
                // as soon as a FENCE exists and is not completed yet, there is no need to search further in the array
                // none of the other are completed	
                if (entry.fence->WaitForCompletion(0.0f))
                    entry.fence = nullptr;
                else
                    break; 
            }
            // search a buffer valid for given fence
            entry.GetBuffer(required_size, result);
            // remove empty entries
            if (entry.buffers.size() == 0)
            {
                entries.erase(entries.begin() + i);
                --i;
            }
            // return the buffer if OK
            if (result != nullptr)
            {
                if (renderer != nullptr) // renderer not null => buffer is given back once frame is finished
                {
                    GPUBufferCacheEntries* entries = GetCacheEntryForFence(renderer->GetCurrentFrameFence()); // as soon as current frame is rendered, the GPUBuffer will be available for others
                    if (entries != nullptr)
                        entries->buffers.push_back(result);
                }
                return true;
            }                
        }
        return CreateBuffer(required_size, renderer, result);
    }

    GPUBufferCacheEntries* GPUBufferCache::GetCacheEntryForFence(GPUFence* fence)
    {
        assert(fence != nullptr);

        // maybe an entry for this fence already exists : in that case it's necesseraly on top of the stack
        if (entries.size() > 0)
        {
            GPUBufferCacheEntries& last_entry = entries[entries.size() - 1];
            if (last_entry.fence == fence)
                return &last_entry;
        }
        // create a new entry for incomming fence
        GPUBufferCacheEntries new_entry;
        new_entry.fence = fence;
        entries.push_back(std::move(new_entry));
        return &entries[entries.size() - 1];
    }

    bool GPUBufferCache::CreateBuffer(size_t required_size, GPURenderer * renderer, shared_ptr<GPUBuffer>& result)
    {
        // create an entry for current fence (or get existing one)
        GPUBufferCacheEntries* entries = nullptr;
        if (renderer != nullptr)
        {
            entries = GetCacheEntryForFence(renderer->GetCurrentFrameFence());
            if (entries == nullptr)
                return nullptr;
        }
        // create a dynamic buffer
        result = new GPUBuffer(true);
        if (result == nullptr)
            return false;
        result->SetBufferData(nullptr, required_size); // no allocation, but set buffer size forever
        if (entries != nullptr)
            entries->buffers.push_back(result); // the buffer will be available as soon as current frame is over
        return true;
    }

}; // namespace chaos

