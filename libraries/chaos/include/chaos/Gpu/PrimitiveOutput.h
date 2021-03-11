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
     * GPUPrimitiveBufferCacheEntry : the PrimitiveOutput has its own cache because we do not want to waste too much space whenever a new big strip/fan is requested (thoses buffers have been mapped in memory)
     */

    class GPUPrimitiveBufferCacheEntry
    {
    public:

        /** the buffer (no need to use shared_ptr<> while they are already in use */
        GPUBuffer * buffer = nullptr;
        /** get the size that remains inside the buffer */
        size_t remaining_size = 0;
        /** the starting position of the mapped buffer */
        char * buffer_start = nullptr;
    };


 
    /**
     * PrimitiveOutputBase : a primitive generator (the base class)
     */

    class PrimitiveOutputBase
    {
    public:

        PrimitiveOutputBase() = default;




        /** constructor */
        PrimitiveOutputBase(GPUDynamicMesh* in_dynamic_mesh, GPUBufferCache* in_buffer_cache, GPUVertexDeclaration* in_vertex_declaration, GPURenderMaterial* in_render_material, size_t in_vertex_requirement_evaluation);

        /** destructor */
        ~PrimitiveOutputBase();



        /** gets the size of one vertice of the generated primitive */
        size_t GetVertexSize() const { return vertex_size; }
        /** flush all pending Draws into the GPUDynamicMesh */
        void Flush();
        /** change the material for the next primitives */
        void SetRenderMaterial(GPURenderMaterial* in_render_material);







        char* GeneratePrimitive(size_t requested_size, PrimitiveType primitive_type);

    protected:

        /** get a buffer we already have used partially */
        GPUPrimitiveBufferCacheEntry * GetInternalCachedBuffer(size_t required_size);
        /** put a buffer partially used to be restore later */
        void GiveBufferToInternalCache(GPUBuffer* in_buffer, size_t in_remaining_size, char* in_buffer_start);






        /** flush the currently been constructed mesh element */
        void FlushMeshElement();

        void UnmapAllInternalBuffer();


        char* AllocateBufferMemory(size_t in_size);


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
        /** the material to use */
        GPURenderMaterial* render_material = nullptr;

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




        /** our internal cache for the buffer we have started to use */
        std::vector<GPUPrimitiveBufferCacheEntry> internal_buffer_cache;


        /** the current type of primitive we are working on */
        PrimitiveType current_primitive_type = PrimitiveType::NONE;
        /** the mesh element we are currently working on */
        GPUDynamicMeshElement current_mesh_element;


   
        /** the currently been filled primitive */
        GPUDrawPrimitive current_draw_primitive;
    };

    // ==========================================================================================

    template<typename VERTEX_TYPE>
    class PrimitiveOutputXXX : public PrimitiveOutputBase
    {
    public:


        TrianglePrimitive<VERTEX_TYPE> AddTriangle()
        {

            return AddPrimitive()
        }

        QuadPrimitive<VERTEX_TYPE> AddQuad()
        {


        }

        TrianglePairPrimitive<VERTEX_TYPE> AddTrianglePair()
        {
            return TrianglePairPrimitive<VERTEX_TYPE>(GeneratePrimitive(6 * sizeof(VERTEX_TYPE), GL_TRIANGLES), sizeof(VERTEX_TYPE));
        }

        TriangleStripPrimitive<VERTEX_TYPE> AddTriangleStrip(int count)
        {

        }

        TriangleFanPrimitive<VERTEX_TYPE> AddTriangleStrip(int count)
        {
            
        }




    };










        // ==========================================================================================









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
        PrimitiveOutput(GPUDynamicMesh* in_dynamic_mesh, GPUBufferCache* in_buffer_cache, GPUVertexDeclaration * in_vertex_declaration, GPURenderMaterial* in_render_material, size_t in_vertex_requirement_evaluation) :
            PrimitiveOutputBase(in_dynamic_mesh, in_buffer_cache, in_vertex_declaration, in_render_material, in_vertex_requirement_evaluation)
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
    


