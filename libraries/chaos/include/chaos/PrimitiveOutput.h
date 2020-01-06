#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/GPUClasses.h>

namespace chaos
{
    /**
     * PrimitiveType : the type of primitives that can be rendered
     */

    enum class PrimitiveType
    {
        triangle = 0,
        triangle_pair = 1,
        quad = 2,
        triangle_strip = 3,
        triangle_fan = 4,
    };

    /**
     * PrimitiveBase : base object for writing GPU primitives into memory (GPU mapped memory for the usage) 
     */

    template<size_t VERTICES_COUNT = 0>
    class PrimitiveBase
    {
    public:

        /** base constructor */
        PrimitiveBase() = default;
        /** copy constructor */
        PrimitiveBase(PrimitiveBase const& src) = default;
        /** initialization constructor */
        PrimitiveBase(char* in_buffer, size_t in_vertex_size, size_t in_vertices_count) :
            buffer(in_buffer),
            vertex_size(in_vertex_size),
            vertices_count(in_vertices_count)
        {
            assert(in_buffer != nullptr);
            assert(in_vertex_size > 0);
            // xxx : do not test for vertices_count. At this point 0, is a valid value for Fans and Strips
        }

        /** gets the buffer for this primitive */
        char* GetBuffer() const { return buffer; }
        /** gets the size of one vertice of this primitive */
        size_t GetVertexSize() const { return vertex_size; }
        /** gets the number of vertices for this primitive */
        size_t GetVerticesCount() const { return vertices_count; }

    protected:

        /** the buffer where we write buffer */
        char* buffer = nullptr;
        /** the size of a vertex for this primitive */
        size_t vertex_size = 0;
        /** the number of vertices for this primitive */
        size_t vertices_count = VERTICES_COUNT;
    };

    /**
      * TypedPrimitiveBase : base object for writing GPU primitives into memory (vertex type is known)
      */

    template<typename VERTEX_TYPE, size_t VERTICES_COUNT = 0>
    class TypedPrimitiveBase : public PrimitiveBase<VERTICES_COUNT>
    {
    public:

        using vertex_type = VERTEX_TYPE;

        /** base constructor */
        TypedPrimitiveBase() = default;
        /** copy constructor */
        TypedPrimitiveBase(TypedPrimitiveBase const & src) = default;
        /** downcast constructor */
        template<typename OTHER_VERTEX_TYPE>
        TypedPrimitiveBase(TypedPrimitiveBase<OTHER_VERTEX_TYPE> const& src, std::enable_if_t<std::is_base_of_v<VERTEX_TYPE, OTHER_VERTEX_TYPE>, int> = 0) :
            TypedPrimitiveBase(src.GetBuffer(), src.GetVertexSize(), src.GetVerticesCount()) {}
        /** constructor */
        TypedPrimitiveBase(char* in_buffer, size_t in_vertex_size, size_t in_vertices_count) :
            PrimitiveBase(in_buffer, in_vertex_size, in_vertices_count) {}

        /** accessor */
        vertex_type & operator [](size_t index)
        {
            assert(index < vertices_count);
            return *((vertex_type*)(buffer + vertex_size * index));
        }

        /** const accessor */
        vertex_type const & operator [](size_t index) const
        {
            assert(index < vertices_count);
            return *((vertex_type const*)(buffer + vertex_size * index));
        }
    };

    /**
      * The usual existing primitives
      */

    // fixed length primitives
    template<typename VERTEX_TYPE> using TrianglePrimitive = TypedPrimitiveBase<VERTEX_TYPE, 3>;
    template<typename VERTEX_TYPE> using TrianglePairPrimitive = TypedPrimitiveBase<VERTEX_TYPE, 6>;
    template<typename VERTEX_TYPE> using QuadPrimitive = TypedPrimitiveBase<VERTEX_TYPE, 4>;
    // 0 for non-fixed vertices count
    template<typename VERTEX_TYPE> using TriangleStripPrimitive = TypedPrimitiveBase<VERTEX_TYPE, 0>;
    template<typename VERTEX_TYPE> using TriangleFanPrimitive = TypedPrimitiveBase<VERTEX_TYPE, 0>;   

    /**
     * PrimitiveOutputBase : a primitive generator (the base class)
     */

    class PrimitiveOutputBase
    {

    public:

        /** constructor */
        PrimitiveOutputBase(GPUDynamicMesh * in_dynamic_mesh, GPUBufferCache * in_buffer_cache, GPURenderer* in_renderer) :
            dynamic_mesh(in_dynamic_mesh),
            buffer_cache(in_buffer_cache),
            renderer(in_renderer)
        {
            assert(in_dynamic_mesh != nullptr);
            assert(in_renderer != nullptr);
        }

        /** gets the size of one vertice of the generated primitive */
        size_t GetVertexSize() const { return vertex_size; }

    protected:

        /** allocate a buffer for the primitive and register a new primitive */
        char* GeneratePrimitive(size_t required_size);

    protected:

        /** the dynamic mesh we are working on (to store primitives to render) */
        GPUDynamicMesh * dynamic_mesh = nullptr;
        /** a buffer cache */
        GPUBufferCache* buffer_cache = nullptr;
        /** the renderer used fence requests */
        GPURenderer* renderer = nullptr;

        /** start of currently allocated buffer */
        char* buffer_start = nullptr;
        /** end of currently allocated buffer */
        char* buffer_end = nullptr;
        /** size of a vertex */
        size_t vertex_size = 0;
    };

    /**
     * TypedPrimitiveOutputBase : generic primitive generator
     */

    template<typename VERTEX_TYPE, size_t VERTICES_COUNT = 0>
    class TypedPrimitiveOutputBase : public PrimitiveOutputBase
    {
    public:

        static constexpr size_t vertices_count = VERTICES_COUNT; // should be 0 for STRIPS & FANS

        using vertex_type = VERTEX_TYPE;

        using primitive_type = TypedPrimitiveBase<VERTEX_TYPE, vertices_count>;

        /** constructor */
        TypedPrimitiveOutputBase(GPUDynamicMesh* in_dynamic_mesh, GPUBufferCache* in_buffer_cache, GPURenderer* in_renderer) :
            PrimitiveOutputBase(in_dynamic_mesh, in_buffer_cache, in_renderer)
        {
            vertex_size = sizeof(vertex_type);
        }

        /** add a primitive */
        primitive_type AddPrimitive(size_t custom_vertices_count = 0)
        {
            assert((vertices_count == 0) ^ (custom_vertices_count == 0)); // STRIPS & FANS require a CUSTOM number of vertices, other requires a NON CUSTOM number of vertices



            // implementation for STRIPS or FANS
            if constexpr (vertices_count == 0)
            {
                primitive_type result;

                

                // TODO : implement fans and strips 
                assert(0);


                return result;
            }
            // implementation for fixed length primitives
            else
            {
                return primitive_type(
                    GeneratePrimitive(vertex_size * vertices_count),
                    vertex_size,
                    vertices_count
                );
            }
        }


    };

    /**
      * The usual existing primitives output's
      */

    // fixed length primitive
    template<typename VERTEX_TYPE> using TriangleOutput = TypedPrimitiveOutputBase<VERTEX_TYPE, 3>;
    template<typename VERTEX_TYPE> using TrianglePairOutput = TypedPrimitiveOutputBase<VERTEX_TYPE, 6>;
    template<typename VERTEX_TYPE> using QuadOutput = TypedPrimitiveOutputBase<VERTEX_TYPE, 4>;
    // 0 for non-fixed vertices count
    template<typename VERTEX_TYPE> using TriangleFanOutput = TypedPrimitiveOutputBase<VERTEX_TYPE, 0>;   
    template<typename VERTEX_TYPE> using TriangleStripOutput = TypedPrimitiveOutputBase<VERTEX_TYPE, 0>;


}; // namespace chaos

