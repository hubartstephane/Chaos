#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
#include <chaos/TiledMap.h>
#include <chaos/TiledMapTools.h>

#include <death/GameLevel.h>
#include <death/TiledMapLevel.h>
#include <death/Game.h>
#include <death/GameFramework.h>

#include "Ludum43Game.h"

// =================================================
// MyFinishingTriggerObject
// =================================================

class MyFinishingTriggerObject : public death::TiledMap::TriggerObject
{

public:

	/** constructor */
	MyFinishingTriggerObject(death::TiledMap::LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObjectSurface * in_surface_object);

	/** override */
	virtual bool Initialize() override;

protected:

	/** called whenever a collision with player is detected (returns false, if loop is to be broken) */
	virtual bool OnPlayerCollisionEvent(float delta_time, death::Player * player, chaos::ParticleDefault::Particle * player_particle, chaos::CollisionType event_type) override;
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

	virtual chaos::ParticleLayerBase * DoCreateParticleLayer(death::TiledMap::LayerInstance * layer_instance) override;

	virtual bool FinalizeLayerParticles(death::TiledMap::LayerInstance * layer_instance, chaos::ParticleAllocationBase * allocation) override;

	virtual death::TiledMap::GeometricObject * DoCreateGeometricObject(death::TiledMap::LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object) override;
};
