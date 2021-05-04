#include <chaos/Chaos.h>

namespace chaos
{
    void ParticleToPrimitives(ParticleBackground const& particle, PrimitiveOutput<VertexDefault> & output)
    {
        QuadPrimitive<VertexDefault> quad = output.AddQuads();
        ParticleToPrimitive(particle, quad);
    }

    void ParticleToPrimitive(ParticleBackground const& particle, QuadPrimitive<VertexDefault> & primitive)
    {
        primitive[0].position = glm::vec2(-1.0f, -1.0f);
        primitive[1].position = glm::vec2(+1.0f, -1.0f);
        primitive[2].position = glm::vec2(+1.0f, +1.0f);
        primitive[3].position = glm::vec2(-1.0f, +1.0f);

        for (VertexDefault& vertex : primitive)
        {
            glm::vec2 texcoord = vertex.position * 0.5f + glm::vec2(0.5f, 0.5f);

            vertex.texcoord.x = texcoord.x;
            vertex.texcoord.y = texcoord.y;
            vertex.texcoord.z = 0.0f;
            vertex.color = particle.color;
        }
    }
    void ParticleToPrimitive(ParticleBackground const& particle, TrianglePairPrimitive<VertexDefault> & primitive)
    {
        primitive[0].position = glm::vec2(-1.0f, -1.0f);
        primitive[1].position = glm::vec2(+1.0f, -1.0f);
        primitive[2].position = glm::vec2(-1.0f, +1.0f);

        primitive[3].position = glm::vec2(-1.0f, +1.0f);
        primitive[4].position = glm::vec2(+1.0f, -1.0f);
        primitive[5].position = glm::vec2(+1.0f, +1.0f);

        for (VertexDefault& vertex : primitive)
        {
            glm::vec2 texcoord = vertex.position * 0.5f + glm::vec2(0.5f, 0.5f);

            vertex.texcoord.x = texcoord.x;
            vertex.texcoord.y = texcoord.y;
            vertex.texcoord.z = 0.0f;
            vertex.color = particle.color;
        }
    }

}; // namespace chaos
