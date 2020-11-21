#include <chaos/Chaos.h>

namespace chaos
{
    GPUDynamicMeshElement & GPUDynamicMesh::AddMeshElement()
    {
        return elements.emplace_back();
    }

    void GPUDynamicMesh::Clear(GPUBufferCache* buffer_cache)
    {
        // XXX : shu on pourrait aussi donner les IndexBuffer => ATTENTION, l index buffer GPURenderer::QUADIndexBuffer ne doit pas etre donné !



        if (buffer_cache != nullptr) // give buffers to cache if we want that
            for (GPUDynamicMeshElement& element : elements)
                buffer_cache->GiveBuffer(element.vertex_buffer, last_rendered_fence.get());
        elements.clear();
        last_rendered_fence = nullptr;
    }

    int GPUDynamicMesh::DoDisplay(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params)
    {
        // create a vertex array cache if necessary
        if (vertex_array_cache == nullptr)
            vertex_array_cache = new GPUVertexArrayCache;
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
            GPUVertexArray const* vertex_array = vertex_array_cache->FindOrCreateVertexArray(renderer, program, element.vertex_buffer.get(), element.index_buffer.get(), element.vertex_declaration.get(), 0);
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
            // XXX : make code slower : 
            //last_rendered_fence = renderer->GetCurrentFrameFence();
            glBindVertexArray(0);
        }
        return result;
    }

}; // namespace chaos

