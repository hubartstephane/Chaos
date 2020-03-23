#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
#include <chaos/TiledMap.h>
#include <chaos/TiledMapTools.h>

#include <death/Level.h>
#include <death/TiledMapLevel.h>
#include <death/Game.h>
#include <death/GameFramework.h>

#include "Ludum43Game.h"

// =================================================
// MyFinishingTriggerObject
// =================================================

class MyFinishingTriggerObject : public death::TiledMapTriggerObject
{

public:

	/** constructor */
	MyFinishingTriggerObject(death::TiledMapLayerInstance * in_layer_instance, chaos::TiledMap::GeometricObjectSurface * in_surface_object);

	/** override */
	virtual bool Initialize() override;

protected:

	/** called whenever a collision with player is detected (returns false, if loop is to be broken) */
	virtual bool OnPlayerCollisionEvent(float delta_time, death::Player * player, chaos::CollisionType event_type) override;
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

	virtual bool FinalizeLayerParticles(death::TiledMapLayerInstance * layer_instance, chaos::ParticleAllocationBase * allocation) override;

	virtual death::TiledMapGeometricObject * DoCreateGeometricObject(death::TiledMapLayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object) override;
};
