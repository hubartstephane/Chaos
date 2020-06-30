#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/Object.h>
#include <chaos/TiledMap.h>
#include <chaos/TiledMapTools.h>

#include <death/Level.h>
#include <death/TiledMapLevel.h>
#include <death/Game.h>
#include <death/GameFramework.h>

#include "Ludum44Game.h"

// =================================================
// PowerUpTriggerObject
// =================================================

class PowerUpTriggerObject : public death::TiledMapTriggerObject
{

public:

	void ResetTrigger() { reset_trigger = true; }

protected:

	/** override */
	virtual bool Initialize(death::TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object) override;
	/** override */
	virtual bool OnCollisionEvent(float delta_time, chaos::Object* object, chaos::CollisionType event_type) override;

public:

	bool reset_trigger = false;
	bool decrease_power = false;
};

// =================================================
// SpeedUpTriggerObject
// =================================================

class SpeedUpTriggerObject : public death::TiledMapTriggerObject
{

protected:

	/** override */
	virtual bool Initialize(death::TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object) override;
	/** override */
	virtual bool OnCollisionEvent(float delta_time, chaos::Object* object, chaos::CollisionType event_type) override;

protected:

	float scroll_speed = 1.0f;
};


// =================================================
// SpawnerTriggerObject
// =================================================

class SpawnerTriggerObject : public death::TiledMapTriggerObject
{

protected:

	/** override */
	virtual bool Initialize(death::TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object) override;
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

class LudumLevel : public death::TiledMapLevel
{
	friend class LudumLevelInstance;

	CHAOS_OBJECT_DECLARE_CLASS2(LudumLevel, death::TiledMapLevel);

	LudumLevel();

protected:

	virtual chaos::ParticleLayerBase * DoCreateParticleLayer(death::TiledMapLayerInstance * layer_instance) override;

	virtual death::GeometricObjectFactory DoGetGeometricObjectFactory(death::TiledMapLayerInstance * in_layer_instance, chaos::TiledMap::TypedObject* in_typed_object) override;
};
