#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
#include <chaos/TiledMap.h>
#include <chaos/TiledMapTools.h>

#include <death/GameLevel.h>
#include <death/TiledMapLevel.h>
#include <death/Game.h>
#include <death/GameFramework.h>

#include "Ludum44Game.h"

// =================================================
// PowerUpTriggerObject
// =================================================

class PowerUpTriggerObject : public death::TiledMap::TriggerObject
{

public:

	/** constructor */
	using death::TiledMap::TriggerObject::TriggerObject;

	void ResetTrigger() { reset_trigger = true; }

protected:

	/** called whenever a collision with player is detected (returns false, if loop is to be broken) */
	virtual bool OnPlayerCollisionEvent(double delta_time, death::Player * player, chaos::ParticleDefault::Particle * player_particle, int event_type) override;

protected:

	bool reset_trigger = false;
};

// =================================================
// SpeedUpTriggerObject
// =================================================

class SpeedUpTriggerObject : public death::TiledMap::TriggerObject
{

public:

	/** constructor */
	using death::TiledMap::TriggerObject::TriggerObject;

protected:

	/** called whenever a collision with player is detected (returns false, if loop is to be broken) */
	virtual bool OnPlayerCollisionEvent(double delta_time, death::Player * player, chaos::ParticleDefault::Particle * player_particle, int event_type) override;
};


// =================================================
// SpawnerTriggerObject
// =================================================

class SpawnerTriggerObject : public death::TiledMap::TriggerObject
{

public:

	/** constructor */
	using death::TiledMap::TriggerObject::TriggerObject;

	/** override */
	virtual bool Initialize() override;

protected:

	/** called whenever a collision with player is detected (returns false, if loop is to be broken) */
	virtual bool OnCameraCollisionEvent(double delta_time, chaos::box2 const & camera_box, int event_type) override;
};



// =================================================
// Levels
// =================================================

class LudumLevel : public death::TiledMap::Level
{
	friend class LudumLevelInstance;

public:

	/** constructor */
	LudumLevel();

protected:

	/** override */
	virtual death::GameLevelInstance * DoCreateLevelInstance(death::Game * in_game) override;

	virtual bool FinalizeLayerParticles(death::TiledMap::LayerInstance * layer_instance, chaos::ParticleAllocationBase * allocation) override;

	virtual chaos::ParticleLayerBase * DoCreateParticleLayer(death::TiledMap::LayerInstance * layer_instance) override;

	virtual death::TiledMap::GeometricObject * DoCreateGeometricObject(death::TiledMap::LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object) override;

	virtual bool OnPlayerTileCollision(double delta_time, class death::Player * player, chaos::ParticleDefault::Particle * player_particle, death::TiledMap::TileParticle * particle) override;

};
