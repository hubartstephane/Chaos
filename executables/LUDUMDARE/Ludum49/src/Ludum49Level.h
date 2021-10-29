#pragma once

#include "Ludum49.h"
#include "Ludum49Game.h"
#include "Ludum49Particles.h"



// =================================================
// Levels
// =================================================

class LudumLevel : public TMLevel
{
	friend class LudumLevelInstance;

	CHAOS_DECLARE_OBJECT_CLASS(LudumLevel, TMLevel);

	LudumLevel();

protected:

	virtual ParticleLayerBase * DoCreateParticleLayer(TMLayerInstance * layer_instance) override;

	virtual TMObjectFactory DoGetObjectFactory(TMLayerInstance * in_layer_instance, TiledMap::TypedObject const * in_typed_object) override;

	virtual bool Initialize(TiledMap::Map* in_tiled_map) override;

public:



};
