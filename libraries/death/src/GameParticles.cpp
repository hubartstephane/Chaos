#include <death/GameParticles.h>

namespace death
{
    void ParticleBackgroundLayerTrait::ParticleToPrimitives(ParticleBackground const& particle, chaos::QuadOutput<chaos::VertexDefault> & output) const
    {
        chaos::QuadPrimitive<chaos::VertexDefault> primitive = output.AddPrimitive();

        primitive[0].position = glm::vec2(-1.0f, -1.0f);
        primitive[1].position = glm::vec2(+1.0f, -1.0f);
        primitive[2].position = glm::vec2(+1.0f, +1.0f);
        primitive[3].position = glm::vec2(-1.0f, +1.0f);

        for (chaos::VertexDefault & vertex : primitive)
        {
            glm::vec2 texcoord = vertex.position * 0.5f + glm::vec2(0.5f, 0.5f);

            vertex.texcoord.x = texcoord.x;
            vertex.texcoord.y = texcoord.y;
            vertex.texcoord.z = 0.0f;
            vertex.color = particle.color;
        }
    }

    void ParticleBackgroundLayerTrait::ParticleToPrimitives(ParticleBackground const& particle, chaos::TrianglePairOutput<chaos::VertexDefault>& output) const
    {
        chaos::TrianglePairPrimitive<chaos::VertexDefault> primitive = output.AddPrimitive();

        primitive[0].position = glm::vec2(-1.0f, -1.0f);
        primitive[1].position = glm::vec2(+1.0f, -1.0f);
        primitive[2].position = glm::vec2(-1.0f, +1.0f);

        primitive[3].position = glm::vec2(-1.0f, +1.0f);
        primitive[4].position = glm::vec2(+1.0f, -1.0f);
        primitive[5].position = glm::vec2(+1.0f, +1.0f);

        for (chaos::VertexDefault& vertex : primitive)
        {
            glm::vec2 texcoord = vertex.position * 0.5f + glm::vec2(0.5f, 0.5f);

            vertex.texcoord.x = texcoord.x;
            vertex.texcoord.y = texcoord.y;
            vertex.texcoord.z = 0.0f;
            vertex.color = particle.color;
        }
    }
	
}; // namespace death
