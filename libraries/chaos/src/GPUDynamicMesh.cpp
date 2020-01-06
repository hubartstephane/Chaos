#include <chaos/GPUDynamicMesh.h>

#include <chaos/GPURenderer.h>
#include <chaos/GPUBufferCache.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/GPUFence.h>

namespace chaos
{

    void GPUDynamicMesh::Clear(GPUBufferCache* buffer_cache)
    {
        if (buffer_cache != nullptr) // give buffers to cache if we want that
            for (GPUDynamicMeshElement& element : elements)
                buffer_cache->GiveBuffer(element.cached_buffer, last_rendered_fence.get());
        elements.clear();
        last_rendered_fence = nullptr;
    }

    int GPUDynamicMesh::DoDisplay(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params)
    {
        // store fence for this last rendering time
        last_rendered_fence = renderer->GetCurrentFrameFence();
        // display the elements



        return 0;
    }




}; // namespace chaos

