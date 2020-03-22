#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/AutoCast.h>
#include <death/GameFramework.h>

namespace death
{

#define DEATH_GAMEENTITY_DECLAREGETTERS()\
	size_t GetPlayerCount() const;\
	chaos::AutoCastable<Player> GetPlayer(size_t player_index);\
	chaos::AutoConstCastable<Player> GetPlayer(size_t player_index) const;\
	chaos::AutoCastable<Game> GetGame();\
	chaos::AutoConstCastable<Game> GetGame() const;\
	chaos::AutoCastable<GameInstance> GetGameInstance();\
	chaos::AutoConstCastable<GameInstance> GetGameInstance() const;\
	chaos::AutoCastable<LevelInstance> GetLevelInstance();\
	chaos::AutoConstCastable<LevelInstance> GetLevelInstance() const;\
	chaos::AutoCastable<Level> GetLevel();\
	chaos::AutoConstCastable<Level> GetLevel() const;\
	size_t GetCameraCount() const;\
	chaos::AutoCastable<Camera> GetCamera(size_t camera_index);\
	chaos::AutoConstCastable<Camera> GetCamera(size_t camera_index) const;

#define DEATH_GAMEENTITY_IMPLEMENTGETTERS(CLASSNAME)\
	size_t CLASSNAME::GetPlayerCount() const { return GameEntityTools::GetPlayerCount(this);}\
	chaos::AutoCastable<Player> CLASSNAME::GetPlayer(size_t player_index) { return GameEntityTools::GetPlayer(this, player_index);}\
	chaos::AutoConstCastable<Player> CLASSNAME::GetPlayer(size_t player_index) const { return GameEntityTools::GetPlayer(this, player_index); }\
	chaos::AutoCastable<Game> CLASSNAME::GetGame() { return GameEntityTools::GetGame(this);}\
	chaos::AutoConstCastable<Game> CLASSNAME::GetGame() const { return GameEntityTools::GetGame(this); }\
	chaos::AutoCastable<GameInstance> CLASSNAME::GetGameInstance() { return GameEntityTools::GetGameInstance(this); }\
	chaos::AutoConstCastable<GameInstance> CLASSNAME::GetGameInstance() const { return GameEntityTools::GetGameInstance(this); }\
	chaos::AutoCastable<LevelInstance> CLASSNAME::GetLevelInstance() { return GameEntityTools::GetLevelInstance(this); }\
	chaos::AutoConstCastable<LevelInstance> CLASSNAME::GetLevelInstance() const { return GameEntityTools::GetLevelInstance(this); }\
	chaos::AutoCastable<Level> CLASSNAME::GetLevel() { return GameEntityTools::GetLevel(this); }\
	chaos::AutoConstCastable<Level> CLASSNAME::GetLevel() const { return GameEntityTools::GetLevel(this); }\
	size_t CLASSNAME::GetCameraCount() const { return GameEntityTools::GetCameraCount(this);}\
	chaos::AutoCastable<Camera> CLASSNAME::GetCamera(size_t camera_index) { return GameEntityTools::GetCamera(this, camera_index); }\
	chaos::AutoConstCastable<Camera> CLASSNAME::GetCamera(size_t camera_index) const { return GameEntityTools::GetCamera(this, camera_index); }

}; // namespace death
