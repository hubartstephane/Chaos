#include <chaos/Chaos.h>

namespace chaos
{
	void DrawLine(PrimitiveOutput<VertexDefault>& output, glm::vec2 const& p1, glm::vec2 const& p2, glm::vec4 const& color)
	{
		LinePrimitive<VertexDefault> line = output.AddLines(1);
		line[0].position = p1;
		line[0].texcoord.z = -1.0f;
		line[0].color = color;
		line[0].flags = 0;

		line[1].position = p2;
		line[1].texcoord.z = -1.0f;
		line[1].color = color;
		line[1].flags = 0;
	}

	void DrawBox(PrimitiveOutput<VertexDefault>& output, obox2 const& b, glm::vec4 const& color, bool fill)
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
			QuadPrimitive<VertexDefault> quad = output.AddQuads(1);
			for (size_t i = 0; i < 4; ++i)
			{
				quad[i].position = vertex_positions[i];
				quad[i].texcoord.z = -1.0f;
				quad[i].color = color;
				quad[i].flags = 0;
			}
		}
	}
	void DrawSphere(PrimitiveOutput<VertexDefault>& output, sphere2 const& s, glm::vec4 const& color, bool fill)
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
			TriangleFanPrimitive<VertexDefault> fan = output.AddTriangleFan(SEGMENT_COUNT + 2);

			fan[0].position = s.position;
			fan[0].texcoord.z = -1.0f;
			fan[0].color = color;
			fan[0].flags = 0;
			
			for (size_t i = 0; i < SEGMENT_COUNT + 1; ++i)
			{
				VertexDefault& v = fan[1 + i];
				v.position = vertex_position(i % SEGMENT_COUNT);
				v.texcoord.z = -1.0f;
				v.color = color;
				v.flags = 0;
			}
		}
	}

}; // namespace chaos