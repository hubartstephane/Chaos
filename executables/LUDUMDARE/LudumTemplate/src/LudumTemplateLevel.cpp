#include "LudumTemplateLevel.h"


death::GameLevelInstance * LudumLevel::DoCreateLevelInstance()
{
	return new LudumLevelInstance;
}