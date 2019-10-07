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

	float part_ratio = chaos::MathTools::CastAndDiv<float>(particle->enemy_index, particle->enemy_particle_count);

	float base_offset = particle_initial_range * part_ratio;

	float t = (base_offset + particle_speed * particle->time);
	

	glm::vec2 spawner_position  = particle->spawner_surface.position;
	glm::vec2 spawner_half_size = particle->spawner_surface.half_size;

	glm::vec2 & particle_position = particle->bounding_box.position;
	glm::vec2 & particle_half_size  = particle->bounding_box.half_size ;



	float spawner_radius = chaos::GetInnerSphere(particle->spawner_surface).radius;

	float min_camera_x = camera_box.position.x - camera_box.half_size.x;


	static int type = 0;

	if (type == 0)
	{


		

		float R = spawner_radius - particle_half_size.x;
		particle_position = spawner_position;
		particle_position.x += R * cosf(t);
		particle_position.y += R * sin(t);		
		
		// destroy if out of screen for ever
		if (spawner_position.x + spawner_half_size.x + particle_half_size.x < min_camera_x)
			return true;


	}
	else if (type == 1)
	{
//		target_position.x = t + particle->spawner_surface.position.x;

	//	particle->bounding_box.position.y = part_ratio

	//	particle->velocity = glm::vec2(-2000.0f, 0.0f);
	//	if (particle->time == 0.0)
	//		particle->bounding_box.position = p;

	
	//	if (p.x + particle->bounding_box.half_size.x < min_camera_x)
	//		return true;
	}
	else if (type == 2)
	{


	}














	


	return false;
}

















bool EnemyType::Initialize(chaos::TiledMap::ObjectTypeDefinition const * def)
{
	bitmap_name = def->FindPropertyString("BITMAP_NAME", "");
	enemy_life  = def->FindPropertyFloat("ENEMY_LIFE", enemy_life);
	enemy_damage  = def->FindPropertyFloat("ENEMY_DAMAGE", enemy_damage);

	return true;
}


