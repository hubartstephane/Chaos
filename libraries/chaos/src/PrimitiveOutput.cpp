#include <chaos/PrimitiveOutput.h>

#include <chaos/GPUDrawPrimitive.h>
#include <chaos/GPUDynamicMesh.h>

namespace chaos
{
    void PrimitiveOutputBase::Flush()
    {
        // skip empty primitive
        if (buffer_start == nullptr || buffer_position == buffer_start)
            return;
        // one new GPU buffer each time so primitives need to be flushed => one GPUDynamicMeshElement each time
        GPUDynamicMeshElement& new_element = dynamic_mesh->AddMeshElement();
        
        new_element.cached_buffer = cached_buffer;
        new_element.vertex_declaration = vertex_declaration;        
        new_element.render_material = nullptr; // XXX : the used material will be given by ParticleLayer each frame so that if we change Layer::Material, the dynamic mesh will be updated too
      
        new_element.cached_buffer.buffer->UnMapBuffer();

        // insert the primitive into the element
        GPUDrawPrimitive primitive;
        primitive.primitive_type = primitive_gl_type;
        primitive.indexed = false;
        primitive.count = (int)((buffer_position - buffer_start) / vertex_size);
        primitive.start = 0;
        primitive.base_vertex_index = 0;
        new_element.primitives.push_back(primitive);
        // clear internals
        buffer_start = nullptr;
        buffer_position = nullptr;
        buffer_end = nullptr;
        cached_buffer = GPUBufferCacheEntry();
    }

    char* PrimitiveOutputBase::GeneratePrimitive(size_t required_size)
    {
        // flush primitive and allocate memory if necessary
        if (buffer_start == nullptr || buffer_position + required_size >= buffer_end)
        {
            if (buffer_start != nullptr)
                Flush();
            if (ReserveBuffer(required_size) == nullptr)
                return nullptr;
        }
        // make buffer progress and update pending_vertices_count
        char* result = buffer_position;
        buffer_position += required_size;
        return result;
    }

    char* PrimitiveOutputBase::ReserveBuffer(size_t required_size)
    {
        // compute/evaluate number of vertices to use for the buffer
        size_t count = std::max(vertex_requirement_evaluation, required_size / vertex_size);
        if (count == 0)
            count = 100 * vertices_per_primitive; // a default buffer of 100 primitives
        // compute buffer size and allocate a buffer
        size_t bufsize = count * vertex_size;
        buffer_cache->GetBuffer(bufsize, cached_buffer);
        // map the buffer
        buffer_start = cached_buffer.buffer->MapBuffer(0, 0, false, true);
        buffer_end = buffer_start + cached_buffer.buffer->GetBufferSize();
        buffer_position = buffer_start;

        return buffer_position;
    }

}; // namespace chaos
