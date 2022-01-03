#include <chaos/Chaos.h>

namespace chaos
{
    void GPUMesh::SetVertexArrayCache(GPUVertexArrayCache* in_vertex_array_cache)
    {
        vertex_array_cache = in_vertex_array_cache;
    }

    GPUMeshElement & GPUMesh::AddMeshElement()
    {
        return elements.emplace_back();
    }

    void GPUMesh::Clear(GPUBufferPool* buffer_pool)
    {
        // XXX : shu on pourrait aussi donner les IndexBuffer => ATTENTION, l index buffer GPURenderer::QUADIndexBuffer ne doit pas etre donné !

        if (buffer_pool != nullptr) // give buffers to pool if we want that
        {
            size_t count = elements.size();
            for (size_t i = 0; i < count; ++i)
            {
                GPUMeshElement const& element = elements[i];
                if (element.vertex_buffer != nullptr)
                {
                    // we don't want to give the buffer twice to the pool
                    // in reverse order because this is more likely to find the same buffer just before this
                    size_t j = i;
                    for (; j > 0; --j)
                        if (elements[j - 1].vertex_buffer == element.vertex_buffer)
                            break;
                    if (j == 0)
                    {
                        assert(!element.vertex_buffer->IsMapped());
                        buffer_pool->GiveBuffer(element.vertex_buffer.get(), last_rendered_fence.get());
                    }
                }
            }
        }
        elements.clear();
        last_rendered_fence = nullptr;
    }

    int GPUMesh::DoDisplay(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params)
    {
        // create a vertex array cache if necessary
        if (vertex_array_cache == nullptr)
            vertex_array_cache = new GPUVertexArrayCache;
        // display the elements
        int result = 0;
        for (GPUMeshElement & element : elements)
        {
            // early skip
            if (element.primitives.size() == 0)
                continue;
            // setup draw context
            GPURenderMaterial const * final_material = render_params.GetMaterial(this, element.render_material.get());
            if (final_material == nullptr)
                continue;
            GPUProgram const* program = final_material->UseMaterial(uniform_provider, render_params);
            if (program == nullptr)
                continue;
            GPUVertexArray const* vertex_array = vertex_array_cache->FindOrCreateVertexArray(renderer, program, element.vertex_buffer.get(), element.index_buffer.get(), element.vertex_declaration.get(), element.vertex_buffer_offset);
            if (vertex_array == nullptr)
                continue;

            GLuint vertex_array_id = vertex_array->GetResourceID();
            glBindVertexArray(vertex_array_id);

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
        // XXX : make code slower

        //if (result > 0)
        // last_rendered_fence = renderer->GetCurrentFrameFence(); 

        // restore an 'empty' state
        glUseProgram(0);
        glBindVertexArray(0);
        return result;
    }

    int GPUMesh::DisplayWithMaterial(GPURenderMaterial const* material, GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params)
    {
		DisableReferenceCount<GPUConstantMaterialProvider> material_provider(material);  // while on stack, use DisableReferenceCount<...>
		GPURenderParams other_render_params = render_params;
		other_render_params.material_provider = &material_provider;
		return Display(renderer, uniform_provider, other_render_params);
    }

    int GPUMesh::DisplayWithProgram(GPUProgram const* program, GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params)
    {
        return DisplayWithMaterial(program->GetDefaultMaterial(), renderer, uniform_provider, render_params);
    }

}; // namespace chaos

