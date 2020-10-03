#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/Object.h>
#include <chaos/TiledMap.h>

#include <death/Level.h>
#include <death/TM.h>
#include <death/Game.h>
#include <death/GameFramework.h>

#include "Ludum47Game.h"


// =================================================
// LudumSpeedIndication
// =================================================

class LudumSpeedIndication : public death::TMObject
{
	CHAOS_OBJECT_DECLARE_CLASS2(LudumSpeedIndication, death::TMObject);

protected:

	virtual bool Initialize(death::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const* in_geometric_object, death::TMObjectReferenceSolver& reference_solver);

public:

	float speed_factor = 1.0f;

};

// =================================================
// LudumRoad
// =================================================

class RoadPoint
{
public:

	glm::vec2 position;
	float speed_factor = 1.0f;

};

class LudumRoad : public death::TMObject
{
	CHAOS_OBJECT_DECLARE_CLASS2(LudumRoad, death::TMObject);

protected:

	virtual bool Initialize(death::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const* in_geometric_object, death::TMObjectReferenceSolver& reference_solver);

	virtual void OnLevelStarted() override;

public:

	std::vector<RoadPoint> points;
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

	virtual bool Initialize(chaos::TiledMap::Map* in_tiled_map) override;

public:



};
