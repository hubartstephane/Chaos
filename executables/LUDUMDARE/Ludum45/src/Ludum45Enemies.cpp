#include "Ludum45Enemies.h"

#include <chaos/TiledMap.h>

bool EnemyPattern::Initialize(chaos::TiledMap::ObjectTypeDefinition const * def)
{
	enemy_count = def->FindPropertyInt("ENEMY_COUNT", 0);
	particle_initial_range = def->FindPropertyFloat("PARTICLE_INITIAL_RANGE", particle_initial_range);
	particle_speed = def->FindPropertyFloat("PARTICLE_SPEED", 0);

	return true;
}

bool EnemyPattern::UpdateParticle(float delta_time, ParticleEnemy * particle, chaos::box2 const & player_box, chaos::box2 const & camera_box)
{
	float base_offset = particle_initial_range * chaos::MathTools::CastAndDiv<float>(particle->enemy_index, particle->enemy_particle_count);

	float t = (base_offset + particle_speed * particle->time);
	
	glm::vec2 p = particle->spawner_surface.position;

	// destroy if outof screen for ever
	if (p.x + particle->spawner_surface.half_size.x < camera_box.position.x - camera_box.half_size.x)
		return true;



	float R = glm::length(particle->spawner_surface.half_size);
	p.x += R * cosf(t);
	p.y += R * sin(t);



	particle->bounding_box.position = p;


	return false;
}

















bool EnemyType::Initialize(chaos::TiledMap::ObjectTypeDefinition const * def)
{
	bitmap_name = def->FindPropertyString("BITMAP_NAME", "");
	enemy_life  = def->FindPropertyFloat("ENEMY_LIFE", enemy_life);
	enemy_damage  = def->FindPropertyFloat("ENEMY_DAMAGE", enemy_damage);

	return true;
}

