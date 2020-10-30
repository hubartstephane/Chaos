#pragma once

#include <chaos/Chaos.h>

#include "Ludum45Game.h"
#include "Ludum45Enemies.h"

// =================================================
// EnemySpawnerTrigger
// =================================================

class EnemySpawnerTrigger : public chaos::TMTrigger
{
	CHAOS_GAMEPLAY_TM_ALL_FRIENDS;

	CHAOS_DECLARE_OBJECT_CLASS2(EnemySpawnerTrigger, chaos::TMTrigger);

protected:

	/** override */
	virtual bool Initialize(chaos::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object, chaos::TMObjectReferenceSolver & in_reference_solver) override;
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

class BonusSpawnerTrigger : public chaos::TMTrigger
{
	CHAOS_GAMEPLAY_TM_ALL_FRIENDS;

	CHAOS_DECLARE_OBJECT_CLASS2(BonusSpawnerTrigger, chaos::TMTrigger);

protected:

	/** override */
	virtual bool Initialize(chaos::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const * in_geometric_object, chaos::TMObjectReferenceSolver& in_reference_solver) override;
	/** override */
	virtual bool OnCollisionEvent(float delta_time, chaos::Object* object, chaos::CollisionType event_type) override;

protected:

	std::string bonus_name;
	chaos::TagType bonus_type;
};

// =================================================
// Levels
// =================================================

class LudumLevel : public chaos::TMLevel
{
	friend class LudumLevelInstance;

	CHAOS_DECLARE_OBJECT_CLASS2(LudumLevel, chaos::TMLevel);

	LudumLevel();

protected:

	virtual chaos::ParticleLayerBase * DoCreateParticleLayer(chaos::TMLayerInstance * layer_instance) override;

	virtual chaos::TMObjectFactory DoGetObjectFactory(chaos::TMLayerInstance * in_layer_instance, chaos::TiledMap::TypedObject const * in_typed_object) override;
};
