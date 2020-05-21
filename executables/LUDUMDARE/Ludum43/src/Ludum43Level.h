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
	using death::TiledMapTriggerObject::TiledMapTriggerObject;
	/** override */
	virtual bool Initialize(chaos::TiledMap::GeometricObject* in_geometric_object) override;

protected:

	/** override */
	virtual bool OnCollisionEvent(float delta_time, chaos::ReferencedObject* object, chaos::CollisionType event_type) override;
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

	virtual death::GeometricObjectFactory DoGetGeometricObjectFactory(death::TiledMapLayerInstance * in_layer_instance, chaos::TiledMap::TypedObject* in_typed_object) override;
};
