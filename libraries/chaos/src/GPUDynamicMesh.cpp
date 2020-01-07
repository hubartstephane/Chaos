#include <chaos/GPUDynamicMesh.h>

#include <chaos/GPURenderer.h>
#include <chaos/GPUBufferCache.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/GPUFence.h>
#include <chaos/GPURenderParams.h>

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
        int result = 0;
       
        for (GPUDynamicMeshElement& element : elements)
        {
            GPURenderMaterial const * final_material = nullptr;
            GPUProgram const * program = nullptr;
            GPUVertexArray const * vertex_array = nullptr;

            for (GPUDrawPrimitive const& primitive : element.primitives)
            {
                // empty primitive
                if (primitive.count <= 0)
                    continue;
                // search the material
                if (final_material == nullptr)
                {
                    final_material = render_params.GetMaterial(this, element.render_material.get());
                    if (final_material == nullptr)
                        break;
                }
                // search the program
                if (program == nullptr)
                {
                    program = final_material->UseMaterial(uniform_provider, render_params);
                    if (program == nullptr)
                        break;
                }
                // get the vertex array
                if (vertex_array == nullptr)
                {
                  //  vertex_array = element.vertex_array_cache.FindOrCreateVertexArray(program, rd.cached_vertex_buffer.buffer.get(), nullptr, vertex_declaration, 0);
                    if (vertex_array == nullptr)
                        break;
                }


            }
        }
        return result;
    }


#if 0










    GPUFence* fence = nullptr;

    GLenum gl_primitive_type = GetGLPrimitiveType();

    int result = 0;

    GPUProgram const* program = nullptr;
    for (ParticleLayerBaseRenderData& rd : render_data)
    {
        // really rendering something
        if (rd.vertices_count == 0)
            continue;
        // search the program
        if (program == nullptr)
        {
            program = final_material->UseMaterial(uniform_provider, render_params);
            if (program == nullptr)
                return 0;
        }
        // get the vertex array
        GPUVertexArray const* vertex_array = rd.vertex_array_cache.FindOrCreateVertexArray(program, rd.cached_vertex_buffer.buffer.get(), nullptr, vertex_declaration, 0);
        if (vertex_array == nullptr)
            continue;

        glBindVertexArray(vertex_array->GetResourceID());

        // one draw call for the whole buffer
        GPUDrawPrimitive primitive;
        primitive.primitive_type = gl_primitive_type;
        primitive.indexed = false;
        primitive.count = (int)rd.vertices_count;
        primitive.start = 0;
        primitive.base_vertex_index = 0;
        renderer->Draw(primitive, render_params.instancing);

        // mark the re buffer as used until the current fence is over
        if (fence == nullptr)
            fence = renderer->GetCurrentFrameFence();
        rd.fence = fence;

        ++result;
    }
    // restore previous state (only if anyone changed it)
    if (result > 0)
        glBindVertexArray(0);
    return result;
#endif





}; // namespace chaos

