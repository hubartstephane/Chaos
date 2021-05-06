#ifdef CHAOS_FORWARD_DECLARATION




#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


namespace chaos
{

    /**
     * Standalone functions
     */

    template<typename VERTEX_TYPE>
    void DrawLine(PrimitiveOutput<VERTEX_TYPE>& output, glm::vec2 const& p1, glm::vec2 const& p2, glm::vec4 const& color)
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
    void DrawBox(PrimitiveOutput<VERTEX_TYPE>& output, obox2 const& b, glm::vec4 const& color, bool fill)
    {
        glm::vec2 vertex_positions[4];
        GenerateVertexPositionAttributes(b, b.rotator, vertex_positions); // in order BL, BR, TR, TL

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
    void DrawSphere(PrimitiveOutput<VERTEX_TYPE>& output, sphere2 const& s, glm::vec4 const& color, bool fill)
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
    QuadPrimitive<VERTEX_TYPE> DrawText(PrimitiveOutput<VERTEX_TYPE>& output, char const* in_text, ParticleTextGenerator::GeneratorParams const& params, ParticleTextGenerator::CreateTextAllocationParams const& allocation_params)
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
        // create the primitives
        return TextToPrimitives(output, generator_result, allocation_params);
    }


}; // namespace chaos

#else 

namespace chaos
{
    /** draw a line */
    template<typename VERTEX_TYPE>
    void DrawLine(PrimitiveOutput<VERTEX_TYPE>& output, glm::vec2 const& p1, glm::vec2 const& p2, glm::vec4 const& color);
    /** draw a box/obox */
    template<typename VERTEX_TYPE>
    void DrawBox(PrimitiveOutput<VERTEX_TYPE>& output, obox2 const& b, glm::vec4 const& color, bool fill);
    /** draw a sphere */
    template<typename VERTEX_TYPE>
    void DrawSphere(PrimitiveOutput<VERTEX_TYPE>& output, sphere2 const& s, glm::vec4 const& color, bool fill);
    /** text creation */
    template<typename VERTEX_TYPE>
    QuadPrimitive<VERTEX_TYPE> DrawText(PrimitiveOutput<VERTEX_TYPE>& output, char const* in_text, ParticleTextGenerator::GeneratorParams const& params, ParticleTextGenerator::CreateTextAllocationParams const& allocation_params);

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
    


