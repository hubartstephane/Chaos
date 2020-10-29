#pragma once

#include <chaos/Chaos.h>

#include "Ludum44Game.h"

// =================================================
// PowerUpTrigger
// =================================================

class PowerUpTrigger : public chaos::TMTrigger
{
	CHAOS_OBJECT_DECLARE_CLASS2(PowerUpTrigger, chaos::TMTrigger);

public:

	void ResetTrigger() { reset_trigger = true; }

protected:

	/** override */
	virtual bool Initialize(chaos::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object, chaos::TMObjectReferenceSolver& in_reference_solver) override;
	/** override */
	virtual bool OnCollisionEvent(float delta_time, chaos::Object* object, chaos::CollisionType event_type) override;

public:

	bool reset_trigger = false;
	bool decrease_power = false;
};

// =================================================
// SpeedUpTrigger
// =================================================

class SpeedUpTrigger : public chaos::TMTrigger
{

	CHAOS_OBJECT_DECLARE_CLASS2(SpeedUpTrigger, chaos::TMTrigger);

protected:

	/** override */
	virtual bool Initialize(chaos::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object, chaos::TMObjectReferenceSolver& in_reference_solver) override;
	/** override */
	virtual bool OnCollisionEvent(float delta_time, chaos::Object* object, chaos::CollisionType event_type) override;

protected:

	float scroll_speed = 1.0f;
};


// =================================================
// SpawnerTrigger
// =================================================

class SpawnerTrigger : public chaos::TMTrigger
{

	CHAOS_OBJECT_DECLARE_CLASS2(SpawnerTrigger, chaos::TMTrigger);

protected:

	/** override */
	virtual bool Initialize(chaos::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object, chaos::TMObjectReferenceSolver& in_reference_solver) override;
	/** override */
	virtual bool OnCollisionEvent(float delta_time, chaos::Object * object, chaos::CollisionType event_type) override;

public:

	float scale_factor = 1.0f;
	float life_factor = 1.0f;
	int   count = 10;
	int   spawn_curve_type = 0;
	int   spawn_enemy_type = 0;
	int   spawn_move_type = 0;
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
};
