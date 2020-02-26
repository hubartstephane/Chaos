#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Tickable.h>
#include <chaos/AutoCast.h>
#include <death/GameFramework.h>

namespace death
{

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
