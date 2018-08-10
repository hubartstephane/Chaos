#pragma once


#include "LudumTemplateParticles.h"
#include "LudumTemplateGame.h"

#include <chaos/CollisionFramework.h>
#include <chaos/ClassTools.h>


// ===========================================================================
// Background particle system
// ===========================================================================

size_t ParticleBackgroundTrait::ParticleToVertices(ParticleBackground const * particle, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const
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

// ===========================================================================
// Object particle system
// ===========================================================================

size_t ParticleObjectTrait::ParticleToVertices(ParticleObject const * particle, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const
{
	// generate particle corners and texcoords
	chaos::ParticleTools::GenerateBoxParticle(particle->bounding_box, particle->texcoords, vertices);
	// copy the color in all triangles vertex
	for (size_t i = 0; i < 6; ++i)
		vertices[i].color = particle->color;

	return vertices_per_particle;
}

bool ParticleObjectTrait::UpdateParticle(float delta_time, ParticleObject * particle, chaos::ParticleAllocation * allocation) const
{
	return false;
}