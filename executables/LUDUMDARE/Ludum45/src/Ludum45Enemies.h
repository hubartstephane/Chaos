#pragma once

#include <chaos/Chaos.h>

#include <death/Game.h>
#include <death/GameInstance.h>
#include <death/Player.h>
#include <death/GameFramework.h>

#include "Ludum45Particles.h"


class EnemyPattern : public chaos::Object
{

public:

	virtual bool Initialize(chaos::TiledMap::PropertyOwner const * def);


	
	bool UpdateParticle(float delta_time, ParticleEnemy * particle, chaos::box2 const & camera_box);

public:

	std::string name;

	size_t enemy_count = 0;
	float particle_initial_range = 0.0f;
	float particle_speed = 0.0f;
	int   pattern_index = 1;
};




class EnemyType : public chaos::Object
{
public:

	virtual bool Initialize(chaos::TiledMap::ObjectTypeDefinition const * def);


public:

	std::string name;

	std::string bitmap_name;
	float enemy_health = 0.0f;
	float enemy_damage = 0.0f;
	float scale = 1.0f;
};