#include <chaos/GPUDynamicMesh.h>

#include <chaos/GPURenderer.h>
#include <chaos/GPUBufferCache.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/GPUFence.h>

namespace chaos
{

    void GPUDynamicMesh::Clear(GPUBufferCache* buffer_cache, GPUFence * fence)
    {
        assert(fence == nullptr || buffer_cache != nullptr); // fence cannot be null alone

        if (buffer_cache != nullptr) // give buffers to cache if we want that
            for (GPUDynamicMeshElement& element : elements)
                buffer_cache->GiveBuffer(element.cached_buffer, fence);
        elements.clear();
    }

    int GPUDynamicMesh::DoDisplay(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params)
    {

        return 0;
    }




}; // namespace chaos

