#pragma once


#include "Ludum43Particles.h"
#include "Ludum43Game.h"

#include <chaos/CollisionFramework.h>
#include <chaos/ClassTools.h>



#if 0

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



// ===========================================================================
// Object particle system
// ===========================================================================

static size_t AtlasParticleToVertices(LudumGame * game, ParticleObjectAtlas const * p, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation, float rotation = 0.0f)
{
	// work on a copy
	ParticleObjectAtlas particle = *p;

	int image_id = 0;
	if (particle.delta_image < 0)
		image_id = 1 + (-particle.delta_image);	
	else
		image_id = particle.delta_image + (int)(game->GetGameClockTime() / particle.frequency);

	// tweak particle coords considering that incomming sprite is the whole atlas (get sub-image coordinates)
	particle.texcoords = chaos::ParticleTools::MakeParticleTexcoordsAtlas(particle.texcoords, particle.atlas_dimension, particle.skip_last, image_id);
	// generate particle corners and texcoords
	chaos::ParticleTools::GenerateBoxParticle(particle.bounding_box, particle.texcoords, vertices, rotation);
	// copy the color in all triangles vertex
	for (size_t i = 0; i < 6; ++i)
		vertices[i].color = particle.color;

	return vertices_per_particle;

}

// ===========================================================================
// Object particle system
// ===========================================================================

size_t ParticleObjectAtlasTrait::ParticleToVertices(ParticleObjectAtlas const * p, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const
{
	return AtlasParticleToVertices(game, p, vertices, vertices_per_particle, allocation, 0.0f);
}

bool ParticleObjectAtlasTrait::UpdateParticle(float delta_time, ParticleObjectAtlas * particle, chaos::ParticleAllocation * allocation) const
{
	return false;
}





#endif


// ===========================================================================
// ParticlePlayerTrait
// ===========================================================================

size_t ParticlePlayerTrait::ParticleToVertices(ParticlePlayer const * p, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const
{
	return chaos::ParticleDefault::ParticleTrait::ParticleToVertices(p, vertices, vertices_per_particle, allocation);
}

bool ParticlePlayerTrait::UpdateParticle(float delta_time, ParticlePlayer * particle, chaos::ParticleAllocation * allocation) const
{




	return false;
}
















#if 0

// ===========================================================================
// ParticleWaterTrait
// ===========================================================================

size_t ParticleWaterTrait::ParticleToVertices(ParticleWater const * p, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const
{
	// work on a copy
	ParticleWater particle = *p;

	// compute the image id
	int image_id = (int)((particle.initial_life - particle.current_life) / particle.initial_life * (float)atlas_dimension.x);

	// tweak particle coords considering that incomming sprite is the whole atlas (get sub-image coordinates)
	particle.texcoords = chaos::ParticleTools::MakeParticleTexcoordsAtlas(particle.texcoords, atlas_dimension, 0, image_id);
	// generate particle corners and texcoords
	chaos::ParticleTools::GenerateBoxParticle(particle.bounding_box, particle.texcoords, vertices);
	// copy the color in all triangles vertex
	for (size_t i = 0; i < 6; ++i)
		vertices[i].color = particle.color;

	return vertices_per_particle;
}

bool ParticleWaterTrait::UpdateParticle(float delta_time, ParticleWater * particle, chaos::ParticleAllocation * allocation) const
{
	particle->bounding_box.position += particle->velocity * delta_time;

	particle->current_life -= delta_time;
	if (particle->current_life <= 0.0f)
		return true;
	return false;
}


#endif








