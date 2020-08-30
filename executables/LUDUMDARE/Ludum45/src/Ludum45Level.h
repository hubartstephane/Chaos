#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/Object.h>
#include <chaos/TiledMap.h>

#include <death/Level.h>
#include <death/TM.h>
#include <death/Game.h>
#include <death/GameFramework.h>

#include "Ludum45Game.h"
#include "Ludum45Enemies.h"

// =================================================
// EnemySpawnerTrigger
// =================================================

class EnemySpawnerTrigger : public death::TMTrigger
{
	DEATH_TILEDLEVEL_ALL_FRIENDS;

	CHAOS_OBJECT_DECLARE_CLASS2(EnemySpawnerTrigger, death::TMTrigger);

protected:

	/** override */
	virtual bool Initialize(death::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object, death::TMObjectReferenceSolver & in_reference_solver) override;
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

class BonusSpawnerTrigger : public death::TMTrigger
{
	DEATH_TILEDLEVEL_ALL_FRIENDS;

	CHAOS_OBJECT_DECLARE_CLASS2(BonusSpawnerTrigger, death::TMTrigger);

protected:

	/** override */
	virtual bool Initialize(death::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object, death::TMObjectReferenceSolver& in_reference_solver) override;
	/** override */
	virtual bool OnCollisionEvent(float delta_time, chaos::Object* object, chaos::CollisionType event_type) override;

protected:

	std::string bonus_name;
	chaos::TagType bonus_type;
};

// =================================================
// Levels
// =================================================

class LudumLevel : public death::TMLevel
{
	friend class LudumLevelInstance;

	CHAOS_OBJECT_DECLARE_CLASS2(LudumLevel, death::TMLevel);

	LudumLevel();

protected:

	virtual chaos::ParticleLayerBase * DoCreateParticleLayer(death::TMLayerInstance * layer_instance) override;

	virtual death::TMObjectFactory DoGetObjectFactory(death::TMLayerInstance * in_layer_instance, chaos::TiledMap::TypedObject const * in_typed_object) override;
};
