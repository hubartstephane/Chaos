#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/BitmapAtlas.h>
#include <chaos/ReferencedObject.h>
#include <chaos/NamedObject.h>
#include <chaos/ParticleAccessor.h>

#include <death/Game.h>
#include <death/GameInstance.h>
#include <death/Player.h>
#include <death/GameFramework.h>

#include "Ludum45Particles.h"


class EnemyPattern : public chaos::ReferencedObject
{

public:

	virtual bool Initialize(chaos::TiledMap::ObjectTypeDefinition const * def);


	
	bool UpdateParticle(float delta_time, ParticleEnemy * particle, chaos::box2 const & player_box, chaos::box2 const & camera_box);

public:

	std::string name;

	size_t enemy_count = 0;
	float particle_initial_range = 0.0f;
	float particle_speed = 0.0f;
	int   pattern_index = 1;
};




class EnemyType : public chaos::ReferencedObject
{
public:

	virtual bool Initialize(chaos::TiledMap::ObjectTypeDefinition const * def);


public:

	std::string name;

	std::string bitmap_name;
	float enemy_life = 0.0f;
	float enemy_damage = 0.0f;
};