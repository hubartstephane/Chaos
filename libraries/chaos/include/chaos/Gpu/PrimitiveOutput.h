#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
    class PrimitiveOutputBase;

    template<typename VERTEX_TYPE, PrimitiveType PRIMITIVE_TYPE>
    class PrimitiveOutput;

}; // namespace chaos

#else 

namespace chaos
{
 
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
     * PrimitiveOutput : generic primitive generator
     */

    template<typename VERTEX_TYPE, PrimitiveType PRIMITIVE_TYPE> 
    class PrimitiveOutput : public PrimitiveOutputBase
    {
    public:

        using vertex_type = VERTEX_TYPE;

        using primitive_type = PrimitiveBase<vertex_type, PRIMITIVE_TYPE>;

        /** constructor */
        PrimitiveOutput(GPUDynamicMesh* in_dynamic_mesh, GPUBufferCache* in_buffer_cache, GPUVertexDeclaration * in_vertex_declaration, GPURenderer* in_renderer, size_t in_vertex_requirement_evaluation) :
            PrimitiveOutputBase(in_dynamic_mesh, in_buffer_cache, in_vertex_declaration, in_renderer, in_vertex_requirement_evaluation)
        {
            vertex_size = sizeof(vertex_type);
            vertices_per_primitive = GetVerticesPerParticle(PRIMITIVE_TYPE);
            real_vertices_per_primitive = GetRealVerticesPerParticle(PRIMITIVE_TYPE);
            type = PRIMITIVE_TYPE;
            primitive_gl_type = GetGLPrimitiveType(PRIMITIVE_TYPE);
        }

        /** cast operator to child vertex type */
        template<typename OTHER_VERTEX_TYPE>
        operator PrimitiveOutput<OTHER_VERTEX_TYPE, PRIMITIVE_TYPE>& ()
        {
            static_assert(std::is_base_of_v<OTHER_VERTEX_TYPE, VERTEX_TYPE>);
            return *(PrimitiveOutput<OTHER_VERTEX_TYPE, PRIMITIVE_TYPE>*)this;
        }
        /** cast operator to child vertex type */
        template<typename OTHER_VERTEX_TYPE>
        operator PrimitiveOutput<OTHER_VERTEX_TYPE, PRIMITIVE_TYPE> const& () const
        {
            static_assert(std::is_base_of_v<OTHER_VERTEX_TYPE, VERTEX_TYPE>);
            return *(PrimitiveOutput<OTHER_VERTEX_TYPE, PRIMITIVE_TYPE>*)this;
        }

        /** add a primitive */
        inline primitive_type AddPrimitive(size_t custom_vertices_count = 0)
        {
            assert((vertices_per_primitive == 0) ^ (custom_vertices_count == 0)); // STRIPS & FANS require a CUSTOM number of vertices, other requires a NON CUSTOM number of vertices
            // implementation for STRIPS or FANS
            if constexpr (GetVerticesPerParticle(PRIMITIVE_TYPE) == 0)
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

    // fixed length primitive
    template<typename VERTEX_TYPE> using TriangleOutput = PrimitiveOutput<VERTEX_TYPE, PrimitiveType::TRIANGLE>;
    template<typename VERTEX_TYPE> using TrianglePairOutput = PrimitiveOutput<VERTEX_TYPE, PrimitiveType::TRIANGLE_PAIR>;
    template<typename VERTEX_TYPE> using QuadOutput = PrimitiveOutput<VERTEX_TYPE, PrimitiveType::QUAD>;
    // non-fixed length vertices count
    template<typename VERTEX_TYPE> using TriangleStripOutput = PrimitiveOutput<VERTEX_TYPE, PrimitiveType::TRIANGLE_STRIP>;
    template<typename VERTEX_TYPE> using TriangleFanOutput = PrimitiveOutput<VERTEX_TYPE, PrimitiveType::TRIANGLE_FAN>;

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
    


