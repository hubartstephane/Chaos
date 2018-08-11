#pragma once


#include "Ludum42Particles.h"
#include "Ludum42Game.h"

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








size_t ParticleObjectAtlasTrait::ParticleToVertices(ParticleObjectAtlas const * p, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const
{
	// work on a copy
	ParticleObjectAtlas particle = *p;

	int image_id = 0;
	if (particle.skip_last < 0)
	{
		image_id = 1 + (-particle.skip_last);	
	}
	else
	{
		image_id = (int)(game->GetMainClockTime() / particle.frequency);
	}

	// tweak particle texcoords to have a sub image
	int image_count = (particle.atlas_dimension.x * particle.atlas_dimension.y) - particle.skip_last;
	if (image_count > 0)
	{
		image_id = image_id % image_count;

		glm::vec2 atlas_coord = glm::vec2(
			(float)(image_id % particle.atlas_dimension.x),
			(float)(image_id / particle.atlas_dimension.y)
		);

		glm::vec2 atlas_size = (particle.texcoords.topright - particle.texcoords.bottomleft) / chaos::GLMTools::RecastVector<glm::vec2>(particle.atlas_dimension);

		particle.texcoords.bottomleft = particle.texcoords.bottomleft + atlas_coord * atlas_size;
		particle.texcoords.topright = particle.texcoords.bottomleft + atlas_size;
	}

	// generate particle corners and texcoords
	chaos::ParticleTools::GenerateBoxParticle(particle.bounding_box, particle.texcoords, vertices);
	// copy the color in all triangles vertex
	for (size_t i = 0; i < 6; ++i)
		vertices[i].color = particle.color;

	return vertices_per_particle;
}

bool ParticleObjectAtlasTrait::UpdateParticle(float delta_time, ParticleObjectAtlas * particle, chaos::ParticleAllocation * allocation) const
{
	return false;
}


