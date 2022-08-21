#include "chaos/Chaos.h"

#include "Ludum45Enemies.h"

bool EnemyPattern::Initialize(chaos::TiledMap::PropertyOwner const * def)
{
	enemy_count = def->GetPropertyValueInt("ENEMY_COUNT", 0);
	particle_initial_range = def->GetPropertyValueFloat("PARTICLE_INITIAL_RANGE", particle_initial_range);
	particle_speed = def->GetPropertyValueFloat("PARTICLE_SPEED", 0);
	pattern_index =  def->GetPropertyValueInt("PATTERN_INDEX", pattern_index);


	return true;
}

bool EnemyPattern::UpdateParticle(float delta_time, ParticleEnemy * particle, chaos::box2 const & camera_box)
{

	float particle_ratio = chaos::MathTools::CastAndDiv<float>(particle->enemy_index, particle->enemy_particle_count);

	float base_offset = particle_initial_range * particle_ratio;

	float t = (base_offset + particle_speed * particle->time);
	

	glm::vec2 spawner_position  = particle->spawner_box.position;
	glm::vec2 spawner_half_size = particle->spawner_box.half_size;

	glm::vec2 & particle_position = particle->bounding_box.position;
	glm::vec2 & particle_half_size  = particle->bounding_box.half_size ;



	float spawner_radius = chaos::GetInnerSphere(particle->spawner_box).radius;

	float min_camera_x = camera_box.position.x - camera_box.half_size.x;

	bool spawner_out = (spawner_position.x + spawner_half_size.x + particle_half_size.x < min_camera_x);
	bool particle_out = (particle_position.x + particle_half_size.x < min_camera_x);
		
	bool result = false;
	if (pattern_index == 1)
	{

		// CIRCLE
		
		float R = spawner_radius - particle_half_size.x;
		particle_position = spawner_position;
		particle_position.x += R * std::cos(t);
		particle_position.y += R * std::sin(t);

		result = spawner_out;
	}
	else if (pattern_index == 2)
	{

		// COS sur Y

		particle_position.x = spawner_position.x + spawner_half_size.x * (particle_ratio * 2.0f - 1.0f);
		particle_position.y = spawner_position.y + spawner_half_size.y * std::cos(t);

		result = particle_out;
	}
	else if (pattern_index == 3)
	{
		// LIGNE VERTICAL qui avance
		particle_position.y = spawner_position.y + spawner_half_size.y * (particle_ratio * 2.0f - 1.0f);
		particle_position.x = spawner_position.x - t;

		result = particle_out;

	}
	else if (pattern_index == 4)
	{
	
		// RANDOM + droit devant
		if (particle->time == 0.0f)
		{
			particle_position = spawner_position + (spawner_half_size - particle_half_size) * (2.0f * chaos::GLMTools::RandVec2() - glm::vec2(1.0f, 1.0f));			
			particle->velocity.x = -particle_speed;
		}

		result = particle_out;
	}
	else if (pattern_index == 5)
	{

		particle_position.x = spawner_position.x + spawner_half_size.x * (particle_ratio * 2.0f - 1.0f);
		particle_position.y = spawner_position.y + spawner_half_size.y * (particle_ratio * 2.0f - 1.0f);
		particle->velocity.x = -particle_speed;
	
		result = particle_out;
	}

	return result;
}

bool EnemyType::Initialize(chaos::TiledMap::ObjectTypeDefinition const * def)
{
	bitmap_name = def->GetPropertyValueString("BITMAP_NAME", "");
    enemy_health = def->GetPropertyValueFloat("ENEMY_HEALTH", enemy_health);
	enemy_damage  = def->GetPropertyValueFloat("ENEMY_DAMAGE", enemy_damage);
	scale =  def->GetPropertyValueFloat("SCALE", scale);

	return true;
}


