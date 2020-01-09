#include <chaos/GPUDynamicMesh.h>

#include <chaos/GPURenderer.h>
#include <chaos/GPUBufferCache.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/GPUFence.h>
#include <chaos/GPURenderParams.h>

namespace chaos
{
    GPUDynamicMeshElement & GPUDynamicMesh::AddMeshElement()
    {
        GPUDynamicMeshElement&result = elements.emplace_back();


        return result;
        return elements.emplace_back();
    }

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
        // display the elements
        int result = 0;       
        for (GPUDynamicMeshElement & element : elements)
        {
            // early skip
            if (element.primitives.size() == 0)
                continue;
            // setup draw context
            GPURenderMaterial const * final_material = render_params.GetMaterial(this, element.render_material.get());
            if (final_material == nullptr)
                continue;
            GPUProgram const * program = final_material->UseMaterial(uniform_provider, render_params);
            if (program == nullptr)
                continue;
            GPUVertexArray const * vertex_array = element.vertex_array_cache.FindOrCreateVertexArray(program, element.cached_buffer.buffer.get(), nullptr, element.vertex_declaration, 0);
            if (vertex_array == nullptr)
                continue;

            glBindVertexArray(vertex_array->GetResourceID());

            // draw all primitives
            for (GPUDrawPrimitive const& primitive : element.primitives)
            {
                if (primitive.count <= 0)
                    continue;
                renderer->Draw(primitive, render_params.instancing);
                ++result;
            }
        }
        // store fence for this last rendering time (only if some draw call has been made)
        if (result > 0)
        {
            last_rendered_fence = renderer->GetCurrentFrameFence();
            glBindVertexArray(0);
        }
        return result;
    }

}; // namespace chaos

