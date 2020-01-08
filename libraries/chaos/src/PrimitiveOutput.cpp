#include <chaos/PrimitiveOutput.h>

#include <chaos/GPUDrawPrimitive.h>
#include <chaos/GPUDynamicMesh.h>

namespace chaos
{

    void PrimitiveOutputBase::Flush()
    {
        // skip empty primitive
        if (pending_vertices_count <= 0)
            return;
        // one new GPU buffer each time so primitives need to be flushed => one GPUDynamicMeshElement each time
        GPUDynamicMeshElement& new_element = dynamic_mesh->AddMeshElement();


        // insert the primitive into the element
        GPUDrawPrimitive primitive;
        primitive.primitive_type = primitive_gl_type;
        primitive.indexed = false;
        primitive.count = (int)pending_vertices_count;
        primitive.start = 0;
        primitive.base_vertex_index = 0;
        new_element.primitives.push_back(primitive);
        // clear the pending number of elements
        pending_vertices_count = 0;
    }

    char* PrimitiveOutputBase::GeneratePrimitive(size_t required_size)
    {
        assert(required_size > 0);

        // enought memory reserved for this primitive
        if (buffer_start + required_size <= buffer_end)
        {
            char* result = buffer_start;
            buffer_start += required_size;
            pending_vertices_count += primitive_vertices_count;
            return result;
        }

#if 0
        GPUDrawPrimitive primitive;
        primitive.primitive_type = GL_TRIANGLES;
        primitive.indexed = false;
        primitive.count = (int)vertices_count;
        primitive.start = 0;
        primitive.base_vertex_index = 0;
#endif




        return new char[required_size];
    }

}; // namespace chaos
