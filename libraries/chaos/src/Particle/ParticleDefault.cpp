#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void GenerateVertexPositionAttributes(box2 const& bounding_box, float rotation, glm::vec2* vertex_positions) // in order BL, BR, TR, TL
	{
		box_corners2 corners = GetBoxCorners(bounding_box);

		glm::vec2 const & bottomleft = corners.min;
		glm::vec2 const & topright   = corners.max;

		// compute the vertices
		vertex_positions[0] = glm::vec2(bottomleft.x, bottomleft.y);
		vertex_positions[1] = glm::vec2(topright.x, bottomleft.y);
		vertex_positions[2] = glm::vec2(topright.x, topright.y);
		vertex_positions[3] = glm::vec2(bottomleft.x, topright.y);

		// apply the rotation
		if (rotation != 0.0f)
		{
			glm::vec2 center_position = (bottomleft + topright) * 0.5f;

			float c = std::cos(rotation);
			float s = std::sin(rotation);
			for (int i = 0; i < 4; ++i)
				vertex_positions[i] = GLMTools::Rotate(vertex_positions[i] - center_position, c, s) + center_position;
		}
	}

	void GenerateVertexTextureAttributes(ParticleTexcoords const& texcoords, int flags, glm::vec3* vertex_texcoords) // in order BL, BR, TR, TL
	{
		int bitmap_index = texcoords.bitmap_index;

		// compute the vertices
		vertex_texcoords[0] = glm::vec3(texcoords.bottomleft.x, texcoords.bottomleft.y, bitmap_index);
		vertex_texcoords[1] = glm::vec3(texcoords.topright.x, texcoords.bottomleft.y, bitmap_index);
		vertex_texcoords[2] = glm::vec3(texcoords.topright.x, texcoords.topright.y, bitmap_index);
		vertex_texcoords[3] = glm::vec3(texcoords.bottomleft.x, texcoords.topright.y, bitmap_index);

		// apply texture symetries
		if ((flags & ParticleFlags::TEXTURE_DIAGONAL_FLIP) != 0)
		{
			std::swap(vertex_texcoords[0], vertex_texcoords[2]);
		}
		if ((flags & ParticleFlags::TEXTURE_HORIZONTAL_FLIP) != 0)
		{
			std::swap(vertex_texcoords[0], vertex_texcoords[1]);
			std::swap(vertex_texcoords[2], vertex_texcoords[3]);
		}
		if ((flags & ParticleFlags::TEXTURE_VERTICAL_FLIP) != 0)
		{
			std::swap(vertex_texcoords[0], vertex_texcoords[3]);
			std::swap(vertex_texcoords[1], vertex_texcoords[2]);
		}
	}

	void GenerateVertexFlagAttributes(int flags, int* vertex_flags) // in order BL, BR, TR, TL
	{
		// just keep the EIGHT_BITS_MODE flag (VertexFlags::EIGHT_BITS_MODE == ParticleFlags::EIGHT_BITS_MODE)
		int output_flags = (flags & ParticleFlags::EIGHT_BITS_MODE);

		vertex_flags[0] = VertexFlags::BOTTOM_LEFT | output_flags;
		vertex_flags[1] = VertexFlags::BOTTOM_RIGHT | output_flags;
		vertex_flags[2] = VertexFlags::TOP_RIGHT | output_flags;
		vertex_flags[3] = VertexFlags::TOP_LEFT | output_flags;
	}

	void GetTypedVertexDeclaration(GPUVertexDeclaration* result, boost::mpl::identity<VertexDefault>)
	{
		result->Push(VertexAttributeSemantic::POSITION, 0, VertexAttributeType::FLOAT2, "position");
		result->Push(VertexAttributeSemantic::TEXCOORD, 0, VertexAttributeType::FLOAT3, "texcoord");
		result->Push(VertexAttributeSemantic::COLOR, 0, VertexAttributeType::FLOAT4, "color");
		result->Push(VertexAttributeSemantic::NONE, -1, VertexAttributeType::INT1, "flags");
	}

}; // namespace chaos

