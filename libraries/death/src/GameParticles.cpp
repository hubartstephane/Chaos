#include <death/GameParticles.h>

namespace death
{
    void ParticleBackgroundTrait::ParticleToVertices(ParticleBackground const* particle, chaos::VertexOutput<chaos::ParticleDefault::Vertex> & vertices) const
    {
        chaos::ParticleDefault::Vertex& bl = vertices[0];
        chaos::ParticleDefault::Vertex& br = vertices[1];
        chaos::ParticleDefault::Vertex& tr = vertices[2];
        chaos::ParticleDefault::Vertex& tl = vertices[3];

        bl.position = glm::vec2(-1.0f, -1.0f);
        br.position = glm::vec2(+1.0f, -1.0f);
        tr.position = glm::vec2(+1.0f, +1.0f);
        tl.position = glm::vec2(-1.0f, +1.0f);


      
        // copy the vertices in all triangles vertex
        for (size_t i = 0; i < 6; ++i)
        {
            glm::vec2 texcoord = vertices[i].position * 0.5f + glm::vec2(0.5, 0.5);

            vertices[i].texcoord.x = texcoord.x;
            vertices[i].texcoord.y = texcoord.y;
            vertices[i].texcoord.z = 0.0f;
            vertices[i].color = particle->color;
        }

    }

	size_t ParticleBackgroundTrait::ParticleToVertices(ParticleBackground const * particle, chaos::ParticleDefault::Vertex * vertices, size_t vertices_per_particle) const
	{
		vertices[0].position.x = -1.0;
		vertices[0].position.y = -1.0;

		vertices[1].position.x = 1.0;
		vertices[1].position.y = -1.0;

		vertices[2].position.x = -1.0;
		vertices[2].position.y = 1.0;

		vertices[3] = vertices[2];
		vertices[4] = vertices[1];

		vertices[5].position.x = 1.0;
		vertices[5].position.y = 1.0;

		// copy the vertices in all triangles vertex
		for (size_t i = 0; i < 6; ++i)
		{
			glm::vec2 texcoord = vertices[i].position * 0.5f + glm::vec2(0.5, 0.5);

			vertices[i].texcoord.x = texcoord.x;
			vertices[i].texcoord.y = texcoord.y;
			vertices[i].texcoord.z = 0.0f;
			vertices[i].color = particle->color;
		}

		return vertices_per_particle;
	}
	
}; // namespace death
