#include <death/GameParticles.h>

namespace death
{

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
