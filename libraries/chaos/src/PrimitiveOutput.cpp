#include <chaos/PrimitiveOutput.h>

#include <chaos/GPUDrawPrimitive.h>
#include <chaos/GPUDynamicMesh.h>

namespace chaos
{
    void PrimitiveOutputBase::Flush()
    {
        // skip empty primitive
        if (pending_vertices_count <= 0 || buffer_start == nullptr)
            return;
        // one new GPU buffer each time so primitives need to be flushed => one GPUDynamicMeshElement each time
        GPUDynamicMeshElement& new_element = dynamic_mesh->AddMeshElement();
        
        new_element.cached_buffer = cached_buffer;
        new_element.vertex_declaration = *vertex_declaration;        
        new_element.render_material = nullptr; // XXX : the used material will be given by ParticleLayer each frame so that if we change Layer::Material, the dynamic mesh will be updated too


        new_element.cached_buffer.buffer->UnMapBuffer();


        // insert the primitive into the element
        GPUDrawPrimitive primitive;
        primitive.primitive_type = primitive_gl_type;
        primitive.indexed = false;
        primitive.count = (int)pending_vertices_count;
        primitive.start = 0;
        primitive.base_vertex_index = 0;
        new_element.primitives.push_back(primitive);
        // clear internals
        buffer_start = nullptr;
        buffer_end = nullptr;
        pending_vertices_count = 0;
        cached_buffer = GPUBufferCacheEntry();
    }

    char* PrimitiveOutputBase::GeneratePrimitive(size_t required_size)
    {
        assert(required_size > 0);

        // flush primitive and allocate memory if necessary
        if (buffer_start == nullptr || buffer_start + required_size >= buffer_end)
        {
            if (buffer_start != nullptr)
                Flush();
            if (ReserveBuffer(required_size) == nullptr)
                return nullptr;
        }

        assert(buffer_start != nullptr);

        // make buffer progress and update pending_vertices_count
        char* result = buffer_start;
        buffer_start += required_size;
        pending_vertices_count += vertices_per_primitive;
        return result;
    }

    char* PrimitiveOutputBase::ReserveBuffer(size_t required_size)
    {
        // compute the real size to allocate
        size_t count = std::max(vertex_requirement_evaluation, required_size / vertex_size);
        if (count == 0)
            count = 100 * vertices_per_primitive; // a default buffer of 100 primitives

        size_t bufsize = count * vertex_size;

        assert(buffer_cache != nullptr);
        buffer_cache->GetBuffer(bufsize, cached_buffer);

        buffer_start = cached_buffer.buffer->MapBuffer(0, 0, false, true);
        buffer_end = buffer_start + bufsize;

        return buffer_start;
    }

}; // namespace chaos
