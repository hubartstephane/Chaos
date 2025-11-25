namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

    class GPUPrimitiveBufferCacheEntry;

    class GPUPrimitiveOutputBase;

    template<typename VERTEX_TYPE>
    class GPUPrimitiveOutput;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

    /**
    * GPUPrimitiveBufferCacheEntry : the GPUPrimitiveOutput has its own cache because we do not want to waste too much space whenever a new big strip/fan is requested (thoses buffers have been mapped in memory)
    */

    class CHAOS_API GPUPrimitiveBufferCacheEntry
    {
    public:

        /** the buffer (no need to use shared_ptr<> while they are already in use */
        GPUBuffer* buffer = nullptr;
        /** the starting position of the buffer */
        char* buffer_start = nullptr;
        /** the writing position of the mapped buffer */
        char* buffer_position = nullptr;
        /** the end position of the mapped buffer */
        char* buffer_end = nullptr;
    };

    /**
     * GPUPrimitiveOutputBase : a primitive generator (the base class)
     */

    class CHAOS_API GPUPrimitiveOutputBase
    {
    public:

        /** default/minimum number of vertices allocation */
        static constexpr size_t MIN_VERTEX_ALLOCATION = 100;

        /** constructor */
        GPUPrimitiveOutputBase(
			GPUMesh* in_mesh,
			GPUVertexDeclaration* in_vertex_declaration,
			GPURenderMaterial* in_render_material,
			size_t in_vertex_requirement_evaluation = MIN_VERTEX_ALLOCATION);
        /** constructor */
        GPUPrimitiveOutputBase(
			GPUMesh* in_mesh,
			GPUVertexDeclaration* in_vertex_declaration,
			ObjectRequest in_render_material_request,
			size_t in_vertex_requirement_evaluation = MIN_VERTEX_ALLOCATION);
        /** destructor */
        ~GPUPrimitiveOutputBase();

        /** find the bitmap info */
        AtlasBitmapInfo const* FindBitmapInfo(ObjectRequest bitmap_request, ObjectRequest folder_request = "sprites") const;

        /** gets the size of one vertice of the generated primitive */
        size_t GetVertexSize() const { return vertex_size; }
        /** flush all pending Draws into the GPUMesh */
        void Flush();
        /** change the material for the next primitives */
        void SetRenderMaterial(GPURenderMaterial* in_render_material);
        /** change the material for the next primitives */
        void SetRenderMaterial(ObjectRequest render_material_request);
        /** generate some memory for a bunch of data for a given primitive type */
        char* GeneratePrimitive(size_t requested_size, PrimitiveType primitive_type);

    protected:

        /** get a buffer we already have used partially */
        GPUPrimitiveBufferCacheEntry* GetInternalCachedBuffer(size_t required_size);
        /** put a buffer partially used to be restore later */
        void GiveBufferToInternalCache(GPUBuffer* in_buffer, char* in_buffer_start, char* in_buffer_position, char* in_buffer_end);
        /** flush the currently been constructed mesh element */
        void FlushMeshElement();
        /** flush currently being constructed primitive */
        void FlushDrawPrimitive();
        /** get some memory */
        char* AllocateBufferMemory(size_t in_size);

    protected:

        /** the dynamic mesh we are working on (to store primitives to render) */
        GPUMesh* mesh = nullptr;
        /** the vertex declaration for all buffers */
        GPUVertexDeclaration* vertex_declaration = nullptr;
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
        std::vector<GPUPrimitiveBufferCacheEntry> internal_buffer_pool;
        /** the current type of primitive we are working on */
        PrimitiveType current_primitive_type = PrimitiveType::NONE;
        /** the pending primitives */
        std::vector<GPUDrawPrimitive> pending_primitives;
    };

    /**
     * GPUPrimitiveOutput : a primitive generator
     */

    template<typename VERTEX_TYPE>
    class GPUPrimitiveOutput : public GPUPrimitiveOutputBase
    {
    public:

        using vertex_type = VERTEX_TYPE;

        /** constructor */
        GPUPrimitiveOutput(GPUMesh* in_mesh, GPUVertexDeclaration* in_vertex_declaration, GPURenderMaterial* in_render_material, size_t in_vertex_requirement_evaluation = MIN_VERTEX_ALLOCATION) :
            GPUPrimitiveOutputBase(in_mesh, in_vertex_declaration, in_render_material, in_vertex_requirement_evaluation)
        {
            vertex_size = sizeof(vertex_type);
            if (vertex_declaration == nullptr)
            {
                vertex_declaration = new GPUVertexDeclaration;
                if (vertex_declaration != nullptr)
                    GetTypedVertexDeclaration(vertex_declaration, boost::mpl::identity<vertex_type>());
            }
        }
        /** constructor */
        GPUPrimitiveOutput(GPUMesh* in_mesh, GPUVertexDeclaration* in_vertex_declaration, ObjectRequest in_render_material_request, size_t in_vertex_requirement_evaluation = MIN_VERTEX_ALLOCATION) :
            GPUPrimitiveOutputBase(in_mesh, in_vertex_declaration, in_render_material_request, in_vertex_requirement_evaluation)
        {
            vertex_size = sizeof(vertex_type);
            if (vertex_declaration == nullptr)
            {
                vertex_declaration = new GPUVertexDeclaration;
                if (vertex_declaration != nullptr)
                    GetTypedVertexDeclaration(vertex_declaration, boost::mpl::identity<vertex_type>());
            }
        }

        /** cast operator to child vertex type */
        template<typename OTHER_VERTEX_TYPE>
        operator GPUPrimitiveOutput<OTHER_VERTEX_TYPE>& ()
        {
            static_assert(std::is_base_of_v<OTHER_VERTEX_TYPE, VERTEX_TYPE>);
            return *(GPUPrimitiveOutput<OTHER_VERTEX_TYPE>*)this;
        }
        /** cast operator to child vertex type */
        template<typename OTHER_VERTEX_TYPE>
        operator GPUPrimitiveOutput<OTHER_VERTEX_TYPE> const& () const
        {
            static_assert(std::is_base_of_v<OTHER_VERTEX_TYPE, VERTEX_TYPE>);
            return *(GPUPrimitiveOutput<OTHER_VERTEX_TYPE>*)this;
        }

        /** insert some points */
        PointPrimitive<vertex_type> AddPoints(size_t primitive_count = 1)
        {
            size_t vertex_count = primitive_count * 1;
            return { GeneratePrimitiveAndConstruct(vertex_size, vertex_count, PrimitiveType::POINT), vertex_size, vertex_count };
        }
        /** insert some quads */
        QuadPrimitive<vertex_type> AddQuads(size_t primitive_count = 1)
        {
            size_t vertex_count = primitive_count * 4;
            return { GeneratePrimitiveAndConstruct(vertex_size, vertex_count, PrimitiveType::QUAD), vertex_size, vertex_count };
        }
        /** insert some triangles */
        TrianglePrimitive<vertex_type> AddTriangles(size_t primitive_count = 1)
        {
            size_t vertex_count = primitive_count * 3;
            return { GeneratePrimitiveAndConstruct(vertex_size, vertex_count, PrimitiveType::TRIANGLE), vertex_size, vertex_count };
        }
        /** insert some triangles pairs */
        TrianglePairPrimitive<vertex_type> AddTrianglePairs(size_t primitive_count = 1)
        {
            size_t vertex_count = primitive_count * 6;
            return { GeneratePrimitiveAndConstruct(vertex_size, vertex_count, PrimitiveType::TRIANGLE_PAIR), vertex_size, vertex_count };
        }
        /** insert a triangle strip */
        TriangleStripPrimitive<vertex_type> AddTriangleStrip(size_t vertex_count)
        {
            assert(vertex_count >= 3);
            return { GeneratePrimitiveAndConstruct(vertex_size, vertex_count, PrimitiveType::TRIANGLE_STRIP), vertex_size, vertex_count };
        }
        /** insert a triangle fan */
        TriangleFanPrimitive<vertex_type> AddTriangleFan(size_t vertex_count)
        {
            assert(vertex_count >= 3);
            return { GeneratePrimitiveAndConstruct(vertex_size, vertex_count, PrimitiveType::TRIANGLE_FAN), vertex_size, vertex_count };
        }
        /** insert a line */
        LinePrimitive<vertex_type> AddLines(size_t primitive_count = 1)
        {
            size_t vertex_count = primitive_count * 2;
            return { GeneratePrimitiveAndConstruct(vertex_size, vertex_count, PrimitiveType::LINE), vertex_size, vertex_count };
        }
        /** insert a line strip */
        LineStripPrimitive<vertex_type> AddLineStrip(size_t vertex_count)
        {
            assert(vertex_count >= 2);
            return { GeneratePrimitiveAndConstruct(vertex_size, vertex_count, PrimitiveType::LINE_STRIP), vertex_size, vertex_count };
        }
        /** insert a line loop */
        LineLoopPrimitive<vertex_type> AddLineLoop(size_t vertex_count)
        {
            assert(vertex_count >= 2);
            return { GeneratePrimitiveAndConstruct(vertex_size, vertex_count, PrimitiveType::LINE_LOOP), vertex_size, vertex_count };
        }

    protected:

        /** generate a buffer and call constructor for each primitives */
        char* GeneratePrimitiveAndConstruct(size_t vertex_size, size_t vertex_count, PrimitiveType primitive_type)
        {
            assert(vertex_size * vertex_count > 0);
            vertex_type* buffer = (vertex_type*)GeneratePrimitive(vertex_size * vertex_count, primitive_type);
            for (size_t i = 0; i < vertex_count; ++i)
                new (&buffer[i]) vertex_type(); // placement new
            return (char*)buffer;
        }
    };


    /**
     * Standalone functions
     */

    template<typename VERTEX_TYPE>
    void DrawLine(GPUPrimitiveOutput<VERTEX_TYPE>& output, glm::vec2 const& p1, glm::vec2 const& p2, glm::vec4 const& color);

    template<typename VERTEX_TYPE>
    void DrawBox(GPUPrimitiveOutput<VERTEX_TYPE>& output, obox2 const& b, glm::vec4 const& color, bool fill);

    template<typename VERTEX_TYPE>
    void DrawSphere(GPUPrimitiveOutput<VERTEX_TYPE>& output, sphere2 const& s, glm::vec4 const& color, bool fill);

    /** text creation */
    template<typename VERTEX_TYPE>
    QuadPrimitive<VERTEX_TYPE> DrawText(GPUPrimitiveOutput<VERTEX_TYPE>& output, char const* in_text, ParticleTextGenerator::GeneratorParams const& params = {}, ParticleTextGenerator::CreateTextAllocationParams const& allocation_params = {}, ParticleCorners* out_bounding_box = nullptr);


#else // => defined CHAOS_TEMPLATE_IMPLEMENTATION

    /**
     * Standalone functions
     */

    template<typename VERTEX_TYPE>
    void DrawLine(GPUPrimitiveOutput<VERTEX_TYPE>& output, glm::vec2 const& p1, glm::vec2 const& p2, glm::vec4 const& color)
    {
        LinePrimitive<VERTEX_TYPE> line = output.AddLines(1);
        line[0].position = p1;
        line[0].texcoord.z = -1.0f;
        line[0].color = color;
        line[0].flags = 0;

        line[1].position = p2;
        line[1].texcoord.z = -1.0f;
        line[1].color = color;
        line[1].flags = 0;
    }

    template<typename VERTEX_TYPE>
    void DrawBox(GPUPrimitiveOutput<VERTEX_TYPE>& output, obox2 const& b, glm::vec4 const& color, bool fill)
    {
        glm::vec2 vertex_positions[4];
        GenerateVertexPositionAttributes(b, b.rotation, vertex_positions); // in order BL, BR, TR, TL

        if (!fill)
        {
            for (size_t i = 0; i < 4; ++i)
                DrawLine(output, vertex_positions[i], vertex_positions[(i + 1) % 4], color);
        }
        else
        {
            QuadPrimitive<VERTEX_TYPE> quad = output.AddQuads(1);
            for (size_t i = 0; i < 4; ++i)
            {
                quad[i].position = vertex_positions[i];
                quad[i].texcoord.z = -1.0f;
                quad[i].color = color;
                quad[i].flags = 0;
            }
        }
    }

    template<typename VERTEX_TYPE>
    void DrawSphere(GPUPrimitiveOutput<VERTEX_TYPE>& output, sphere2 const& s, glm::vec4 const& color, bool fill)
    {
        constexpr int SEGMENT_COUNT = 40;

        auto vertex_position = [=](size_t index)
        {
            float angle = float(index) * float(2.0 * M_PI) / float(SEGMENT_COUNT);
            return s.position + s.radius * glm::vec2(std::cos(angle), std::sin(angle));
        };

        if (!fill)
        {
            for (size_t i = 0; i < SEGMENT_COUNT; ++i)
                DrawLine(output, vertex_position(i), vertex_position((i + 1) % SEGMENT_COUNT), color);
        }
        else
        {
            TriangleFanPrimitive<VERTEX_TYPE> fan = output.AddTriangleFan(SEGMENT_COUNT + 2);

            fan[0].position = s.position;
            fan[0].texcoord.z = -1.0f;
            fan[0].color = color;
            fan[0].flags = 0;

            for (size_t i = 0; i < SEGMENT_COUNT + 1; ++i)
            {
                VERTEX_TYPE& v = fan[1 + i];
                v.position = vertex_position(i % SEGMENT_COUNT);
                v.texcoord.z = -1.0f;
                v.color = color;
                v.flags = 0;
            }
        }
    }


    /** text creation */
    template<typename VERTEX_TYPE>
    QuadPrimitive<VERTEX_TYPE> DrawText(GPUPrimitiveOutput<VERTEX_TYPE>& output, char const* in_text, ParticleTextGenerator::GeneratorParams const& params, ParticleTextGenerator::CreateTextAllocationParams const& allocation_params, ParticleCorners * out_bounding_box)
    {
        // get the application
        WindowApplication const* window_application = Application::GetInstance();
        if (window_application == nullptr)
            return {};
        // get the generator
        ParticleTextGenerator::Generator const* generator = window_application->GetTextGenerator();
        if (generator == nullptr)
            return {};
        // generate the data
        ParticleTextGenerator::GeneratorResult generator_result;
        if (!generator->Generate(in_text, generator_result, params))
            return {};
        if (out_bounding_box != nullptr)
            *out_bounding_box = generator_result.bounding_box;
        // create the primitives
        return TextToPrimitives(output, generator_result, allocation_params);
    }

#endif

}; // namespace chaos