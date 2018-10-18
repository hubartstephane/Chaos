#include <death/GameLevel.h>

namespace death
{
	// =====================================
	// GameLevelInstance implementation
	// =====================================

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

