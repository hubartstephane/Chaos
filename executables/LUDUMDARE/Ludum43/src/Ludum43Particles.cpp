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
	glm::vec2 & player_position   = update_data.player_particle.bounding_box.position;
	glm::vec2 & particle_position = particle->bounding_box.position;
	glm::vec2 & particle_velocity = particle->velocity;

	float player_attraction_maxradius = update_data.player_attraction_maxradius;
	float player_attraction_minradius = update_data.player_attraction_minradius;
	float player_attraction_force     = update_data.player_attraction_force;
	float particle_max_velocity       = update_data.particle_max_velocity;


	glm::vec2 delta_pos = (player_position - particle_position);

	float l2 = glm::length2(delta_pos);
	float l  = 0.0f;
	// particle in direction of player
	if (l2 < player_attraction_maxradius * player_attraction_maxradius)
	{
		// particle never goes below lin radius
		if (l2 < player_attraction_minradius * player_attraction_minradius)
		{		
			l  = player_attraction_minradius;
			l2 = l * l;
			particle_position = player_position - glm::normalize(delta_pos) * l;
		}
		else		
		{
			l = chaos::MathTools::Sqrt(l2);
		}

		particle_velocity = particle_velocity + player_attraction_force * delta_pos; // * (update_data.player_attraction_maxradius - l) / update_data.player_attraction_maxradius;
	}
	// particle slowing down
	else
	{	
		particle->velocity *= powf(update_data.particle_slowing_factor, delta_time);	
	}


	// update particle velocity
	float velocity_length2 = glm::length2(particle_velocity);
	if (velocity_length2 > particle_max_velocity * particle_max_velocity)
		particle_velocity = glm::normalize(particle_velocity) * particle_max_velocity;				
	// update particle position
	particle_position += particle->velocity * (float)delta_time;

	return false;
}

ParticleAtomTrait::UpdateAtomData ParticleAtomTrait::BeginUpdateParticles(float delta_time, ParticleAtom * particles, size_t count, chaos::ParticleAllocation * allocation) const
{
	ParticleAtomTrait::UpdateAtomData result;
	result.player_particle = *((ParticlePlayer const*)game->GetPlayerParticle());

	result.player_attraction_minradius = game->player_attraction_minradius;
	result.player_attraction_maxradius = game->player_attraction_maxradius;
	result.particle_slowing_factor     = game->particle_slowing_factor;
	result.player_attraction_force     = game->player_attraction_force;
	result.enemy_attraction_radius = game->enemy_attraction_radius;
	result.particle_max_velocity = game->particle_max_velocity;

	return result;
}



