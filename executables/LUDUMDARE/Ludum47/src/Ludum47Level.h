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
