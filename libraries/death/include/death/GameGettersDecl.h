#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/AutoCast.h>
#include <death/GameFramework.h>

namespace death
{

#define DEATH_GAMEGETTERS_DECLARE()\
	size_t GetPlayerCount() const;\
	chaos::AutoCastable<Player> GetPlayer(size_t player_index);\
	chaos::AutoConstCastable<Player> GetPlayer(size_t player_index) const;\
	chaos::AutoCastable<PlayerPawn> GetPlayerPawn(size_t player_index);\
	chaos::AutoConstCastable<PlayerPawn> GetPlayerPawn(size_t player_index) const;\
	chaos::AutoCastable<PlayerDisplacementComponent> GetPlayerDisplacementComponent(size_t player_index);\
	chaos::AutoConstCastable<PlayerDisplacementComponent> GetPlayerDisplacementComponent(size_t player_index) const;\
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

#define DEATH_GAMEGETTERS_IMPLEMENT(CLASSNAME)\
	size_t CLASSNAME::GetPlayerCount() const { return GameGettersTools::GetPlayerCount(this);}\
	chaos::AutoCastable<Player> CLASSNAME::GetPlayer(size_t player_index) { return GameGettersTools::GetPlayer(this, player_index);}\
	chaos::AutoConstCastable<Player> CLASSNAME::GetPlayer(size_t player_index) const { return GameGettersTools::GetPlayer(this, player_index); }\
	chaos::AutoCastable<PlayerPawn> CLASSNAME::GetPlayerPawn(size_t player_index) { return GameGettersTools::GetPlayerPawn(this, player_index);}\
	chaos::AutoConstCastable<PlayerPawn> CLASSNAME::GetPlayerPawn(size_t player_index) const { return GameGettersTools::GetPlayerPawn(this, player_index); }\
	chaos::AutoCastable<PlayerDisplacementComponent> CLASSNAME::GetPlayerDisplacementComponent(size_t player_index) { return GameGettersTools::GetPlayerDisplacementComponent(this, player_index);}\
	chaos::AutoConstCastable<PlayerDisplacementComponent> CLASSNAME::GetPlayerDisplacementComponent(size_t player_index) const { return GameGettersTools::GetPlayerDisplacementComponent(this, player_index); }\
	chaos::AutoCastable<Game> CLASSNAME::GetGame() { return GameGettersTools::GetGame(this);}\
	chaos::AutoConstCastable<Game> CLASSNAME::GetGame() const { return GameGettersTools::GetGame(this); }\
	chaos::AutoCastable<GameInstance> CLASSNAME::GetGameInstance() { return GameGettersTools::GetGameInstance(this); }\
	chaos::AutoConstCastable<GameInstance> CLASSNAME::GetGameInstance() const { return GameGettersTools::GetGameInstance(this); }\
	chaos::AutoCastable<LevelInstance> CLASSNAME::GetLevelInstance() { return GameGettersTools::GetLevelInstance(this); }\
	chaos::AutoConstCastable<LevelInstance> CLASSNAME::GetLevelInstance() const { return GameGettersTools::GetLevelInstance(this); }\
	chaos::AutoCastable<Level> CLASSNAME::GetLevel() { return GameGettersTools::GetLevel(this); }\
	chaos::AutoConstCastable<Level> CLASSNAME::GetLevel() const { return GameGettersTools::GetLevel(this); }\
	size_t CLASSNAME::GetCameraCount() const { return GameGettersTools::GetCameraCount(this);}\
	chaos::AutoCastable<Camera> CLASSNAME::GetCamera(size_t camera_index) { return GameGettersTools::GetCamera(this, camera_index); }\
	chaos::AutoConstCastable<Camera> CLASSNAME::GetCamera(size_t camera_index) const { return GameGettersTools::GetCamera(this, camera_index); }

}; // namespace death
