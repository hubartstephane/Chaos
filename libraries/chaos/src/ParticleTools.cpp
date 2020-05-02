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

        box2 ParticleCornersToBox(ParticleCorners const& corners)
        {
            return box2(std::make_pair(corners.bottomleft, corners.topright));
        }

        ParticleCorners BoxToParticleCorners(box2 const& box)
        {
            std::pair<glm::vec2, glm::vec2> tmp = GetBoxCorners(box);

            ParticleCorners result;
            result.bottomleft = tmp.first;
            result.topright = tmp.second;
            return result;
        }

        ParticleCorners GetParticleCorners(glm::vec2 const& position, glm::vec2 const& size, Hotpoint hotpoint)
        {
            ParticleCorners result;
            result.bottomleft = ConvertHotpointToBottomLeft(position, size, hotpoint);
            result.topright = result.bottomleft + size;
            return result;
        }

        ParticleTexcoords GetParticleTexcoords(BitmapAtlas::BitmapLayout const& layout)
        {
            ParticleTexcoords result;
            result.bottomleft = layout.bottomleft_texcoord;
            result.topright = layout.topright_texcoord;
            result.bitmap_index = layout.bitmap_index;
            return result;
        }

        ParticleTexcoords MakeParticleTexcoordsAtlas(ParticleTexcoords texcoords, glm::ivec2 const& atlas_dimension, int skip_last, int image_id)
        {
            // tweak particle texcoords to have a sub image
            int image_count = (atlas_dimension.x * atlas_dimension.y) - skip_last;
            if (image_count > 0)
            {
                image_id = image_id % image_count;

                glm::vec2 atlas_coord = glm::vec2(
                    (float)(image_id % atlas_dimension.x),
                    (float)(image_id / atlas_dimension.x)
                );

                glm::vec2 atlas_size = (texcoords.topright - texcoords.bottomleft) / RecastVector<glm::vec2>(atlas_dimension);

                texcoords.bottomleft = texcoords.bottomleft + atlas_coord * atlas_size;
                texcoords.topright = texcoords.bottomleft + atlas_size;
            }
            return texcoords;
        }

        ParticleTexcoords MakeParticleTexcoordsAtlas(ParticleTexcoords texcoords, glm::ivec2 const& atlas_dimension, glm::ivec2 const& image_id)
        {
            // tweak particle texcoords to have a sub image
            int image_count = (atlas_dimension.x * atlas_dimension.y);
            if (image_count > 0)
            {
                glm::vec2 atlas_coord = glm::vec2(
                    (float)(image_id.x % atlas_dimension.x),
                    (float)(image_id.y % atlas_dimension.y)
                );

                glm::vec2 atlas_size = (texcoords.topright - texcoords.bottomleft) / RecastVector<glm::vec2>(atlas_dimension);

                texcoords.bottomleft = texcoords.bottomleft + atlas_coord * atlas_size;
                texcoords.topright = texcoords.bottomleft + atlas_size;
            }
            return texcoords;
        }

		void GenerateBoxParticleAttributes(glm::vec2* vertex_positions, glm::vec3* vertex_texcoords, ParticleCorners const& corners, ParticleTexcoords const& texcoords, int flags, float rotation) // in order BL, BR, TR, TL
		{
			float bitmap_index = (float)texcoords.bitmap_index;

			// compute the vertices
			vertex_positions[0] = glm::vec2(corners.bottomleft.x, corners.bottomleft.y);
			vertex_texcoords[0] = glm::vec3(texcoords.bottomleft.x, texcoords.bottomleft.y, bitmap_index);

			vertex_positions[1] = glm::vec2(corners.topright.x, corners.bottomleft.y);
			vertex_texcoords[1] = glm::vec3(texcoords.topright.x, texcoords.bottomleft.y, bitmap_index);

			vertex_positions[2] = glm::vec2(corners.topright.x, corners.topright.y);
			vertex_texcoords[2] = glm::vec3(texcoords.topright.x, texcoords.topright.y, bitmap_index);

			vertex_positions[3] = glm::vec2(corners.bottomleft.x, corners.topright.y);
			vertex_texcoords[3] = glm::vec3(texcoords.bottomleft.x, texcoords.topright.y, bitmap_index);

			// apply the rotation
			if (rotation != 0.0f)
			{
				glm::vec2 center_position = (corners.bottomleft + corners.topright) * 0.5f;

				float c = std::cos(rotation);
				float s = std::sin(rotation);
				for (int i = 0 ; i < 4 ; ++i)
					vertex_positions[i] = GLMTools::Rotate(vertex_positions[i] - center_position, c, s) + center_position;
			}

			// apply texture symetries
			if ((flags & ParticleDefaultFlags::TEXTURE_HORIZONTAL_FLIP) != 0)
			{
				std::swap(vertex_texcoords[0].x, vertex_texcoords[1].x);
				std::swap(vertex_texcoords[2].x, vertex_texcoords[3].x);
			}
			if ((flags & ParticleDefaultFlags::TEXTURE_VERTICAL_FLIP) != 0)
			{
				std::swap(vertex_texcoords[0].y, vertex_texcoords[3].y);
				std::swap(vertex_texcoords[1].y, vertex_texcoords[2].y);
			}
		}

        void GenerateBoxParticle(QuadPrimitive<VertexDefault>& primitive, ParticleCorners const& corners, ParticleTexcoords const& texcoords, int flags, float rotation)
        {
			// in order BL, BR, TR, TL
			glm::vec2 vertex_positions[4];
			glm::vec3 vertex_texcoords[4];
			GenerateBoxParticleAttributes(vertex_positions, vertex_texcoords, corners, texcoords, flags, rotation);

			VertexDefault& v0 = primitive[0];
			VertexDefault& v1 = primitive[1];
			VertexDefault& v2 = primitive[2];
			VertexDefault& v3 = primitive[3];
	
			v0.position = vertex_positions[0];
			v0.texcoord = vertex_texcoords[0];

			v1.position = vertex_positions[1];
			v1.texcoord = vertex_texcoords[1];

			v2.position = vertex_positions[2];
			v2.texcoord = vertex_texcoords[2];

			v3.position = vertex_positions[3];
			v3.texcoord = vertex_texcoords[3];
        }

        void GenerateBoxParticle(QuadPrimitive<VertexDefault>& primitive, box2 const& box, ParticleTexcoords const& texcoords, int flags, float rotation)
        {
            std::pair<glm::vec2, glm::vec2> corners = GetBoxCorners(box);

            ParticleCorners particle_corners;
            particle_corners.bottomleft = corners.first;
            particle_corners.topright = corners.second;
            GenerateBoxParticle(primitive, particle_corners, texcoords, flags, rotation);
        }

        void GenerateBoxParticle(TrianglePairPrimitive<VertexDefault>& primitive, ParticleCorners const& corners, ParticleTexcoords const& texcoords, int flags, float rotation)
        {
			// in order BL, BR, TR, TL
			glm::vec2 vertex_positions[4];
			glm::vec3 vertex_texcoords[4];
			GenerateBoxParticleAttributes(vertex_positions, vertex_texcoords, corners, texcoords, flags, rotation);

            VertexDefault& v0 = primitive[0];
            VertexDefault& v1 = primitive[1];
            VertexDefault& v2 = primitive[2];
            VertexDefault& v3 = primitive[3];
            VertexDefault& v4 = primitive[4];
            VertexDefault& v5 = primitive[5];

            v0.position = vertex_positions[0];
            v0.texcoord = vertex_texcoords[0];

            v1.position = vertex_positions[1];
            v1.texcoord = vertex_texcoords[1];

            v2.position = vertex_positions[2];
            v2.texcoord = vertex_texcoords[2];

            v3.position = vertex_positions[0];
            v3.texcoord = vertex_texcoords[0];

            v4.position = vertex_positions[2];
            v4.texcoord = vertex_texcoords[2];

            v5.position = vertex_positions[3];
            v5.texcoord = vertex_texcoords[3];
        }

        void GenerateBoxParticle(TrianglePairPrimitive<VertexDefault>& primitive, box2 const& box, ParticleTexcoords const& texcoords, int flags, float rotation)
        {
            std::pair<glm::vec2, glm::vec2> corners = GetBoxCorners(box);

            ParticleCorners particle_corners;
            particle_corners.bottomleft = corners.first;
            particle_corners.topright = corners.second;
            GenerateBoxParticle(primitive, particle_corners, texcoords, flags, rotation);
        }

    }; // namespace ParticleTools

}; // namespace chaos

