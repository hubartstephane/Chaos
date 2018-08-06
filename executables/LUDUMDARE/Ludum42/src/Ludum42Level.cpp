#include "Ludum42Level.h"


death::GameLevelInstance * LudumLevel::DoCreateLevelInstance()
{
	return new LudumLevelInstance;
}