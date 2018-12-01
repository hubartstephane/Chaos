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
	result.player_particle = *((ParticlePlayer const*)game->GetPlayerParticle());

	return result;
}

bool ParticleEnemyTrait::UpdateParticle(float delta_time, ParticleEnemy * particle, chaos::ParticleAllocation * allocation, ParticleEnemyTrait::UpdateEnemyData update_data) const
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

bool ParticleAtomTrait::UpdateParticle(float delta_time, ParticleAtom * particle, chaos::ParticleAllocation * allocation, ParticleAtomTrait::UpdateAtomData update_data) const
{
	glm::vec2 delta_pos = (update_data.player_particle.bounding_box.position - particle->bounding_box.position);

	float l2 = glm::length2(delta_pos);
	if (l2 < update_data.player_attraction_maxradius * update_data.player_attraction_maxradius)
	{
		float l = chaos::MathTools::Sqrt(l2);

		glm::vec2 velocity = 
			particle->velocity +
			update_data.player_attraction_force * delta_pos; // * (update_data.player_attraction_maxradius - l) / update_data.player_attraction_maxradius;

		float velocity2 = glm::length2(velocity);
		if (velocity2 > update_data.particle_max_velocity * update_data.particle_max_velocity)
		{
			velocity = glm::normalize(velocity) * update_data.particle_max_velocity;				
		}
		particle->velocity = velocity;
	}
	else
	{
		static float slowing_factor = 0.5f;


		//float velocity2 = glm::length2(particle->velocity);
		
		particle->velocity *= powf(slowing_factor, delta_time); // * 20.0f;
	
	}


	particle->bounding_box.position += particle->velocity * (float)delta_time;

	return false;
}

ParticleAtomTrait::UpdateAtomData ParticleAtomTrait::BeginUpdateParticles(float delta_time, ParticleAtom * particles, size_t count, chaos::ParticleAllocation * allocation) const
{
	ParticleAtomTrait::UpdateAtomData result;
	result.player_particle = *((ParticlePlayer const*)game->GetPlayerParticle());

	result.player_attraction_minradius = game->player_attraction_minradius;
	result.player_attraction_maxradius = game->player_attraction_maxradius;
	result.player_attraction_force     = game->player_attraction_force;
	result.enemy_attraction_radius = game->enemy_attraction_radius;
	result.particle_max_velocity = game->particle_max_velocity;

	return result;
}



