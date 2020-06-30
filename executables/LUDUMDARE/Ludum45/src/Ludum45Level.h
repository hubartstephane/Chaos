#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/Object.h>
#include <chaos/TiledMap.h>
#include <chaos/TiledMapTools.h>

#include <death/Level.h>
#include <death/TiledMapLevel.h>
#include <death/Game.h>
#include <death/GameFramework.h>

#include "Ludum45Game.h"
#include "Ludum45Enemies.h"

// =================================================
// EnemySpawnerTrigger
// =================================================

class EnemySpawnerTrigger : public death::TiledMapTrigger
{
	DEATH_TILEDLEVEL_ALL_FRIENDS

	CHAOS_OBJECT_DECLARE_CLASS2(EnemySpawnerTrigger, death::TiledMapTrigger);

protected:

	/** override */
	virtual bool Initialize(death::TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object) override;
	/** override */
	virtual bool OnCollisionEvent(float delta_time, chaos::Object* object, chaos::CollisionType event_type) override;

protected:

	EnemyPattern * pattern = nullptr;
	EnemyType * type = nullptr;

	std::string enemy_type;
	std::string enemy_pattern;
};

// =================================================
// BonusSpawnerTrigger
// =================================================

class BonusSpawnerTrigger : public death::TiledMapTrigger
{
	DEATH_TILEDLEVEL_ALL_FRIENDS

	CHAOS_OBJECT_DECLARE_CLASS2(BonusSpawnerTrigger, death::TiledMapTrigger);

protected:

	/** override */
	virtual bool Initialize(death::TiledMapLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject* in_geometric_object) override;
	/** override */
	virtual bool OnCollisionEvent(float delta_time, chaos::Object* object, chaos::CollisionType event_type) override;

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

	CHAOS_OBJECT_DECLARE_CLASS2(LudumLevel, death::TiledMapLevel);

	LudumLevel();

protected:

	virtual chaos::ParticleLayerBase * DoCreateParticleLayer(death::TiledMapLayerInstance * layer_instance) override;

	virtual death::GeometricObjectFactory DoGetObjectFactory(death::TiledMapLayerInstance * in_layer_instance, chaos::TiledMap::TypedObject* in_typed_object) override;
};
