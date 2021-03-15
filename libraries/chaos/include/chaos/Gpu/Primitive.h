#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
    enum class PrimitiveType;

    template<typename VERTEX_TYPE>
    class PrimitiveBase;

    template<typename VERTEX_TYPE, PrimitiveType PRIMITIVE_TYPE>
    class TypedPrimitive;

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
		NONE,
        POINT,
		TRIANGLE,
		TRIANGLE_PAIR,
		QUAD,
		TRIANGLE_STRIP,
		TRIANGLE_FAN,
        LINE,
        LINE_STRIP,
        LINE_LOOP
	};

    /** returns the OpenGL primitive type corresponding to the primitive */
    constexpr GLenum GetGLPrimitiveType(PrimitiveType primitive_type)
    {
        if (primitive_type == PrimitiveType::POINT)
            return GL_POINTS;

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

        if (primitive_type == PrimitiveType::LINE)
            return GL_LINES;
        if (primitive_type == PrimitiveType::LINE_STRIP)
            return GL_LINE_STRIP;
        if (primitive_type == PrimitiveType::LINE_LOOP)
            return GL_LINE_LOOP;

        return GL_NONE;
    }

    /**
     * Primitive : base object for writing GPU primitives into memory (GPU mapped memory for the usage) 
     */

    template<typename VERTEX_TYPE>
    class Primitive
    {
    public:

        using vertex_type = VERTEX_TYPE;

        /** base constructor */
        inline Primitive() = default;
        /** copy constructor */
        inline Primitive(Primitive const & src) = default;
        /** constructor */
        inline Primitive(char* in_buffer, size_t in_vertex_size, size_t in_vertex_count) :
            buffer(in_buffer), 
            vertex_size(in_vertex_size),
            vertex_count(in_vertex_count)
        {
            assert(in_buffer != nullptr);
            assert(in_vertex_size > 0);
            assert(in_vertex_count > 0);
        }

        /** cast operator to child vertex type */
        template<typename OTHER_VERTEX_TYPE> 
        operator Primitive<OTHER_VERTEX_TYPE>& ()
        {
            static_assert(std::is_base_of_v<OTHER_VERTEX_TYPE, VERTEX_TYPE>);
            return *(Primitive<OTHER_VERTEX_TYPE>*)this;
        }
        /** cast operator to child vertex type */
        template<typename OTHER_VERTEX_TYPE>
        operator Primitive<OTHER_VERTEX_TYPE> const & () const
        {
            static_assert(std::is_base_of_v<OTHER_VERTEX_TYPE, VERTEX_TYPE>);
            return *(Primitive<OTHER_VERTEX_TYPE>*)this;
        }

        /** accessor */
        inline vertex_type & operator [](size_t index)
        {
            assert(index < vertex_count);
            return *((vertex_type*)(buffer + vertex_size * index));
        }
        /** const accessor */
        inline vertex_type const & operator [](size_t index) const
        {
            assert(index < vertex_count);
            return *((vertex_type const*)(buffer + vertex_size * index));
        }

        /** gets the buffer for this primitive */
        char* GetBuffer() const { return buffer; }
        /** gets the size of one vertice of this primitive */
        size_t GetVertexSize() const { return vertex_size; }
        /** gets the number of vertices for this primitive */
        size_t GetVerticesCount() const { return vertex_count; }

        /** gets forward iterator on vertices */
        RawDataBufferIterator<VERTEX_TYPE> begin() 
        {
            return RawDataBufferAccessor<VERTEX_TYPE>(buffer, vertex_count, vertex_size).begin();
        }
        /** gets end forward iterator on vertices */
        RawDataBufferIterator<VERTEX_TYPE> end()
        {
            return RawDataBufferAccessor<VERTEX_TYPE>(buffer, vertex_count, vertex_size).end();
        }
        /** gets forward const iterator on vertices */
        RawDataBufferIterator<VERTEX_TYPE const> begin() const
        {
            return RawDataBufferConstAccessor<VERTEX_TYPE>(buffer, vertex_count, vertex_size).begin();
        }
        /** gets end forward const iterator on vertices */
        RawDataBufferIterator<VERTEX_TYPE const> end() const
        {
            return RawDataBufferConstAccessor<VERTEX_TYPE>(buffer, vertex_count, vertex_size).end();
        }

	protected:

        /** the buffer where we write buffer */
        char* buffer = nullptr;
        /** the size of a vertex for this primitive */
        size_t vertex_size = 0;
        /** the number of vertices in this primitive */
        size_t vertex_count = 0;
    };

    // TypedPrimitive : this class helps making difference with functions that works with QUADs and thoses that works with TRIANGLE_PAIR ... (and so one).
    //                  despite of that, it has no real additionnal value uppon its class base 
    template<typename VERTEX_TYPE, PrimitiveType PRIMITIVE_TYPE>
    class TypedPrimitive : public Primitive<VERTEX_TYPE>
    {
    public:
        using Primitive::Primitive;

        /** cast operator to child vertex type */
        template<typename OTHER_VERTEX_TYPE>
        operator TypedPrimitive<OTHER_VERTEX_TYPE, PRIMITIVE_TYPE>& ()
        {
            static_assert(std::is_base_of_v<OTHER_VERTEX_TYPE, VERTEX_TYPE>);
            return *(TypedPrimitive<OTHER_VERTEX_TYPE, PRIMITIVE_TYPE>*)this;
        }
        /** cast operator to child vertex type */
        template<typename OTHER_VERTEX_TYPE>
        operator TypedPrimitive<OTHER_VERTEX_TYPE, PRIMITIVE_TYPE> const& () const
        {
            static_assert(std::is_base_of_v<OTHER_VERTEX_TYPE, VERTEX_TYPE>);
            return *(TypedPrimitive<OTHER_VERTEX_TYPE, PRIMITIVE_TYPE>*)this;
        }
    };


    template<typename VERTEX_TYPE> using PointPrimitive = TypedPrimitive<VERTEX_TYPE, PrimitiveType::POINT>;

    template<typename VERTEX_TYPE> using TrianglePrimitive = TypedPrimitive<VERTEX_TYPE, PrimitiveType::TRIANGLE>;
    template<typename VERTEX_TYPE> using TrianglePairPrimitive = TypedPrimitive<VERTEX_TYPE, PrimitiveType::TRIANGLE_PAIR>;
    template<typename VERTEX_TYPE> using QuadPrimitive = TypedPrimitive<VERTEX_TYPE, PrimitiveType::QUAD>;

    template<typename VERTEX_TYPE> using TriangleStripPrimitive = TypedPrimitive<VERTEX_TYPE, PrimitiveType::TRIANGLE_STRIP>;
    template<typename VERTEX_TYPE> using TriangleFanPrimitive = TypedPrimitive<VERTEX_TYPE, PrimitiveType::TRIANGLE_FAN>;

    template<typename VERTEX_TYPE> using LinePrimitive = TypedPrimitive<VERTEX_TYPE, PrimitiveType::LINE>;
    template<typename VERTEX_TYPE> using LineStripPrimitive = TypedPrimitive<VERTEX_TYPE, PrimitiveType::LINE_STRIP>;
    template<typename VERTEX_TYPE> using LineLoopPrimitive = TypedPrimitive<VERTEX_TYPE, PrimitiveType::LINE_LOOP>;


}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION



