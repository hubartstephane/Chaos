#pragma once


#include "Ludum43Particles.h"
#include "Ludum43Game.h"

#include <chaos/CollisionFramework.h>
#include <chaos/ClassTools.h>




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

// ===========================================================================
// ParticleEnemyTrait
// ===========================================================================

size_t ParticleEnemyTrait::ParticleToVertices(ParticleEnemy const * p, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const
{
	return chaos::ParticleDefault::ParticleTrait::ParticleToVertices(p, vertices, vertices_per_particle, allocation);
}

ParticleEnemyTrait::UpdateEnemyData ParticleEnemyTrait::BeginUpdateParticles(float delta_time, ParticleEnemy * particles, size_t count, chaos::ParticleAllocation * allocation) const
{
	ParticleEnemyTrait::UpdateEnemyData result;

	return result;
}

bool ParticleEnemyTrait::UpdateParticle(float delta_time, ParticleEnemy * particle, chaos::ParticleAllocation * allocation, ParticleEnemyTrait::UpdateEnemyData ExtraData) const
{




	return false;
}


// ===========================================================================
// ParticleAtomTrait
// ===========================================================================

size_t ParticleAtomTrait::ParticleToVertices(ParticleAtom const * p, VertexBase * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const
{
	return chaos::ParticleDefault::ParticleTrait::ParticleToVertices(p, vertices, vertices_per_particle, allocation);
}

bool ParticleAtomTrait::UpdateParticle(float delta_time, ParticleAtom * particle, chaos::ParticleAllocation * allocation, ParticleAtomTrait::UpdateAtomData ExtraData) const
{




	return false;
}

ParticleAtomTrait::UpdateAtomData ParticleAtomTrait::BeginUpdateParticles(float delta_time, ParticleAtom * particles, size_t count, chaos::ParticleAllocation * allocation) const
{
	ParticleAtomTrait::UpdateAtomData result;

	return result;
}



