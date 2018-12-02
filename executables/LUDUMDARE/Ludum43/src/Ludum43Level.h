#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
#include <chaos/TiledMap.h>
#include <chaos/TiledMapTools.h>

#include <death/GameLevel.h>
#include <death/TiledMapLevel.h>
#include <death/Game.h>

#include "Ludum43Game.h"

// =================================================
// FinishingTriggerSurfaceObject
// =================================================

class FinishingTriggerSurfaceObject : public death::TiledMap::TriggerSurfaceObject
{

public:

	/** constructor */
	FinishingTriggerSurfaceObject(death::TiledMap::LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object);

protected:

	/** called whenever a collision with player is detected (returns false, if loop is to be broken) */
	virtual bool OnPlayerCollision(double delta_time, chaos::ParticleDefault::Particle * player_particle);
};


// =================================================
// Levels
// =================================================

class AtomGameObject : public death::TiledMap::GeometricObject
{
public:

	AtomGameObject(death::TiledMap::LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object);


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
	virtual death::TiledMap::GeometricObject * DoCreateTypedObject(death::TiledMap::LayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object) override;

	/** override */
	virtual death::GameLevelInstance * DoCreateLevelInstance(death::Game * in_game) override;

	virtual chaos::ParticleLayer * CreateParticleLayer(death::TiledMap::LayerInstance * layer_instance) override;

	virtual bool FinalizeLayerParticles(death::TiledMap::LayerInstance * layer_instance, chaos::ParticleAllocation * allocation) override;
};

// =================================================
// LevelInstance
// =================================================

class LudumLevelInstance : public death::TiledMap::LevelInstance
{
public:

	/** constructor */
	LudumLevelInstance(class LudumGame * in_game);

protected:

	/** get the typed level */
	LudumLevel * GetTypedLevel();
	LudumLevel const * GetTypedLevel() const;

	virtual bool IsLevelCompleted() const override;

protected:

	/** pointer on game */
	class LudumGame * game = nullptr;
};
