#include "Ludum43Level.h"
#include "Ludum43Game.h"

#include <chaos/GLMTools.h>
#include <chaos/ParticleDefault.h>

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
