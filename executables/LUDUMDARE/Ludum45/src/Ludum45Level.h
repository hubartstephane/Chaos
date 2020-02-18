#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
#include <chaos/TiledMap.h>
#include <chaos/TiledMapTools.h>

#include <death/GameLevel.h>
#include <death/TiledMapLevel.h>
#include <death/Game.h>
#include <death/GameFramework.h>

#include "Ludum45Game.h"
#include "Ludum45Enemies.h"

// =================================================
// EnemySpawnerTriggerObject
// =================================================

class EnemySpawnerTriggerObject : public death::TiledMap::TriggerObject
{
	DEATH_TILEDLEVEL_ALL_FRIENDS

public:

	/** constructor */
	using TriggerObject::TriggerObject;
	/** override */
	virtual bool Initialize() override;

protected:

	/** override */
	virtual bool OnCameraCollisionEvent(float delta_time, chaos::box2 const & camera_box, int event_type) override;

protected:

	EnemyPattern * pattern = nullptr;
	EnemyType * type = nullptr;

	std::string enemy_type;
	std::string enemy_pattern;
};

// =================================================
// BonusSpawnerTriggerObject
// =================================================

class BonusSpawnerTriggerObject : public death::TiledMap::TriggerObject
{
	DEATH_TILEDLEVEL_ALL_FRIENDS

public:

	/** constructor */
	using TriggerObject::TriggerObject;
	/** override */
	virtual bool Initialize() override;

protected:

	/** override */
	virtual bool OnCameraCollisionEvent(float delta_time, chaos::box2 const & camera_box, int event_type) override;

protected:

	std::string bonus_name;
	chaos::TagType bonus_type;
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

	virtual bool OnPlayerTileCollision(float delta_time, class death::Player * player, chaos::ParticleDefault::Particle * player_particle, death::TiledMap::TileParticle * particle) override;
};
