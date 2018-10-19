#include <death/GameLevel.h>

namespace death
{
	// =====================================
	// GameLevelInstance implementation
	// =====================================

	GameLevelInstance * GameLevel::CreateLevelInstance(Game * in_game)
	{
		GameLevelInstance * result = DoCreateLevelInstance(in_game);
		if (result == nullptr)
			return nullptr;
		result->level = this;
		return result;
	}

	GameLevelInstance * GameLevel::DoCreateLevelInstance(Game * in_game)
	{
		return new GameLevelInstance;
	}

	// =====================================
	// GameLevel implementation
	// =====================================

	void GameLevelInstance::OnLevelEnded()
	{
	}
	
	void GameLevelInstance::OnLevelStarted()
	{
	}

}; // namespace death

