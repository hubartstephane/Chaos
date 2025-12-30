#pragma once

#include "Ludum47PCH.h"
#include "Ludum47Game.h"
#include "Ludum47Particles.h"


// =================================================
// LudumSpeedIndication
// =================================================

class ParticleSpawnerDelay
{
public:

	int GetSpawnCount(float delta_time);

	int max_spawned_particles = -1;

	float spawn_per_second = 10.0f;

	int spawned_count = 0;
	float nospawn_time_cumulated = 0.0f; // accumulated time from previous frames with no spawn

	bool emission_started = true;


};


// =================================================
// LudumSpeedIndication
// =================================================

class LudumSpeedIndication : public chaos::TMObject
{
	CHAOS_DECLARE_OBJECT_CLASS(LudumSpeedIndication, chaos::TMObject);

protected:

	virtual bool Initialize(chaos::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const* in_geometric_object, chaos::TMObjectReferenceSolver& reference_solver);

public:

	float speed_factor = 1.0f;

};

// =================================================
// LudumRoad
//=================================================

enum class RoadUpdateValue
{
	NOP,
	NEW_CHECKPOINT,
	WRONG_WAY,
	NEW_LAP,
	END_OF_RACE
};

class RoadPoint
{
public:

	glm::vec2 position;
	float speed_factor = 1.0f;

};

class LudumRoad : public chaos::TMObject
{
	CHAOS_DECLARE_OBJECT_CLASS(LudumRoad, chaos::TMObject);

public:

	RoadUpdateValue UpdateRacePosition(RacePosition& race_position, glm::vec2 const& p, bool player) const;


	float GetSpeedFactor(glm::vec2 const& p) const;

protected:

	virtual bool Initialize(chaos::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const* in_geometric_object, chaos::TMObjectReferenceSolver& reference_solver);

	virtual void OnLevelStarted() override;

	virtual bool DoTick(float delta_time) override;

public:

	std::vector<RoadPoint> points;

	int lap_count = 3;

	float point_speed_factor = 0.5f;

	float player_angular_tweak = 1.0f;
	float opponent_angular_tweak = 1.0f;

	float player_velocity_tweak = 1.0f;
	float opponent_velocity_tweak = 1.0f;

	float checkpoint_short_distance = 50.0f; // distance for validation

	float checkpoint_long_distance = 200.0f; // distance for validation

};

// =================================================
// LudumCollision
//=================================================

class LudumCollision : public chaos::TMObject
{
	CHAOS_DECLARE_OBJECT_CLASS(LudumCollision, chaos::TMObject);

public:

protected:

	virtual bool Initialize(chaos::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const* in_geometric_object, chaos::TMObjectReferenceSolver& reference_solver);

	virtual void OnLevelStarted() override;

public:

	chaos::box2 internal_bounding_box;

	std::vector<glm::vec2> points;
};


// =================================================
// LudumOpponent
// =================================================

class LudumOpponent : public chaos::TMObject
{
	CHAOS_DECLARE_OBJECT_CLASS(LudumOpponent, chaos::TMObject);

protected:

	virtual bool Initialize(chaos::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const* in_geometric_object, chaos::TMObjectReferenceSolver& reference_solver);

	virtual void OnLevelStarted() override;

	virtual bool DoTick(float delta_time) override;

public:

	chaos::shared_ptr<LudumRoad> road;

	RacePosition race_position;

	CarData car_data;

	float max_health = 4.0f;
	float health = 4.0f;

	ParticleSpawnerDelay spawner_delay;

};



// =================================================
// Levels
// =================================================

class LudumLevel : public chaos::TMLevel
{
	friend class LudumLevelInstance;

	CHAOS_DECLARE_OBJECT_CLASS(LudumLevel, chaos::TMLevel);

	LudumLevel();

protected:

	virtual chaos::ParticleLayerBase * DoCreateParticleLayer(chaos::TMLayerInstance * layer_instance) override;

	virtual chaos::TMObjectFactory DoGetObjectFactory(chaos::TMLayerInstance * in_layer_instance, chaos::TiledMap::TypedObject const * in_typed_object) override;

	virtual bool Initialize(chaos::TiledMap::Map* in_tiled_map) override;

public:



};
