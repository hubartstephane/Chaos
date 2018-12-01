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

bool ParticleAtomTrait::ApplyAffectorToParticles(float delta_time, ParticleAtom * particle, ParticleAtomTrait::UpdateAtomData update_data, ParticleAffector const & affector) const
{
	glm::vec2 & particle_position = particle->bounding_box.position;
	glm::vec2 & particle_velocity = particle->velocity;
	
	glm::vec2 const & affector_position = affector.bounding_box.position;

	glm::vec2 delta_pos = (affector_position - particle_position);

	float l2 = glm::length2(delta_pos);

	float attraction_maxradius = affector.attraction_maxradius;
	float attraction_minradius = affector.attraction_minradius;
	float player_attraction_force     = affector.attraction_force;

	// particle in range
	if (l2 < attraction_maxradius * attraction_maxradius)
	{
		float l  = 0.0f;
		// particle never goes below lin radius
		if (l2 < attraction_minradius * attraction_minradius)
		{		
			l  = attraction_minradius;
			particle_position = affector_position - glm::normalize(delta_pos) * l;
		}
		else		
		{
			l = chaos::MathTools::Sqrt(l2);
		}

		float distance_ratio = 1.0f;
		distance_ratio = chaos::MathTools::Clamp(1.0f - (l - attraction_minradius) / (attraction_maxradius - attraction_minradius));

		// create a tangent force
		if (glm::length2(particle_velocity) > 0.0f)
		{
			glm::vec3 a = glm::normalize(glm::vec3(delta_pos.x, delta_pos.y, 0.0f));
			glm::vec3 b = glm::vec3(0.0f, 0.0f, 1.0f);
			glm::vec3 tangent = glm::cross(a, b);

			particle_velocity = particle_velocity + distance_ratio * affector.tangent_force * glm::vec2(tangent.x, tangent.y);
		}

		particle_velocity = particle_velocity + distance_ratio *  affector.attraction_force * delta_pos; // * (update_data.player_attraction_maxradius - l) / update_data.player_attraction_maxradius;

		return true;
	}
	return false;
}

bool ParticleAtomTrait::UpdateParticle(float delta_time, ParticleAtom * particle, chaos::ParticleAllocation * allocation, ParticleAtomTrait::UpdateAtomData update_data) const
{
	glm::vec2 & player_position   = update_data.player_particle.bounding_box.position;
	glm::vec2 & particle_position = particle->bounding_box.position;

	glm::vec2 delta_pos = (player_position - particle_position);

	float l2 = glm::length2(delta_pos);
	float l  = 0.0f;

	// particle too far ?
	if (l2 > update_data.world_clamp_radius * update_data.world_clamp_radius)
		return false;

	// apply all effectors
	bool affected = false;
	affected |= ApplyAffectorToParticles(delta_time, particle, update_data, update_data.player_particle);
	
	size_t enemy_count = update_data.enemy_particles.size();
	for (size_t i = 0 ; i < enemy_count ; ++i)
		affected |= ApplyAffectorToParticles(delta_time, particle, update_data, update_data.enemy_particles[i]);

	// final computation
	float particle_max_velocity = update_data.particle_max_velocity;

	glm::vec2 & particle_velocity = particle->velocity;

	// particle slowing down
	if (!affected)
		particle->velocity *= powf(update_data.particle_slowing_factor, delta_time);	

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

	result.particle_slowing_factor     = game->particle_slowing_factor;	
	result.particle_max_velocity = game->particle_max_velocity;
	result.world_clamp_radius    = game->world_clamp_radius;

	// capture all Enemies in range
	LudumLevelInstance const * level_instance = dynamic_cast<LudumLevelInstance const *>(game->GetCurrentLevelInstance());
	if (level_instance != nullptr)
	{
		death::TiledMap::LayerInstance const * layer_instance = level_instance->FindLayerInstance("Enemies");
		if (layer_instance)
		{
			chaos::ParticleLayer const * particle_layer = layer_instance->GetParticleLayer();
			if (particle_layer != nullptr)
			{
				size_t count = particle_layer->GetAllocationCount();
				for (size_t i = 0 ; i < count ; ++i)
				{
					chaos::ParticleAllocation const * allocation = particle_layer->GetAllocation(i);
					if (allocation == nullptr)
						continue;

					chaos::ParticleConstAccessor<ParticleEnemy> enemies = allocation->GetParticleAccessor<ParticleEnemy>();
					
					size_t enemies_count = enemies.GetCount();
					for (size_t j = 0 ; j < enemies_count ; ++j)
					{
						ParticleEnemy const & enemy = enemies[j];
					
						float l2 = glm::length2(enemy.bounding_box.position - result.player_particle.bounding_box.position);
						if (l2 > result.world_clamp_radius * result.world_clamp_radius)
							continue;
						result.enemy_particles.push_back(enemy);									
					}				
				}
			}		
		}	
	}

	return result;
}



