#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
#include <chaos/TiledMap.h>
#include <chaos/TiledMapTools.h>

#include <death/Level.h>
#include <death/TiledMapLevel.h>
#include <death/Game.h>
#include <death/GameFramework.h>

#include "Ludum46Game.h"



class SpawnerObject : public death::TiledMapGeometricObject
{
	DEATH_TILEDLEVEL_ALL_FRIENDS

public:

	/** constructor */
	using death::TiledMapGeometricObject::TiledMapGeometricObject;

	/** override */
	virtual bool Initialize(chaos::TiledMap::GeometricObject* in_geometric_object) override;

	virtual bool DoTick(float delta_time);

	int GetRemainingParticleCount() const;

protected:

	void SpawnParticles(chaos::ParticleSpawner & spawner, int count);

protected:

	int max_spawned_particles = -1;

	float spawn_per_second = 10.0f;

	glm::vec2 particle_start_velocity = glm::vec2(0.0f, 0.0f);

	std::string target_layer;

	std::string spawned_particle;

	float particle_duration = -1.0f;

protected:

	int spawned_count = 0;
	float nospawn_time_cumulated = 0.0f; // accumulated time from previous frames with no spawn

};

// =================================================
// FireSpawnerObject
// =================================================

class FireSpawnerObject : public SpawnerObject
{
	DEATH_TILEDLEVEL_ALL_FRIENDS

public:

	/** constructor */
	using SpawnerObject::SpawnerObject;

};


// =================================================
// SoulSpawnerObject
// =================================================

class SoulSpawnerObject : public SpawnerObject
{
	DEATH_TILEDLEVEL_ALL_FRIENDS

public:

	/** constructor */
	using SpawnerObject::SpawnerObject;

};


// =================================================
// Levels
// =================================================

class LudumLevel : public death::TiledMapLevel
{
	friend class LudumLevelInstance;

public:

	/** constructor */
	LudumLevel();

protected:

	/** override */
	virtual death::LevelInstance * DoCreateLevelInstance(death::Game * in_game) override;

	virtual chaos::ParticleLayerBase * DoCreateParticleLayer(death::TiledMapLayerInstance * layer_instance) override;

	virtual death::GeometricObjectFactory DoGetGeometricObjectFactory(death::TiledMapLayerInstance * in_layer_instance, chaos::TiledMap::TypedObject* in_typed_object) override;

	virtual bool Initialize(chaos::TiledMap::Map* in_tiled_map) override;

protected:

	int required_souls = 0;

};
