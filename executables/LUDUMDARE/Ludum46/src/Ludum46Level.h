#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/Object.h>
#include <chaos/TiledMap.h>
#include <chaos/TiledMapTools.h>

#include <death/Level.h>
#include <death/TiledMapLevel.h>
#include <death/Game.h>
#include <death/GameFramework.h>

#include "Ludum46Game.h"


// =================================================
// EffectorObject
// =================================================

class EffectorObject
{
public:

	/** no override here */
	virtual bool Initialize(death::TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object);

	virtual void SetEffectorState(bool in_active)
	{
		if (active == in_active)
			return;
		active = in_active;
		OnEffectorChangeState();
	}

	virtual void OnEffectorChangeState() {}

protected:

	bool active = false;



	float delay_between_triggers = 1.0f;

	float last_time_triggered = -1;



};


class SpawnerObject : public death::TiledMapGeometricObject
{
	DEATH_TILEDLEVEL_ALL_FRIENDS

public:

	/** override */
	virtual bool DoTick(float delta_time);

	int GetRemainingParticleCount() const;

protected:

	/** override */
	virtual bool Initialize(death::TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object) override;

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
	bool emission_started = true;
};

// =================================================
// FireSpawnerObject
// =================================================

class FireSpawnerObject : public SpawnerObject, public EffectorObject
{
	DEATH_TILEDLEVEL_ALL_FRIENDS

public:

	/** override */
	virtual bool Initialize(death::TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object) override;

	virtual void OnEffectorChangeState() override;


};


// =================================================
// SoulSpawnerObject
// =================================================

class SoulSpawnerObject : public SpawnerObject
{
	DEATH_TILEDLEVEL_ALL_FRIENDS




};

// =================================================
// SpikeBarObject
// =================================================

class SpikeBarObject : public death::TiledMapGeometricObject, public EffectorObject
{
	DEATH_TILEDLEVEL_ALL_FRIENDS

public:

	virtual void OnEffectorChangeState() override;

protected:

	/** override */
	virtual bool Initialize(death::TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object) override;
};

// =================================================
// SoulTriggerObject
// =================================================

class SoulTriggerObject : public death::TiledMapGeometricObject
{
	DEATH_TILEDLEVEL_ALL_FRIENDS

public:

	/** override */
	virtual bool DoTick(float delta_time);

	virtual bool AddTriggerCount();

protected:

	/** override */
	virtual bool Initialize(death::TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object) override;

public:

	std::vector<std::string> effector_names;

	int trigger_limit = 0;

protected:

	int trigger_count = 0;

};

// =================================================
// SoulBurnTriggerObject
// =================================================


class SoulBurnTriggerObject : public SoulTriggerObject
{
	DEATH_TILEDLEVEL_ALL_FRIENDS

public:

	virtual bool AddTriggerCount() override;
};



// =================================================
// Levels
// =================================================

class LudumLevel : public death::TiledMapLevel
{
	friend class LudumLevelInstance;

	CHAOS_OBJECT_DECLARE_CLASS2(LudumLevel, death::TiledMapLevel);

	LudumLevel();

protected:

	virtual chaos::ParticleLayerBase * DoCreateParticleLayer(death::TiledMapLayerInstance * layer_instance) override;

	virtual death::GeometricObjectFactory DoGetGeometricObjectFactory(death::TiledMapLayerInstance * in_layer_instance, chaos::TiledMap::TypedObject* in_typed_object) override;

	virtual bool Initialize(chaos::TiledMap::Map* in_tiled_map) override;

public:

	int required_souls = 0;

	float flame_initial_health = 90.0f;

	float flame_health_lost_per_second = 0.5f;

};
