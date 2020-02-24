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
        ParticleDefault::Particle* GetParticle(ParticleAllocationBase* allocation, size_t index)
        {
            if (allocation == nullptr)
                return nullptr;
            if (index >= allocation->GetParticleCount())
                return nullptr;

            ParticleAccessor<ParticleDefault::Particle> particles = allocation->GetParticleAccessor();
            if (particles.GetCount() == 0)
                return nullptr;
            return &particles[index];
        }

        ParticleDefault::Particle const* GetParticle(ParticleAllocationBase const* allocation, size_t index)
        {
            if (allocation == nullptr)
                return nullptr;
            if (index >= allocation->GetParticleCount())
                return nullptr;

            ParticleConstAccessor<ParticleDefault::Particle> particles = allocation->GetParticleAccessor();
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
            ParticleDefault::Particle const* particle = GetParticle(allocation, index);
            if (particle == nullptr)
                return box2();
            return particle->bounding_box;
        }

        bool SetParticlePosition(ParticleAllocationBase* allocation, size_t index, glm::vec2 const& position)
        {
            ParticleDefault::Particle* particle = GetParticle(allocation, index);
            if (particle == nullptr)
                return false;
            particle->bounding_box.position = position;
            return true;
        }

        bool SetParticleBox(ParticleAllocationBase* allocation, size_t index, box2 const& box)
        {
            ParticleDefault::Particle* particle = GetParticle(allocation, index);
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
            std::pair<glm::vec2, glm::vec2> tmp = GetBoxExtremums(box);

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
            result.bitmap_index = (float)layout.bitmap_index;
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

        void GenerateBoxParticle(ParticleCorners const& corners, ParticleTexcoords const& texcoords, QuadPrimitive<ParticleDefault::Vertex>& primitive, float rotation)
        {
            ParticleDefault::Vertex & bl = primitive[0];
            bl.position.x = corners.bottomleft.x;
            bl.position.y = corners.bottomleft.y;
            bl.texcoord.x = texcoords.bottomleft.x;
            bl.texcoord.y = texcoords.bottomleft.y;
            bl.texcoord.z = texcoords.bitmap_index;

            ParticleDefault::Vertex& br = primitive[1];
            br.position.x = corners.topright.x;
            br.position.y = corners.bottomleft.y;
            br.texcoord.x = texcoords.topright.x;
            br.texcoord.y = texcoords.bottomleft.y;
            br.texcoord.z = texcoords.bitmap_index;

            ParticleDefault::Vertex & tr = primitive[2];
            tr.position.x = corners.topright.x;
            tr.position.y = corners.topright.y;
            tr.texcoord.x = texcoords.topright.x;
            tr.texcoord.y = texcoords.topright.y;
            tr.texcoord.z = texcoords.bitmap_index;

            ParticleDefault::Vertex & tl = primitive[3];
            tl.position.x = corners.bottomleft.x;
            tl.position.y = corners.topright.y;
            tl.texcoord.x = texcoords.bottomleft.x;
            tl.texcoord.y = texcoords.topright.y;
            tl.texcoord.z = texcoords.bitmap_index;

            if (rotation != 0.0f)
            {
                glm::vec2 center_position = (corners.bottomleft + corners.topright) * 0.5f;

                float c = std::cos(rotation);
                float s = std::sin(rotation);

                bl.position = GLMTools::Rotate(bl.position - center_position, c, s) + center_position;
                br.position = GLMTools::Rotate(br.position - center_position, c, s) + center_position;
                tr.position = GLMTools::Rotate(tr.position - center_position, c, s) + center_position;
                tl.position = GLMTools::Rotate(tl.position - center_position, c, s) + center_position;
            }
        }

        void GenerateBoxParticle(box2 const& box, ParticleTexcoords const& texcoords, QuadPrimitive<ParticleDefault::Vertex>& primitive, float rotation)
        {
            std::pair<glm::vec2, glm::vec2> corners = GetBoxExtremums(box);

            ParticleCorners particle_corners;
            particle_corners.bottomleft = corners.first;
            particle_corners.topright = corners.second;
            GenerateBoxParticle(particle_corners, texcoords, primitive, rotation);
        }

        void GenerateBoxParticle(ParticleCorners const& corners, ParticleTexcoords const& texcoords, TrianglePairPrimitive<ParticleDefault::Vertex>& primitive, float rotation)
        {
            glm::vec2 bl_position = glm::vec2(corners.bottomleft.x, corners.bottomleft.y);
            glm::vec3 bl_texcoord = glm::vec3(texcoords.bottomleft.x, texcoords.bottomleft.y, texcoords.bitmap_index);

            glm::vec2 tr_position = glm::vec2(corners.topright.x, corners.topright.y);
            glm::vec3 tr_texcoord = glm::vec3(texcoords.topright.x, texcoords.topright.y, texcoords.bitmap_index);

            glm::vec2 tl_position = glm::vec2(corners.bottomleft.x, corners.topright.y);
            glm::vec3 tl_texcoord = glm::vec3(texcoords.bottomleft.x, texcoords.topright.y, texcoords.bitmap_index);

            glm::vec2 br_position = glm::vec2(corners.topright.x, corners.bottomleft.y);
            glm::vec3 br_texcoord = glm::vec3(texcoords.topright.x, texcoords.bottomleft.y, texcoords.bitmap_index);

            if (rotation != 0.0f)
            {
                glm::vec2 center_position = (corners.bottomleft + corners.topright) * 0.5f;

                float c = std::cos(rotation);
                float s = std::sin(rotation);

                bl_position = GLMTools::Rotate(bl_position - center_position, c, s) + center_position;
                tr_position = GLMTools::Rotate(tr_position - center_position, c, s) + center_position;
                tl_position = GLMTools::Rotate(tl_position - center_position, c, s) + center_position;
                br_position = GLMTools::Rotate(br_position - center_position, c, s) + center_position;
            }

            ParticleDefault::Vertex& v0 = primitive[0];
            ParticleDefault::Vertex& v1 = primitive[1];
            ParticleDefault::Vertex& v2 = primitive[2];
            ParticleDefault::Vertex& v3 = primitive[3];
            ParticleDefault::Vertex& v4 = primitive[4];
            ParticleDefault::Vertex& v5 = primitive[5];

            v0.position = bl_position;
            v0.texcoord = bl_texcoord;

            v1.position = br_position;
            v1.texcoord = br_texcoord;

            v2.position = tr_position;
            v2.texcoord = tr_texcoord;

            v3.position = bl_position;
            v3.texcoord = bl_texcoord;

            v4.position = tr_position;
            v4.texcoord = tr_texcoord;

            v5.position = tl_position;
            v5.texcoord = tl_texcoord;
        }

        void GenerateBoxParticle(box2 const& box, ParticleTexcoords const& texcoords, TrianglePairPrimitive<ParticleDefault::Vertex>& primitive, float rotation)
        {
            std::pair<glm::vec2, glm::vec2> corners = GetBoxExtremums(box);

            ParticleCorners particle_corners;
            particle_corners.bottomleft = corners.first;
            particle_corners.topright = corners.second;
            GenerateBoxParticle(particle_corners, texcoords, primitive, rotation);
        }

    }; // namespace ParticleTools

}; // namespace chaos

