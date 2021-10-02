#pragma once

#include <chaos/Chaos.h>

#include "Ludum49Game.h"
#include "Ludum49Particles.h"



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

	virtual bool Initialize(chaos::TiledMap::Map* in_tiled_map) override;

public:



};
