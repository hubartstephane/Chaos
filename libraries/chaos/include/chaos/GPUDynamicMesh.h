#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/GPUClasses.h>
#include <chaos/GPURenderable.h>
#include <chaos/GPUBufferCache.h>
#include <chaos/GPUFence.h>


namespace chaos
{
    /**
     * GPUDynamicMeshElement : a renderable element
     */

    class GPUDynamicMeshElement
    {
    public:


        /** the 'vertex buffer' (+ mapping data) */
        GPUBufferCacheEntry cached_buffer;
    };

    /**
     * GPUDynamicMesh : a mesh to be dynamicly constructed
     */

    class GPUDynamicMesh : public GPURenderable
    {

    public:

        /** remove all elements of the rendering (may give GPUBuffers back to a GPUBufferCache) */
        void Clear(GPUBufferCache* buffer_cache);

    protected:

        /** override */
        virtual int DoDisplay(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params);

    protected:

        /** the element to render */
        std::vector<GPUDynamicMeshElement> elements;
        /** the last time the dynamic mesh was rendered with current elements */
        shared_ptr<GPUFence> last_rendered_fence;
    };

}; // namespace chaos

