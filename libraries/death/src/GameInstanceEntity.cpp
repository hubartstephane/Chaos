#include <death/GameInstanceEntity.h>
#include <death/Game.h>

namespace death
{

	// =================================================
	// GameInstanceEntity
	// =================================================

	GameInstanceEntity::GameInstanceEntity(GameInstance * in_game_instance) :
		game_instance(in_game_instance)
	{
		//assert(in_game_instance != nullptr);
	}

	Game * GameInstanceEntity::GetGame()
	{
		if (game_instance == nullptr)
			return nullptr;
		return game_instance->GetGame();
	}

	Game const * GameInstanceEntity::GetGame() const
	{
		if (game_instance == nullptr)
			return nullptr;
		return game_instance->GetGame();
	}

	Level * GameInstanceEntity::GetLevel()
	{
		Game * game = GetGame();
		if (game == nullptr)
			return nullptr;
		return game->GetLevel();
	}
	
	Level const * GameInstanceEntity::GetLevel() const
	{
		Game const * game = GetGame();
		if (game == nullptr)
			return nullptr;
		return game->GetLevel();
	}
	
	LevelInstance * GameInstanceEntity::GetLevelInstance()
	{
		Game * game = GetGame();
		if (game == nullptr)
			return nullptr;
		return game->GetLevelInstance();
	}
	
	LevelInstance const * GameInstanceEntity::GetLevelInstance() const
	{
		Game const * game = GetGame();
		if (game == nullptr)
			return nullptr;
		return game->GetLevelInstance();
	}

	bool GameInstanceEntity::Initialize(death::GameInstance * in_game_instance)
	{
		return true;
	}

}; // namespace death
