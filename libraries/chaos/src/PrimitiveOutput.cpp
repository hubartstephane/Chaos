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
        // transform quads into triangle pair      
        if (type == PrimitiveType::quad)
        {
            char* start = buffer_start;

            size_t offset2 = vertex_size * 2;
            size_t offset4 = vertex_size * 4;
            size_t offset5 = vertex_size * 5;
            size_t offset6 = vertex_size * 6;

            size_t count = ((buffer_position - buffer_start) / vertex_size) / 6;
            for (size_t i = 0; i < count; ++i)
            {
                //_mm_prefetch(start + offset6, 0);

                memcpy(start + offset4, start, vertex_size);
                memcpy(start + offset5, start + offset2, vertex_size);
                start += offset6;
            }
        }
        // one new GPU buffer each time so primitives need to be flushed => one GPUDynamicMeshElement each time
        GPUDynamicMeshElement& new_element = dynamic_mesh->AddMeshElement();
        
        new_element.vertex_buffer = vertex_buffer;
        new_element.vertex_declaration = vertex_declaration;        
        new_element.render_material = nullptr; // XXX : the used material will be given by ParticleLayer each frame so that if we change Layer::Material, the dynamic mesh will be updated too
      
        new_element.vertex_buffer->UnMapBuffer();
        new_element.vertex_buffer->SetBufferData(nullptr, new_element.vertex_buffer->GetBufferSize()); // orphan the buffer

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
        vertex_buffer = nullptr;
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
        size_t count = 0;        
        if (vertex_requirement_evaluation != 0)
            count = std::max(vertex_requirement_evaluation, required_size / vertex_size);
        else
            count = std::max(100 * real_vertices_per_primitive, required_size / vertex_size); // a default buffer of 100 primitives
        // compute buffer size and allocate a buffer
        size_t bufsize = count * vertex_size;         
        if (!buffer_cache->GetBuffer(bufsize, vertex_buffer))
            return nullptr;
        // map the buffer
        buffer_start = vertex_buffer->MapBuffer(0, 0, false, true);
        buffer_end = buffer_start + vertex_buffer->GetBufferSize();
        buffer_position = buffer_start;

        return buffer_position;
    }

}; // namespace chaos
