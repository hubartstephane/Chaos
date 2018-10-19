#include <death/GameLevel.h>

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
		result->level = this;
		if (!result->Initialize(in_game)) // additional initialization
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

	// =====================================
	// GameLevel implementation
	// =====================================

	void GameLevelInstance::OnLevelEnded()
	{
	}
	
	void GameLevelInstance::OnLevelStarted()
	{
	}

	bool GameLevelInstance::Initialize(Game * in_game)
	{
		return true;
	}

}; // namespace death

