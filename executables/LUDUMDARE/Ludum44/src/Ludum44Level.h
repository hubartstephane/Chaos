#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/Object.h>
#include <chaos/TiledMap.h>

#include <death/Level.h>
#include <death/TM.h>
#include <death/Game.h>
#include <death/GameFramework.h>

#include "Ludum44Game.h"

// =================================================
// PowerUpTrigger
// =================================================

class PowerUpTrigger : public death::TMTrigger
{
	CHAOS_OBJECT_DECLARE_CLASS2(PowerUpTrigger, death::TMTrigger);

public:

	void ResetTrigger() { reset_trigger = true; }

protected:

	/** override */
	virtual bool Initialize(death::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object, death::TMObjectReferenceRegistry& in_reference_registry) override;
	/** override */
	virtual bool OnCollisionEvent(float delta_time, chaos::Object* object, chaos::CollisionType event_type) override;

public:

	bool reset_trigger = false;
	bool decrease_power = false;
};

// =================================================
// SpeedUpTrigger
// =================================================

class SpeedUpTrigger : public death::TMTrigger
{

	CHAOS_OBJECT_DECLARE_CLASS2(SpeedUpTrigger, death::TMTrigger);

protected:

	/** override */
	virtual bool Initialize(death::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object, death::TMObjectReferenceRegistry& in_reference_registry) override;
	/** override */
	virtual bool OnCollisionEvent(float delta_time, chaos::Object* object, chaos::CollisionType event_type) override;

protected:

	float scroll_speed = 1.0f;
};


// =================================================
// SpawnerTrigger
// =================================================

class SpawnerTrigger : public death::TMTrigger
{

	CHAOS_OBJECT_DECLARE_CLASS2(SpawnerTrigger, death::TMTrigger);

protected:

	/** override */
	virtual bool Initialize(death::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object, death::TMObjectReferenceRegistry& in_reference_registry) override;
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

class LudumLevel : public death::TMLevel
{
	friend class LudumLevelInstance;

	CHAOS_OBJECT_DECLARE_CLASS2(LudumLevel, death::TMLevel);

	LudumLevel();

protected:

	virtual chaos::ParticleLayerBase * DoCreateParticleLayer(death::TMLayerInstance * layer_instance) override;

	virtual death::TMObjectFactory DoGetObjectFactory(death::TMLayerInstance * in_layer_instance, chaos::TiledMap::TypedObject const * in_typed_object) override;
};
