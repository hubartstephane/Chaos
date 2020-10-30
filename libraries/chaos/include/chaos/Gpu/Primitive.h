#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
    enum class PrimitiveType;

    template<typename VERTEX_TYPE, PrimitiveType PRIMITIVE_TYPE>
    class PrimitiveBase;

}; // namespace chaos

#else 

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
            assert(index < GetVerticesPerParticle(PRIMITIVE_TYPE));
            return *((vertex_type*)(buffer + vertex_size * index));
        }
        /** const accessor */
        inline vertex_type const & operator [](size_t index) const
        {
            assert(index < GetVerticesPerParticle(PRIMITIVE_TYPE));
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

	protected:

		/** number of vertices for the primitives */
		static size_t constexpr count = GetRealVerticesPerParticle(PRIMITIVE_TYPE);
        /** the buffer where we write buffer */
        char* buffer = nullptr;
        /** the size of a vertex for this primitive */
        size_t vertex_size = 0;
    };

    // fixed length primitives
    template<typename VERTEX_TYPE> using TrianglePrimitive = PrimitiveBase<VERTEX_TYPE, PrimitiveType::TRIANGLE>;
    template<typename VERTEX_TYPE> using TrianglePairPrimitive = PrimitiveBase<VERTEX_TYPE, PrimitiveType::TRIANGLE_PAIR>;
    template<typename VERTEX_TYPE> using QuadPrimitive = PrimitiveBase<VERTEX_TYPE, PrimitiveType::QUAD>;
    // non-fixed length vertices count
    template<typename VERTEX_TYPE> using TriangleStripPrimitive = PrimitiveBase<VERTEX_TYPE, PrimitiveType::TRIANGLE_STRIP>;
    template<typename VERTEX_TYPE> using TriangleFanPrimitive = PrimitiveBase<VERTEX_TYPE, PrimitiveType::TRIANGLE_FAN>;

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION



