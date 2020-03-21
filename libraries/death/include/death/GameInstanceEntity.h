#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Tickable.h>
#include <chaos/AutoCast.h>
#include <death/GameFramework.h>

namespace death
{

#define DEATH_GAMEENTITY_DECLAREGETTERS()\
	chaos::AutoCastable<Player> inline MyGetPlayer(size_t player_index);\
	chaos::AutoConstCastable<Player> inline MyGetPlayer(size_t player_index) const;\
	chaos::AutoCastable<Game> inline MyGetGame();\
	chaos::AutoConstCastable<Game> inline MyGetGame() const;\
	chaos::AutoCastable<GameInstance> inline MyGetGameInstance();\
	chaos::AutoConstCastable<GameInstance> inline MyGetGameInstance() const;\
	chaos::AutoCastable<LevelInstance> inline MyGetLevelInstance();\
	chaos::AutoConstCastable<LevelInstance> inline MyGetLevelInstance() const;\
	chaos::AutoCastable<Level> inline MyGetLevel();\
	chaos::AutoConstCastable<Level> inline MyGetLevel() const;\
	chaos::AutoCastable<Camera> inline MyGetCamera(size_t camera_index);\
	chaos::AutoConstCastable<Camera> inline MyGetCamera(size_t camera_index) const;

#define DEATH_GAMEENTITY_IMPLEMENTGETTERS(CLASSNAME)\
	chaos::AutoCastable<Player> CLASSNAME::MyGetPlayer(size_t player_index) { return GameEntityTools::GetPlayer(this, player_index);}\
	chaos::AutoConstCastable<Player> CLASSNAME::MyGetPlayer(size_t player_index) const { return GameEntityTools::GetPlayer(this, player_index); }\
	chaos::AutoCastable<Game> CLASSNAME::MyGetGame() { return GameEntityTools::GetGame(this);}\
	chaos::AutoConstCastable<Game> CLASSNAME::MyGetGame() const { return GameEntityTools::GetGame(this); }\
	chaos::AutoCastable<GameInstance> CLASSNAME::MyGetGameInstance() { return GameEntityTools::GetGameInstance(this); }\
	chaos::AutoConstCastable<GameInstance> CLASSNAME::MyGetGameInstance() const { return GameEntityTools::GetGameInstance(this); }\
	chaos::AutoCastable<LevelInstance> CLASSNAME::MyGetLevelInstance() { return GameEntityTools::GetLevelInstance(this); }\
	chaos::AutoConstCastable<LevelInstance> CLASSNAME::MyGetLevelInstance() const { return GameEntityTools::GetLevelInstance(this); }\
	chaos::AutoCastable<Level> CLASSNAME::MyGetLevel() { return GameEntityTools::GetLevel(this); }\
	chaos::AutoConstCastable<Level> CLASSNAME::MyGetLevel() const { return GameEntityTools::GetLevel(this); }\
	chaos::AutoCastable<Camera> CLASSNAME::MyGetCamera(size_t camera_index) { return GameEntityTools::GetCamera(this, camera_index); }\
	chaos::AutoConstCastable<Camera> CLASSNAME::MyGetCamera(size_t camera_index) const { return GameEntityTools::GetCamera(this, camera_index); }

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
