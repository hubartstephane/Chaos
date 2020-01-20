#include <chaos/GPUBufferCache.h>
#include <chaos/GPUFence.h>
#include <chaos/GPUBuffer.h>
#include <chaos/GPURenderer.h>

namespace chaos
{

    bool GPUBufferCacheEntries::GetBuffer(size_t required_size, shared_ptr<GPUBuffer>& result) // use shared pointer to avoid the buffer destruction when removed from vector<>
    {
        assert(required_size > 0);

        size_t max_accepted_size = (size_t)((1.0f + (float)BUFFER_ACCEPT_RATIO) * (float)required_size);

        size_t count = buffers.size();
        for (size_t i = 0; i < count; ++i)
        {
            GPUBuffer * buffer = buffers[i].get();

            // buffer too small ? 
            size_t buffer_size = buffer->GetBufferSize();
            if (buffer_size < required_size)            
                continue;
            // or too big ?
            if (0 && buffer_size > max_accepted_size) // we do not want to waste to much memory => that why we use a max_accepted_size
                continue;
            // buffer found            
            result = buffer;
            buffers.erase(buffers.begin() + i);
            return true;
        }
        return false;
    }

    bool GPUBufferCache::GiveBuffer(shared_ptr<GPUBuffer> & buffer, GPUFence* fence)
    {
        GPUBufferCacheEntries* cache_entry = GetCacheEntryForFence(fence);
        if (cache_entry == nullptr)
            return false;
        cache_entry->buffers.push_back(buffer);
        return true;
    }

    bool GPUBufferCache::GetBuffer(size_t required_size, shared_ptr<GPUBuffer> & result)
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
                return true;
        }
        return CreateBuffer(required_size, result);
    }

    GPUBufferCacheEntries* GPUBufferCache::GetCacheEntryForFence(GPUFence* fence)
    {
        // maybe an entry for this fence already exists
        for (GPUBufferCacheEntries &entry : entries)
            if (entry.fence == fence)
                return &entry;
        // create a new entry for incomming fence
        GPUBufferCacheEntries new_entry;
        new_entry.fence = fence;
        entries.push_back(std::move(new_entry));
        return &entries[entries.size() - 1];
    }

    bool GPUBufferCache::CreateBuffer(size_t required_size, shared_ptr<GPUBuffer> & result)
    {
        // create a dynamic buffer
        result = new GPUBuffer(true);
        if (result == nullptr)
            return false;
        result->SetBufferData(nullptr, required_size); // no allocation, but set buffer size forever
        return true;
    }

}; // namespace chaos

