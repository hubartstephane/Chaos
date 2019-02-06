#include <death/GameLevel.h>
#include <death/Game.h>
#include <death/GameLevelInstance.h>

namespace death
{
	// =====================================
	// GameLevelInstance implementation
	// =====================================

	GameLevelInstance * GameLevel::CreateLevelInstance(Game * in_game)
	{
		GameLevelInstance * result = DoCreateLevelInstance(in_game); // create the instance
		if (result == nullptr)
			return nullptr;
		if (!result->Initialize(in_game, this)) // additional initialization
		{
			delete(result);
			return nullptr;
		}
		return result;
	}

	GameLevelInstance * GameLevel::DoCreateLevelInstance(Game * in_game)
	{
		return new GameLevelInstance;
	}

}; // namespace death

