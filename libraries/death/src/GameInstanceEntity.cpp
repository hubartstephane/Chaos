#include <death/GameInstanceEntity.h>
#include <death/Game.h>

namespace death
{


	/** returns the player */
	template<typename T>
	static chaos::AutoCastable<Player> GameEntity::GetPlayer(T* src, size_t player_index)
	{


		return nullptr;
	}
	/** returns the player */
	template<typename T>
	static chaos::AutoConstCastable<Player> GameEntity::GetPlayer(T const* src, size_t player_index)
	{
		return nullptr;
	}

	/** returns the game */
	template<typename T>
	static chaos::AutoCastable<Game> GameEntity::GetGame(T* src)
	{
		if constexpr (std::is_convertible_v<T, Game>)
			return (Game*)src;


		return nullptr;
	}
	/** returns the game */
	template<typename T>
	static chaos::AutoConstCastable<Game> GameEntity::GetGame(T const* src)
	{
		if constexpr (std::is_convertible_v<T, Game const>)
			return (Game const*)src;

		return nullptr;
	}

	/** returns the game instance */
	template<typename T>
	static chaos::AutoCastable<GameInstance> GameEntity::GetGameInstance(T* src)
	{
		if constexpr (std::is_convertible_v<T, GameInstance>)
			return (GameInstance*)src;

		return nullptr;
	}
	/** returns the game instance */
	template<typename T>
	static chaos::AutoConstCastable<GameInstance> GameEntity::GetGameInstance(T const* src)
	{
		if constexpr (std::is_convertible_v<T, GameInstance const>)
			return (GameInstance const*)src;

		return nullptr;
	}

	/** returns the level */
	template<typename T>
	static chaos::AutoCastable<Level> GameEntity::GetLevel(T* src)
	{
		if constexpr (std::is_convertible_v<T, Level>)
			return (Level*)src;

		return nullptr;
	}
	/** returns the level */
	template<typename T>
	static chaos::AutoConstCastable<Level> GameEntity::GetLevel(T const* src)
	{
		if constexpr (std::is_convertible_v<T, Level const>)
			return (Level const*)src;

		return nullptr;
	}

	/** returns the level instance */
	template<typename T>
	static chaos::AutoCastable<LevelInstance> GameEntity::GetLevelInstance(T* src)
	{
		if constexpr (std::is_convertible_v<T, LevelInstance>)
			return (LevelInstance*)src;

		return nullptr;
	}
	/** returns the level */
	template<typename T>
	static chaos::AutoConstCastable<LevelInstance> GameEntity::GetLevelInstance(T const* src)
	{
		if constexpr (std::is_convertible_v<T, LevelInstance const>)
			return (LevelInstance const*)src;

		return nullptr;
	}


	
















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
			return nullptr;
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
			return nullptr;
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
			return nullptr;
		return game->GetLevelInstance();
	}

	bool GameInstanceEntity::Initialize(death::GameInstance * in_game_instance)
	{
		return true;
	}

}; // namespace death
