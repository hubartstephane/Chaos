#pragma once

#include <chaos/Chaos.h>

#include "Ludum46Game.h"


// =================================================
// EffectorObject
// =================================================

class EffectorObject
{
public:

	/** no override here */
	virtual bool Initialize(chaos::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object, chaos::TMObjectReferenceSolver& in_reference_solver);

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


class Spawner : public chaos::TMObject
{
	DEATH_TILEDLEVEL_ALL_FRIENDS;

public:

	CHAOS_OBJECT_DECLARE_CLASS2(Spawner, chaos::TMObject);

	/** override */
	virtual bool DoTick(float delta_time);

	int GetRemainingParticleCount() const;

protected:

	/** override */
	virtual bool Initialize(chaos::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object, chaos::TMObjectReferenceSolver& reference_solver) override;

	virtual bool SerializeIntoJSON(nlohmann::json& json) const override;

	virtual bool SerializeFromJSON(nlohmann::json const& json) override;

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
// FireSpawner
// =================================================

class FireSpawner : public Spawner, public EffectorObject
{
	DEATH_TILEDLEVEL_ALL_FRIENDS;

	CHAOS_OBJECT_DECLARE_CLASS2(FireSpawner, Spawner);

public:

	/** override */
	virtual bool Initialize(chaos::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object, chaos::TMObjectReferenceSolver& reference_solver) override;

	virtual void OnEffectorChangeState() override;


};


// =================================================
// SoulSpawner
// =================================================

class SoulSpawner : public Spawner
{
	DEATH_TILEDLEVEL_ALL_FRIENDS;

	CHAOS_OBJECT_DECLARE_CLASS2(SoulSpawner, Spawner);




};

// =================================================
// SpikeBar
// =================================================

class SpikeBar : public chaos::TMObject, public EffectorObject
{
	DEATH_TILEDLEVEL_ALL_FRIENDS;

	CHAOS_OBJECT_DECLARE_CLASS2(SpikeBar, chaos::TMObject);

public:

	virtual void OnEffectorChangeState() override;

protected:

	/** override */
	virtual bool DoTick(float delta_time);

	/** override */
	virtual bool Initialize(chaos::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object, chaos::TMObjectReferenceSolver& reference_solver) override;


	float path_value = 0.0f;

	chaos::weak_ptr<TMObject> my_path;

};

// =================================================
// SoulTrigger
// =================================================

class SoulTrigger : public chaos::TMObject
{
	DEATH_TILEDLEVEL_ALL_FRIENDS;

	CHAOS_OBJECT_DECLARE_CLASS2(SoulTrigger, chaos::TMObject);

public:

	/** override */
	virtual bool DoTick(float delta_time);

	virtual bool AddTriggerCount();

protected:

	/** override */
	virtual bool Initialize(chaos::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object, chaos::TMObjectReferenceSolver& reference_solver) override;

public:

	std::vector<std::string> effector_names;

	int trigger_limit = 0;

protected:

	int trigger_count = 0;

	chaos::weak_ptr<TMObject> myreference;

};

// =================================================
// SoulBurnTrigger
// =================================================


class SoulBurnTrigger : public SoulTrigger
{
	DEATH_TILEDLEVEL_ALL_FRIENDS;

	CHAOS_OBJECT_DECLARE_CLASS2(SoulBurnTrigger, SoulTrigger);

public:

	virtual bool AddTriggerCount() override;
};



// =================================================
// Levels
// =================================================

class LudumLevel : public chaos::TMLevel
{
	friend class LudumLevelInstance;

	CHAOS_OBJECT_DECLARE_CLASS2(LudumLevel, chaos::TMLevel);

	LudumLevel();

protected:

	virtual chaos::ParticleLayerBase * DoCreateParticleLayer(chaos::TMLayerInstance * layer_instance) override;

	virtual chaos::TMObjectFactory DoGetObjectFactory(chaos::TMLayerInstance * in_layer_instance, chaos::TiledMap::TypedObject const * in_typed_object) override;

	virtual bool Initialize(chaos::TiledMap::Map* in_tiled_map) override;

public:

	int required_souls = 0;

	float flame_initial_health = 90.0f;

	float flame_health_lost_per_second = 0.5f;

};
