#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
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

	/** constructor */
	using death::TiledMapTriggerObject::TiledMapTriggerObject;

	void ResetTrigger() { reset_trigger = true; }

	/** override */
	virtual bool Initialize(chaos::TiledMap::GeometricObject* in_geometric_object) override;

protected:

	/** called whenever a collision with player is detected (returns false, if loop is to be broken) */
	virtual bool OnPlayerCollisionEvent(float delta_time, death::Player * player, chaos::CollisionType event_type) override;

public:

	bool reset_trigger = false;
	bool decrease_power = false;
};

// =================================================
// SpeedUpTriggerObject
// =================================================

class SpeedUpTriggerObject : public death::TiledMapTriggerObject
{

public:

	/** constructor */
	using death::TiledMapTriggerObject::TiledMapTriggerObject;
	/** override */
	virtual bool Initialize(chaos::TiledMap::GeometricObject* in_geometric_object) override;

protected:

	/** called whenever a collision with player is detected (returns false, if loop is to be broken) */
	virtual bool OnPlayerCollisionEvent(float delta_time, death::Player * player, chaos::CollisionType event_type) override;

protected:

	float scroll_speed = 1.0f;
};


// =================================================
// SpawnerTriggerObject
// =================================================

class SpawnerTriggerObject : public death::TiledMapTriggerObject
{

public:

	/** constructor */
	using death::TiledMapTriggerObject::TiledMapTriggerObject;
	/** override */
	virtual bool Initialize(chaos::TiledMap::GeometricObject* in_geometric_object) override;

protected:

	/** called whenever a collision with player is detected (returns false, if loop is to be broken) */
	virtual bool OnCameraCollisionEvent(float delta_time, chaos::box2 const & camera_box, chaos::CollisionType event_type) override;

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

public:

	/** constructor */
	LudumLevel();

protected:

	/** override */
	virtual death::LevelInstance * DoCreateLevelInstance(death::Game * in_game) override;

	virtual chaos::ParticleLayerBase * DoCreateParticleLayer(death::TiledMapLayerInstance * layer_instance) override;

	virtual death::GeometricObjectFactory DoGetGeometricObjectFactory(death::TiledMapLayerInstance * in_layer_instance, chaos::TiledMap::TypedObject* in_typed_object) override;
};
