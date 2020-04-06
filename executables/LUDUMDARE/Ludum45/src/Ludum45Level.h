#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
#include <chaos/TiledMap.h>
#include <chaos/TiledMapTools.h>

#include <death/Level.h>
#include <death/TiledMapLevel.h>
#include <death/Game.h>
#include <death/GameFramework.h>

#include "Ludum45Game.h"
#include "Ludum45Enemies.h"

// =================================================
// EnemySpawnerTriggerObject
// =================================================

class EnemySpawnerTriggerObject : public death::TiledMapTriggerObject
{
	DEATH_TILEDLEVEL_ALL_FRIENDS

public:

	/** constructor */
	using death::TiledMapTriggerObject::TiledMapTriggerObject;
	/** override */
	virtual bool Initialize(chaos::TiledMap::GeometricObject* in_geometric_object) override;

protected:

	/** override */
	virtual bool OnCameraCollisionEvent(float delta_time, chaos::box2 const & camera_box, chaos::CollisionType event_type) override;

protected:

	EnemyPattern * pattern = nullptr;
	EnemyType * type = nullptr;

	std::string enemy_type;
	std::string enemy_pattern;
};

// =================================================
// BonusSpawnerTriggerObject
// =================================================

class BonusSpawnerTriggerObject : public death::TiledMapTriggerObject
{
	DEATH_TILEDLEVEL_ALL_FRIENDS

public:

	/** constructor */
	using death::TiledMapTriggerObject::TiledMapTriggerObject;
	/** override */
	virtual bool Initialize(chaos::TiledMap::GeometricObject* in_geometric_object) override;

protected:

	/** override */
	virtual bool OnCameraCollisionEvent(float delta_time, chaos::box2 const & camera_box, chaos::CollisionType event_type) override;

protected:

	std::string bonus_name;
	chaos::TagType bonus_type;
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

	virtual death::GeometricObjectFactory DoGetGeometricObjectFactory(death::TiledMapLayerInstance * in_layer_instance, chaos::TiledMap::GeometricObject * in_geometric_object) override;

	virtual bool OnPlayerTileCollision(float delta_time, class death::Player * player, death::TiledMapParticle * particle) override;
};
