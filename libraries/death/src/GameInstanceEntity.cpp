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

	chaos::AutoCastable<Game> GameInstanceEntity::GetGame()
	{
		if (game_instance == nullptr)
			return nullptr;
		return game_instance->GetGame();
	}

	chaos::AutoConstCastable<Game> GameInstanceEntity::GetGame() const
	{
		if (game_instance == nullptr)
			return (Game const *)nullptr;
		return game_instance->GetGame();
	}

	chaos::AutoCastable<Level> GameInstanceEntity::GetLevel()
	{
		Game * game = GetGame();
		if (game == nullptr)
			return nullptr;
		return game->GetLevel();
	}
	
	chaos::AutoConstCastable<Level> GameInstanceEntity::GetLevel() const
	{
		Game const * game = GetGame();
		if (game == nullptr)
			return (Level const *)nullptr;
		return game->GetLevel();
	}
	
	chaos::AutoCastable<LevelInstance> GameInstanceEntity::GetLevelInstance()
	{
		Game * game = GetGame();
		if (game == nullptr)
			return nullptr;
		return game->GetLevelInstance();
	}
	
	chaos::AutoConstCastable<LevelInstance> GameInstanceEntity::GetLevelInstance() const
	{
		Game const * game = GetGame();
		if (game == nullptr)
			return (LevelInstance const *)nullptr;
		return game->GetLevelInstance();
	}

	bool GameInstanceEntity::Initialize(death::GameInstance * in_game_instance)
	{
		return true;
	}

}; // namespace death
