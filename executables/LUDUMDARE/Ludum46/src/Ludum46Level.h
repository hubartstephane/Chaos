#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
#include <chaos/TiledMap.h>
#include <chaos/TiledMapTools.h>

#include <death/Level.h>
#include <death/TiledMapLevel.h>
#include <death/Game.h>
#include <death/GameFramework.h>

#include "Ludum46Game.h"



class SpawnerObject : public death::TiledMapGeometricObject
{
	DEATH_TILEDLEVEL_ALL_FRIENDS

public:

	/** constructor */
	using death::TiledMapGeometricObject::TiledMapGeometricObject;

	/** override */
	virtual bool Initialize(chaos::TiledMap::GeometricObject* in_geometric_object) override;

	virtual bool DoTick(float delta_time);

protected:

	int max_spawned_particles = -1;

	glm::vec2 particle_start_velocity = glm::vec2(0.0f, 0.0f);

};

#if 0

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
	virtual bool OnCameraCollisionEvent(float delta_time, chaos::box2 const& camera_box, chaos::CollisionType event_type) override;

protected:

	EnemyPattern* pattern = nullptr;
	EnemyType* type = nullptr;

	std::string enemy_type;
	std::string enemy_pattern;
};

#endif


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

	virtual death::GeometricObjectFactory DoGetGeometricObjectFactory(death::TiledMapLayerInstance * in_layer_instance, chaos::TiledMap::TypedObject* in_typed_object) override;
};
