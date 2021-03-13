#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
    class PrimitiveOutputBase;

    template<typename VERTEX_TYPE>
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
        /** the starting position of the buffer */
        char* buffer_start = nullptr;
        /** the writing position of the mapped buffer */
        char* buffer_position = nullptr;
        /** the end position of the mapped buffer */
        char* buffer_end = nullptr;
    };

    /**
     * PrimitiveOutputBase : a primitive generator (the base class)
     */

    class PrimitiveOutputBase
    {
    public:

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
        /** generate some memory for a bunch of data for a given primitive type */
        char* GeneratePrimitive(size_t requested_size, PrimitiveType primitive_type);

    protected:

        /** get a buffer we already have used partially */
        GPUPrimitiveBufferCacheEntry * GetInternalCachedBuffer(size_t required_size);
        /** put a buffer partially used to be restore later */
        void GiveBufferToInternalCache(GPUBuffer* in_buffer, char* in_buffer_start, char * in_buffer_position, char* in_buffer_end);
        /** flush the currently been constructed mesh element */
        void FlushMeshElement();
        /** flush currently being constructed primitive */
        void FlushDrawPrimitive();
        /** get some memory */
        char* AllocateBufferMemory(size_t in_size);

    protected:

        /** the dynamic mesh we are working on (to store primitives to render) */
        GPUDynamicMesh * dynamic_mesh = nullptr;
        /** a buffer cache */
        GPUBufferCache* buffer_cache = nullptr;
        /** the vertex declaration for all buffers */
        GPUVertexDeclaration * vertex_declaration = nullptr;
        /** the material to use */
        GPURenderMaterial* render_material = nullptr;

        /** the shared index buffer for quad creation */
        GPUBuffer* quad_index_buffer = nullptr;
        /** the max number of quad that can be rendered in a single draw call */
        size_t max_quad_count = 0;

        /** the buffer where we are writting vertices */
        shared_ptr<GPUBuffer> vertex_buffer;
        /** an evaluation of how many vertices could be used */
        size_t vertex_requirement_evaluation = 0;

        /** the start position of the buffer */
        char* buffer_start = nullptr; 
        /** the position in buffer from which data has not been flushed yet */
        char* buffer_unflushed = nullptr;
        /** the position in buffer from which to write */
        char* buffer_position = nullptr;
        /** end of currently allocated buffer */
        char* buffer_end = nullptr;

        /** size of a vertex */
        size_t vertex_size = 0;
        /** our internal cache for the buffer we have started to use */
        std::vector<GPUPrimitiveBufferCacheEntry> internal_buffer_cache;
        /** the current type of primitive we are working on */
        PrimitiveType current_primitive_type = PrimitiveType::NONE;
        /** the pending primitives */
        std::vector<GPUDrawPrimitive> pending_primitives;
    };

    template<typename VERTEX_TYPE>
    class PrimitiveOutput : public PrimitiveOutputBase
    {
    public:

        using vertex_type = VERTEX_TYPE;

        /** constructor */
        PrimitiveOutput(GPUDynamicMesh* in_dynamic_mesh, GPUBufferCache* in_buffer_cache, GPUVertexDeclaration* in_vertex_declaration, GPURenderMaterial* in_render_material, size_t in_vertex_requirement_evaluation) :
            PrimitiveOutputBase(in_dynamic_mesh, in_buffer_cache, in_vertex_declaration, in_render_material, in_vertex_requirement_evaluation)
        {
            vertex_size = sizeof(vertex_type);
        }

        /** insert some quads */
        QuadPrimitive<vertex_type> AddQuads(size_t primitive_count = 1)
        {
            size_t vertex_count = primitive_count * 4;
            return { GeneratePrimitive(vertex_size * vertex_count, PrimitiveType::QUAD), vertex_size, vertex_count };
        }
        /** insert some triangles */
        TrianglePrimitive<vertex_type> AddTriangles(size_t primitive_count = 1)
        {
            size_t vertex_count = primitive_count * 3;
            return { GeneratePrimitive(vertex_size * vertex_count, PrimitiveType::TRIANGLE), vertex_size, vertex_count };
        }
        /** insert some triangles pairs*/
        TrianglePairPrimitive<vertex_type> AddTrianglePairs(size_t primitive_count = 1)
        {
            size_t vertex_count = primitive_count * 6;
            return { GeneratePrimitive(vertex_size * vertex_count, PrimitiveType::TRIANGLE_PAIR), vertex_size, vertex_count };
        }
        /** insert a triangle strip */
        TriangleStripPrimitive<vertex_type> AddTriangleStrip(size_t vertex_count)
        {
            assert(vertex_count >= 3);
            return { GeneratePrimitive(vertex_size * vertex_count, PrimitiveType::TRIANGLE_STRIP), vertex_size, vertex_count };
        }
        /** insert a triangle fan */
        TriangleFanPrimitive<vertex_type> AddTriangleFan(size_t vertex_count)
        {
            assert(vertex_count >= 3);
            return { GeneratePrimitive(vertex_size * vertex_count, PrimitiveType::TRIANGLE_FAN), vertex_size, vertex_count };
        }
    };

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
    


