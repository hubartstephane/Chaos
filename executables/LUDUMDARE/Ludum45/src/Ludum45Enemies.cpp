#include "Ludum45Enemies.h"

#include <chaos/TiledMap.h>

bool EnemyPattern::Initialize(chaos::TiledMap::ObjectTypeDefinition const * def)
{
	enemy_count = def->FindPropertyInt("ENEMY_COUNT", 0);

	return true;
}

bool EnemyPattern::UpdateParticle(float delta_time, ParticleEnemy * particle, chaos::box2 const & player_box)
{
	float base_offset = chaos::MathTools::CastAndDiv<float>(particle->enemy_index, particle->enemy_particle_count);


	float R = glm::length(particle->spawner_surface.half_size);

	 glm::vec2 p = particle->spawner_surface.position;

	 float alpha = 2.0f * (float)M_PI * (base_offset + particle->time);

	 p.x += R * cosf(alpha);
	 p.y += R * sin(alpha);



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


