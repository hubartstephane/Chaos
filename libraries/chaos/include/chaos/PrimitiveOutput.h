#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/GPUClasses.h>
#include <chaos/GPUBuffer.h>
#include <chaos/GPUDrawPrimitive.h>
#include <chaos/GPUBufferCache.h>
#include <chaos/RawDataBufferIterator.h>
#include <chaos/RawDataBufferAccessor.h>

namespace chaos
{
    // QUAD
    // ----
    //     3      2
    //     +------+
    //     |      |
    //     |      |
    //     +------+
    //     0      1
    // 
    // TRIANGLE PAIR
    // -------------
    //
    //     5     2,4
    //     +------+
    //     |    / |
    //     | /    |
    //     +------+
    //    0,3     1
    // 

    /**
     * PrimitiveType : the type of primitives that can be rendered
     */

	enum class PrimitiveType : int
	{
		NONE = 0,
		TRIANGLE = 1,
		TRIANGLE_PAIR = 2,
		QUAD = 3,
		TRIANGLE_STRIP = 4,
		TRIANGLE_FAN = 5
	};

    /** returns the number of element per primitive (user accessible) */
    constexpr size_t GetVerticesPerParticle(PrimitiveType primitive_type)
    {
        if (primitive_type == PrimitiveType::TRIANGLE)
            return 3;
        if (primitive_type == PrimitiveType::TRIANGLE_PAIR)
            return 6;
        if (primitive_type == PrimitiveType::QUAD)
            return 4;
        return 0; // strip and fans have no defined values for this
    }

    /** returns the real number of element per primitive (the count in GPU buffer) */
    constexpr size_t GetRealVerticesPerParticle(PrimitiveType primitive_type)
    {
        return GetVerticesPerParticle(primitive_type);
    }

    /** returns the OpenGL primitive type corresponding to the primitive */
    constexpr GLenum GetGLPrimitiveType(PrimitiveType primitive_type)
    {
        if (primitive_type == PrimitiveType::TRIANGLE)
            return GL_TRIANGLES;
        if (primitive_type == PrimitiveType::TRIANGLE_PAIR)
            return GL_TRIANGLES;
        if (primitive_type == PrimitiveType::QUAD)
            return GL_TRIANGLES;
        if (primitive_type == PrimitiveType::TRIANGLE_STRIP)
            return GL_TRIANGLE_STRIP;
        if (primitive_type == PrimitiveType::TRIANGLE_FAN)
            return GL_TRIANGLE_FAN;
        return GL_NONE;
    }

    /**
     * PrimitiveBase : base object for writing GPU primitives into memory (GPU mapped memory for the usage) 
     */

    template<typename VERTEX_TYPE, PrimitiveType PRIMITIVE_TYPE>
    class PrimitiveBase
    {
    public:

        using vertex_type = VERTEX_TYPE;

        /** base constructor */
        inline PrimitiveBase() = default;
        /** copy constructor */
        inline PrimitiveBase(PrimitiveBase const & src) = default;
        /** constructor */
        inline PrimitiveBase(char* in_buffer, size_t in_vertex_size) :
            buffer(in_buffer), 
            vertex_size(in_vertex_size) 
        {
            assert(in_buffer != nullptr);
            assert(in_vertex_size > 0);
        }

        /** cast operator to child vertex type */
        template<typename OTHER_VERTEX_TYPE> 
        operator PrimitiveBase<OTHER_VERTEX_TYPE, PRIMITIVE_TYPE>& () 
        {
            static_assert(std::is_base_of_v<OTHER_VERTEX_TYPE, VERTEX_TYPE>);
            return *(PrimitiveBase<OTHER_VERTEX_TYPE, PRIMITIVE_TYPE>*)this;
        }
        /** cast operator to child vertex type */
        template<typename OTHER_VERTEX_TYPE>
        operator PrimitiveBase<OTHER_VERTEX_TYPE, PRIMITIVE_TYPE> const & () const
        {
            static_assert(std::is_base_of_v<OTHER_VERTEX_TYPE, VERTEX_TYPE>);
            return *(PrimitiveBase<OTHER_VERTEX_TYPE, PRIMITIVE_TYPE>*)this;
        }

        /** accessor */
        inline vertex_type & operator [](size_t index)
        {
            assert(index < chaos::GetVerticesPerParticle(PRIMITIVE_TYPE));
            return *((vertex_type*)(buffer + vertex_size * index));
        }
        /** const accessor */
        inline vertex_type const & operator [](size_t index) const
        {
            assert(index < chaos::GetVerticesPerParticle(PRIMITIVE_TYPE));
            return *((vertex_type const*)(buffer + vertex_size * index));
        }

        /** gets the buffer for this primitive */
        char* GetBuffer() const { return buffer; }
        /** gets the size of one vertice of this primitive */
        size_t GetVertexSize() const { return vertex_size; }
        /** gets the number of vertices for this primitive */
        size_t GetVerticesCount() const { return count; }

        /** gets forward iterator on vertices */
        RawDataBufferIterator<VERTEX_TYPE> begin() 
        {
            return RawDataBufferAccessor<VERTEX_TYPE>(buffer, count, vertex_size).begin();
        }
        /** gets end forward iterator on vertices */
        RawDataBufferIterator<VERTEX_TYPE> end()
        {
            return RawDataBufferAccessor<VERTEX_TYPE>(buffer, count, vertex_size).end();
        }
        /** gets forward const iterator on vertices */
        RawDataBufferIterator<VERTEX_TYPE const> begin() const
        {
            return RawDataBufferConstAccessor<VERTEX_TYPE>(buffer, count, vertex_size).begin();
        }
        /** gets end forward const iterator on vertices */
        RawDataBufferIterator<VERTEX_TYPE const> end() const
        {
            return RawDataBufferConstAccessor<VERTEX_TYPE>(buffer, count, vertex_size).end();
        }

	public:

		/** number of vertices for the primitives */
		static size_t constexpr count = GetRealVerticesPerParticle(PRIMITIVE_TYPE);

    protected:

        /** the buffer where we write buffer */
        char* buffer = nullptr;
        /** the size of a vertex for this primitive */
        size_t vertex_size = 0;
    };

    /**
      * The usual existing primitives
      */

    // fixed length primitives
    template<typename VERTEX_TYPE> using TrianglePrimitive = PrimitiveBase<VERTEX_TYPE, PrimitiveType::TRIANGLE>;
    template<typename VERTEX_TYPE> using TrianglePairPrimitive = PrimitiveBase<VERTEX_TYPE, PrimitiveType::TRIANGLE_PAIR>;
    template<typename VERTEX_TYPE> using QuadPrimitive = PrimitiveBase<VERTEX_TYPE, PrimitiveType::QUAD>;
    // non-fixed length vertices count
    template<typename VERTEX_TYPE> using TriangleStripPrimitive = PrimitiveBase<VERTEX_TYPE, PrimitiveType::TRIANGLE_STRIP>;
    template<typename VERTEX_TYPE> using TriangleFanPrimitive = PrimitiveBase<VERTEX_TYPE, PrimitiveType::TRIANGLE_FAN>;

    /**
     * PrimitiveOutputBase : a primitive generator (the base class)
     */

    class PrimitiveOutputBase
    {
    public:

        /** constructor */
        PrimitiveOutputBase(GPUDynamicMesh * in_dynamic_mesh, GPUBufferCache * in_buffer_cache, GPUVertexDeclaration * in_vertex_declaration, GPURenderer* in_renderer, size_t in_vertex_requirement_evaluation) :
            dynamic_mesh(in_dynamic_mesh),
            buffer_cache(in_buffer_cache),
            vertex_declaration(in_vertex_declaration),
            renderer(in_renderer),
            vertex_requirement_evaluation(in_vertex_requirement_evaluation)
        {
            assert(in_buffer_cache != nullptr);
            assert(in_dynamic_mesh != nullptr);
            assert(in_renderer != nullptr);
        }

        /** gets the size of one vertice of the generated primitive */
        size_t GetVertexSize() const { return vertex_size; }
        /** flush all pending Draws into the GPUDynamicMesh */
        void Flush();

    protected:

        /** register a new primitive */
        char* GeneratePrimitive(size_t required_size);
        /** allocate a buffer for the primitive and register a new primitive */
        char* ReserveBuffer(size_t required_size);

    protected:

        /** the dynamic mesh we are working on (to store primitives to render) */
        GPUDynamicMesh * dynamic_mesh = nullptr;
        /** a buffer cache */
        GPUBufferCache* buffer_cache = nullptr;
        /** the vertex declaration for all buffers */
        GPUVertexDeclaration * vertex_declaration = nullptr;
        /** the renderer used fence requests */
        GPURenderer* renderer = nullptr;
        /** the buffer where we are writting vertices */
        shared_ptr<GPUBuffer> vertex_buffer = nullptr;

        /** an evaluation of how many vertices could be used */
        size_t vertex_requirement_evaluation = 0;
       
        /** the current position for writing */
        char* buffer_position = nullptr;
        /** start of currently allocated buffer */
        char* buffer_start = nullptr;
        /** end of currently allocated buffer */
        char* buffer_end = nullptr;

        /** size of a vertex */
        size_t vertex_size = 0;
        /** the number of vertices per primitive (accessible for user from Primitive with [] operator) */
        size_t vertices_per_primitive = 0;
        /** the real number of vertices per primitives (in GPU memory quads are transformed into a triangle pair) */
        size_t real_vertices_per_primitive = 0;
        /** the primitive type */
        PrimitiveType type;
        /** the GL primitive type */
        GLenum primitive_gl_type = GL_NONE;
    };

    /**
     * TypedPrimitiveOutputBase : generic primitive generator
     */

    template<typename VERTEX_TYPE, PrimitiveType PRIMITIVE_TYPE> // PRIMITIVE_VERTICES_COUNT : should be 0 for STRIPS & FANS
    class TypedPrimitiveOutputBase : public PrimitiveOutputBase
    {
    public:

        using vertex_type = VERTEX_TYPE;

        using primitive_type = PrimitiveBase<vertex_type, PRIMITIVE_TYPE>;

        /** constructor */
        TypedPrimitiveOutputBase(GPUDynamicMesh* in_dynamic_mesh, GPUBufferCache* in_buffer_cache, GPUVertexDeclaration * in_vertex_declaration, GPURenderer* in_renderer, size_t in_vertex_requirement_evaluation) :
            PrimitiveOutputBase(in_dynamic_mesh, in_buffer_cache, in_vertex_declaration, in_renderer, in_vertex_requirement_evaluation)
        {
            vertex_size = sizeof(vertex_type);
            vertices_per_primitive = chaos::GetVerticesPerParticle(PRIMITIVE_TYPE);
            real_vertices_per_primitive = chaos::GetRealVerticesPerParticle(PRIMITIVE_TYPE);
            type = PRIMITIVE_TYPE;
            primitive_gl_type = chaos::GetGLPrimitiveType(PRIMITIVE_TYPE);
        }

        /** cast operator to child vertex type */
        template<typename OTHER_VERTEX_TYPE>
        operator TypedPrimitiveOutputBase<OTHER_VERTEX_TYPE, PRIMITIVE_TYPE>& ()
        {
            static_assert(std::is_base_of_v<OTHER_VERTEX_TYPE, VERTEX_TYPE>);
            return *(TypedPrimitiveOutputBase<OTHER_VERTEX_TYPE, PRIMITIVE_TYPE>*)this;
        }
        /** cast operator to child vertex type */
        template<typename OTHER_VERTEX_TYPE>
        operator TypedPrimitiveOutputBase<OTHER_VERTEX_TYPE, PRIMITIVE_TYPE> const& () const
        {
            static_assert(std::is_base_of_v<OTHER_VERTEX_TYPE, VERTEX_TYPE>);
            return *(TypedPrimitiveOutputBase<OTHER_VERTEX_TYPE, PRIMITIVE_TYPE>*)this;
        }

        /** add a primitive */
        inline primitive_type AddPrimitive(size_t custom_vertices_count = 0)
        {
            assert((vertices_per_primitive == 0) ^ (custom_vertices_count == 0)); // STRIPS & FANS require a CUSTOM number of vertices, other requires a NON CUSTOM number of vertices
            // implementation for STRIPS or FANS
            if constexpr (chaos::GetVerticesPerParticle(PRIMITIVE_TYPE) == 0)
            {
                // TODO : implement fans and strips 
                primitive_type result;                
                assert(0);
                return result;
            }
            // implementation for fixed length primitives
            else
            {
                return primitive_type(GeneratePrimitive(vertex_size * real_vertices_per_primitive), vertex_size);
            }
        }
    };

    /**
      * The usual existing primitives output's
      */

    // fixed length primitive
    template<typename VERTEX_TYPE> using TriangleOutput = TypedPrimitiveOutputBase<VERTEX_TYPE, PrimitiveType::TRIANGLE>;
    template<typename VERTEX_TYPE> using TrianglePairOutput = TypedPrimitiveOutputBase<VERTEX_TYPE, PrimitiveType::TRIANGLE_PAIR>;
    template<typename VERTEX_TYPE> using QuadOutput = TypedPrimitiveOutputBase<VERTEX_TYPE, PrimitiveType::QUAD>;
    // non-fixed length vertices count
    template<typename VERTEX_TYPE> using TriangleStripOutput = TypedPrimitiveOutputBase<VERTEX_TYPE, PrimitiveType::TRIANGLE_STRIP>;
    template<typename VERTEX_TYPE> using TriangleFanOutput = TypedPrimitiveOutputBase<VERTEX_TYPE, PrimitiveType::TRIANGLE_FAN>;
    
}; // namespace chaos

