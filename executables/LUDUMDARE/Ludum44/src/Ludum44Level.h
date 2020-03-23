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

protected:

	/** called whenever a collision with player is detected (returns false, if loop is to be broken) */
	virtual bool OnPlayerCollisionEvent(float delta_time, death::Player * player, chaos::CollisionType event_type) override;

protected:

	bool reset_trigger = false;
};

// =================================================
// SpeedUpTriggerObject
// =================================================

class SpeedUpTriggerObject : public death::TiledMapTriggerObject
{

public:

	/** constructor */
	using death::TiledMapTriggerObject::TiledMapTriggerObject;

protected:

	/** called whenever a collision with player is detected (returns false, if loop is to be broken) */
	virtual bool OnPlayerCollisionEvent(float delta_time, death::Player * player, chaos::CollisionType event_type) override;
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
	virtual bool Initialize() override;

protected:

	/** called whenever a collision with player is detected (returns false, if loop is to be broken) */
	virtual bool OnCameraCollisionEvent(float delta_time, chaos::box2 const & camera_box, chaos::CollisionType event_type) override;
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

	virtual death::TiledMapGeometricObject * DoCreateGeometricObject(death::TiledMapLayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object) override;

	virtual bool OnPlayerTileCollision(float delta_time, class death::Player * player, death::TiledMapParticle * particle) override;

};
