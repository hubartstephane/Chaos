#pragma once

#include "chaos/Chaos.h"

#include "Ludum43Game.h"

// =================================================
// MyChangeLevelTrigger
// =================================================

class MyChangeLevelTrigger : public chaos::TMTrigger
{

	CHAOS_DECLARE_OBJECT_CLASS(MyChangeLevelTrigger, chaos::TMTrigger);

protected:

	/** override */
	virtual bool Initialize(chaos::TMLayerInstance* in_layer_instance, chaos::TiledMap::GeometricObject const* in_geometric_object, chaos::TMObjectReferenceSolver& in_reference_solver) override;
	/** override */
	virtual bool OnCollisionEvent(float delta_time, chaos::Object* object, chaos::CollisionType event_type) override;
};

// =================================================
// Levels
// =================================================

class LudumLevel : public chaos::TMLevel
{
	friend class LudumLevelInstance;

	CHAOS_DECLARE_OBJECT_CLASS(LudumLevel, chaos::TMLevel);

	LudumLevel();

protected:

	virtual chaos::ParticleLayerBase * DoCreateParticleLayer(chaos::TMLayerInstance * layer_instance) override;

	virtual bool FinalizeLayerParticles(chaos::TMLayerInstance * layer_instance, chaos::ParticleAllocationBase * allocation) override;

	virtual chaos::TMObjectFactory DoGetObjectFactory(chaos::TMLayerInstance * in_layer_instance, chaos::TiledMap::TypedObject const * in_typed_object) override;
};
