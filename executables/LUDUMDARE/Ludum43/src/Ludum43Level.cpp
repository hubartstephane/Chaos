#include "Ludum43Level.h"
#include "Ludum43Game.h"

#include <chaos/GLMTools.h>
#include <chaos/ParticleDefault.h>

// =============================================================
// AtomObject implementation
// =============================================================

AtomGameObject::AtomGameObject(death::TiledMap::LayerInstance * in_layer_instance) :
	death::TiledMap::GeometricObject(in_layer_instance)
{
}

// =============================================================
// LudumLevel implementation
// =============================================================

LudumLevel::LudumLevel()
{

}

death::GameLevelInstance * LudumLevel::DoCreateLevelInstance(death::Game * in_game)
{
	return new LudumLevelInstance(dynamic_cast<LudumGame *>(in_game));
}

death::TiledMap::GeometricObject * LudumLevel::DoCreateTypedObject(death::TiledMap::LayerInstance * in_layer_instance)
{
	

	return nullptr;
}

// =============================================================
// LudumLevelInstance implementation
// =============================================================

LudumLevelInstance::LudumLevelInstance(LudumGame * in_game):
	game(in_game)
{
	assert(in_game != nullptr); 
}

LudumLevel * LudumLevelInstance::GetTypedLevel()
{
	return dynamic_cast<LudumLevel*>(GetLevel());
}

LudumLevel const * LudumLevelInstance::GetTypedLevel() const
{
	return dynamic_cast<LudumLevel const *>(GetLevel());
}
