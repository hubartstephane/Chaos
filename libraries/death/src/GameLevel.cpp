#include <death/GameLevel.h>

namespace death
{
	GameLevelInstance * GameLevel::CreateLevelInstance()
	{
		GameLevelInstance * result = DoCreateLevelInstance();
		if (result == nullptr)
			return nullptr;
		result->level = this;
		return result;
	}

	GameLevelInstance * GameLevel::DoCreateLevelInstance()
	{
		return new GameLevelInstance;
	}

}; // namespace death

