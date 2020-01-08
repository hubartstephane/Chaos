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

        //new_element.


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

        // make buffer progress and update pending_vertices_count
        char* result = buffer_start;
        buffer_start += required_size;
        pending_vertices_count += primitive_vertices_count;
        return result;
    }

    char* PrimitiveOutputBase::ReserveBuffer(size_t required_size)
    {



        return nullptr;
    }

}; // namespace chaos
