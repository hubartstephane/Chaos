#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Tickable.h>
#include <chaos/AutoCast.h>
#include <death/GameFramework.h>

namespace death
{

#define DEATH_GAMEENTITY_IMPLEMENT()\
	chaos::AutoCastable<Player> MyGetPlayer(size_t player_index) { return GameEntity::GetPlayer(this, player_index);}\
	chaos::AutoConstCastable<Player> MyGetPlayer(size_t player_index) const { return GameEntity::GetPlayer(this, player_index); }\
	chaos::AutoCastable<Game> MyGetGame() { return GameEntity::GetGame(this);}\
	chaos::AutoConstCastable<Game> MyGetGame() const { return GameEntity::GetGame(this); }\
	chaos::AutoCastable<GameInstance> MyGetGameInstance() { return GameEntity::GetGameInstance(this); }\
	chaos::AutoConstCastable<GameInstance> MyGetGameInstance() const { return GameEntity::GetGameInstance(this); }\
	chaos::AutoCastable<LevelInstance> MyGetLevelInstance() { return GameEntity::GetLevelInstance(this); }\
	chaos::AutoConstCastable<LevelInstance> MyGetLevelInstance() const { return GameEntity::GetLevelInstance(this); }\
	chaos::AutoCastable<Level> MyGetLevel() { return GameEntity::GetLevel(this); }\
	chaos::AutoConstCastable<Level> MyGetLevel() const { return GameEntity::GetLevel(this); }

	class GameEntity
	{
	public:

		/** returns the player */
		template<typename T>
		static chaos::AutoCastable<Player> GetPlayer(T* src, size_t player_index)
		{
			return nullptr;
		}
		/** returns the player */
		template<typename T>
		static chaos::AutoConstCastable<Player> GetPlayer(T const* src, size_t player_index)
		{
			return nullptr;
		}

		/** returns the game */
		template<typename T>
		static chaos::AutoCastable<Game> GetGame(T* src)
		{
			return nullptr;
		}
		/** returns the game */
		template<typename T>
		static chaos::AutoConstCastable<Game> GetGame(T const* src)
		{
			return nullptr;
		}

		/** returns the game instance */
		template<typename T>
		static chaos::AutoCastable<GameInstance> GetGameInstance(T* src)
		{
			return nullptr;
		}
		/** returns the game instance */
		template<typename T>
		static chaos::AutoConstCastable<GameInstance> GetGameInstance(T const* src)
		{
			return nullptr;
		}

		/** returns the level */
		template<typename T>
		static chaos::AutoCastable<Level> GetLevel(T* src)
		{
			return nullptr;
		}
		/** returns the level */
		template<typename T>
		static chaos::AutoConstCastable<Level> GetLevel(T const* src)
		{
			return nullptr;
		}

		/** returns the level instance */
		template<typename T>
		static chaos::AutoCastable<LevelInstance> GetLevelInstance(T* src)
		{
			return nullptr;
		}
		/** returns the level */
		template<typename T>
		static chaos::AutoConstCastable<LevelInstance> GetLevelInstance(T const* src)
		{
			return nullptr;
		}
	};


	// =============================================
	// GameInstanceEntity
	// =============================================

	class GameInstanceEntity
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

	public:

		/** constructor */
		GameInstanceEntity(GameInstance * in_game_instance);
		/** destructor */
		virtual ~GameInstanceEntity() = default;

		/** returns the game */
		chaos::AutoCastable<Game> GetGame();
		/** returns the game */
		chaos::AutoConstCastable<Game> GetGame() const;

		/** returns the game instance */
		chaos::AutoCastable<GameInstance> GetGameInstance() { return game_instance; }
		/** returns the game instance */
		chaos::AutoConstCastable<GameInstance> GetGameInstance() const { return game_instance; }

		/** returns the level */
		chaos::AutoCastable<Level> GetLevel();
		/** returns the level */
		chaos::AutoConstCastable<Level> GetLevel() const;

		/** returns the level instance */
		chaos::AutoCastable<LevelInstance> GetLevelInstance();
		/** returns the level */
		chaos::AutoConstCastable<LevelInstance> GetLevelInstance() const;

	protected:

		/** initialize the game instance */
		virtual bool Initialize(death::GameInstance * in_game_instance);

	protected:

		/** the game instance owning the player */
		GameInstance * game_instance = nullptr;
	};

}; // namespace death
