#include <chaos/ParticleTools.h>
#include <chaos/Hotpoint.h>
#include <chaos/MathTools.h>
#include <chaos/ParticleLayer.h>
#include <chaos/GPURenderMaterial.h>
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/GPUResourceManager.h>

namespace chaos
{
    namespace ParticleTools
    {
        ParticleDefault* GetParticle(ParticleAllocationBase* allocation, size_t index)
        {
            if (allocation == nullptr)
                return nullptr;
            if (index >= allocation->GetParticleCount())
                return nullptr;

            ParticleAccessor<ParticleDefault> particles = allocation->GetParticleAccessor();
            if (particles.GetCount() == 0)
                return nullptr;
            return &particles[index];
        }

        ParticleDefault const* GetParticle(ParticleAllocationBase const* allocation, size_t index)
        {
            if (allocation == nullptr)
                return nullptr;
            if (index >= allocation->GetParticleCount())
                return nullptr;

            ParticleConstAccessor<ParticleDefault> particles = allocation->GetParticleAccessor();
            if (particles.GetCount() == 0)
                return nullptr;
            return &particles[index];
        }

        glm::vec2 GetParticlePosition(ParticleAllocationBase const* allocation, size_t index)
        {
            return GetParticleBox(allocation, index).position;
        }

        box2 GetParticleBox(ParticleAllocationBase const* allocation, size_t index)
        {
            ParticleDefault const* particle = GetParticle(allocation, index);
            if (particle == nullptr)
                return box2();
            return particle->bounding_box;
        }

        bool SetParticlePosition(ParticleAllocationBase* allocation, size_t index, glm::vec2 const& position)
        {
            ParticleDefault* particle = GetParticle(allocation, index);
            if (particle == nullptr)
                return false;
            particle->bounding_box.position = position;
            return true;
        }

        bool SetParticleBox(ParticleAllocationBase* allocation, size_t index, box2 const& box)
        {
            ParticleDefault* particle = GetParticle(allocation, index);
            if (particle == nullptr)
                return false;
            particle->bounding_box = box;
            return true;
        }

		void GenerateVertexPositionAttributes(glm::vec2* vertex_positions, ParticleCorners const& corners, float rotation, int flags) // in order BL, BR, TR, TL
		{
			// compute the vertices
			vertex_positions[0] = glm::vec2(corners.bottomleft.x, corners.bottomleft.y);
			vertex_positions[1] = glm::vec2(corners.topright.x, corners.bottomleft.y);
			vertex_positions[2] = glm::vec2(corners.topright.x, corners.topright.y);
			vertex_positions[3] = glm::vec2(corners.bottomleft.x, corners.topright.y);

			// apply the rotation
			if (rotation != 0.0f)
			{
				glm::vec2 center_position = (corners.bottomleft + corners.topright) * 0.5f;

				float c = std::cos(rotation);
				float s = std::sin(rotation);
				for (int i = 0; i < 4; ++i)
					vertex_positions[i] = GLMTools::Rotate(vertex_positions[i] - center_position, c, s) + center_position;
			}
		}

		void GenerateVertexTextureAttributes(glm::vec3* vertex_texcoords, ParticleTexcoords const& texcoords, int flags) // in order BL, BR, TR, TL
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

		void GenerateVertexFlagAttributes(int* vertex_flags, int flags) // in order BL, BR, TR, TL
		{
			// just keep the HEIGHT_BITS_MODE flag (VertexFlags::HEIGHT_BITS_MODE == ParticleFlags::HEIGHT_BITS_MODE)
			int output_flags = (flags & ParticleFlags::HEIGHT_BITS_MODE);

			vertex_flags[0] = VertexFlags::BOTTOM_LEFT | output_flags;
			vertex_flags[1] = VertexFlags::BOTTOM_RIGHT | output_flags;
			vertex_flags[2] = VertexFlags::TOP_RIGHT | output_flags;
			vertex_flags[3] = VertexFlags::TOP_LEFT | output_flags;
		}

        void GenerateBoxParticle(QuadPrimitive<VertexDefault>& primitive, box2 const& box, ParticleTexcoords const& texcoords, float rotation, int flags)
        {
            std::pair<glm::vec2, glm::vec2> corners = GetBoxCorners(box);

            ParticleCorners particle_corners;
            particle_corners.bottomleft = corners.first;
            particle_corners.topright = corners.second;

			// in order BL, BR, TR, TL
			glm::vec2 vertex_positions[4];
			GenerateVertexPositionAttributes(vertex_positions, particle_corners, rotation, flags);

			glm::vec3 vertex_texcoords[4];
			GenerateVertexTextureAttributes(vertex_texcoords, texcoords, flags);

			int vertex_flags[4];
			GenerateVertexFlagAttributes(vertex_flags, flags);

			VertexDefault& v0 = primitive[0];
			VertexDefault& v1 = primitive[1];
			VertexDefault& v2 = primitive[2];
			VertexDefault& v3 = primitive[3];

			v0.position = vertex_positions[0];
			v0.texcoord = vertex_texcoords[0];
			v0.flags = vertex_flags[0];

			v1.position = vertex_positions[1];
			v1.texcoord = vertex_texcoords[1];
			v1.flags = vertex_flags[1];

			v2.position = vertex_positions[2];
			v2.texcoord = vertex_texcoords[2];
			v2.flags = vertex_flags[2];

			v3.position = vertex_positions[3];
			v3.texcoord = vertex_texcoords[3];
			v3.flags = vertex_flags[3];
        }

        void GenerateBoxParticle(TrianglePairPrimitive<VertexDefault>& primitive, box2 const& box, ParticleTexcoords const& texcoords, float rotation, int flags)
        {
            std::pair<glm::vec2, glm::vec2> corners = GetBoxCorners(box);

            ParticleCorners particle_corners;
            particle_corners.bottomleft = corners.first;
            particle_corners.topright = corners.second;

			// in order BL, BR, TR, TL
			glm::vec2 vertex_positions[4];
			GenerateVertexPositionAttributes(vertex_positions, particle_corners, rotation, flags);

			glm::vec3 vertex_texcoords[4];
			GenerateVertexTextureAttributes(vertex_texcoords, texcoords, flags);

			int vertex_flags[4];
			GenerateVertexFlagAttributes(vertex_flags, flags);

			VertexDefault& v0 = primitive[0];
			VertexDefault& v1 = primitive[1];
			VertexDefault& v2 = primitive[2];
			VertexDefault& v3 = primitive[3];
			VertexDefault& v4 = primitive[4];
			VertexDefault& v5 = primitive[5];

			v0.position = vertex_positions[0];
			v0.texcoord = vertex_texcoords[0];
			v0.flags = vertex_flags[0];

			v1.position = vertex_positions[1];
			v1.texcoord = vertex_texcoords[1];
			v1.flags = vertex_flags[1];

			v2.position = vertex_positions[2];
			v2.texcoord = vertex_texcoords[2];
			v2.flags = vertex_flags[2];

			v3.position = vertex_positions[0];
			v3.texcoord = vertex_texcoords[0];
			v3.flags = vertex_flags[0];

			v4.position = vertex_positions[2];
			v4.texcoord = vertex_texcoords[2];
			v4.flags = vertex_flags[2];

			v5.position = vertex_positions[3];
			v5.texcoord = vertex_texcoords[3];
			v5.flags = vertex_flags[3];
        }

    }; // namespace ParticleTools

}; // namespace chaos

