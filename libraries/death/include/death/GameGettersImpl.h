#pragma once

// XXX : Due to class declaration order and dependencies, this must only be included from .cpp !!
//       usefull for classes using DEATH_GAMEGETTERS_IMPLEMENT(...)

#include <chaos/StandardHeaders.h>
#include <chaos/AutoCast.h>
#include <chaos/MetaProgramming.h>

#include <death/GameFramework.h>
#include <death/Player.h>
#include <death/PlayerPawn.h>
#include <death/PlayerDisplacementComponent.h>
#include <death/Camera.h>
#include <death/Game.h>
#include <death/GameInstance.h>
#include <death/LevelInstance.h>
#include <death/Level.h>


//  PlayerDisplacementComponent
//      ^
//      |
//      v
//    Player <----> GameInstance <----> Game <----> LevelInstance <----> Camera 
//      ^                                                |                 ^
//      |                                                |                 |
//      v                                                v                 v
//   PlayerPawn                                        Level         CameraComponent

namespace death
{
	class GameGettersTools
	{
	public:


		// =======================================================================
		// Player Getters
		// =======================================================================

		template<typename T>
		static inline size_t GetPlayerCount(T const * src)
		{
			GameInstance const* gi = GetGameInstance(src);
			if (gi != nullptr)
				return gi->players.size();
			return 0;
		}

		template<typename T>
		static inline chaos::AutoCastable<Player> GetPlayer(T* src, size_t player_index)
		{
			assert(src != nullptr);
			GameInstance* gi = GetGameInstance(src);
			if (gi != nullptr)
			{
				if (player_index >= gi->players.size())
					return nullptr;
				return chaos::meta::get_raw_pointer(gi->players[player_index]);
			}
			return nullptr;
		}

		template<typename T>
		static inline chaos::AutoConstCastable<Player> GetPlayer(T const* src, size_t player_index)
		{
			assert(src != nullptr);
			GameInstance const* gi = GetGameInstance(src);
			if (gi != nullptr)
			{
				if (player_index >= gi->players.size())
					return nullptr;
				return chaos::meta::get_raw_pointer(gi->players[player_index]);
			}
			return nullptr;
		}

		// =======================================================================
		// PlayerPawn Getters
		// =======================================================================

		template<typename T>
		static inline chaos::AutoCastable<PlayerPawn> GetPlayerPawn(T* src, size_t player_index)
		{
			assert(src != nullptr);
			Player * pl = GetPlayer(src, player_index);
			if (pl != nullptr)
				return chaos::meta::get_raw_pointer(pl->pawn);
			return nullptr;
		}

		template<typename T>
		static inline chaos::AutoConstCastable<PlayerPawn> GetPlayerPawn(T const* src, size_t player_index)
		{
			assert(src != nullptr);
			Player const * pl = GetPlayer(src, player_index);
			if (pl != nullptr)
				return chaos::meta::get_raw_pointer(pl->pawn);
			return nullptr;
		}

		// =======================================================================
		// Camera Getters
		// =======================================================================
		
		template<typename T>
		static inline size_t GetCameraCount(T const* src)
		{
			LevelInstance const* li = GetLevelInstance(src);
			if (li != nullptr)
				return li->cameras.size();
			return 0;
		}

		template<typename T>
		static inline chaos::AutoCastable<Camera> GetCamera(T* src, size_t camera_index)
		{
			assert(src != nullptr);
			LevelInstance* li = GetLevelInstance(src);
			if (li != nullptr)
				return li->DoGetCamera(camera_index, true);  // accept free camera
			return nullptr;
		}
		
		template<typename T>
		static inline chaos::AutoConstCastable<Camera> GetCamera(T const* src, size_t camera_index)
		{
			assert(src != nullptr);
			LevelInstance const* li = GetLevelInstance(src);
			if (li != nullptr)
				return li->DoGetCamera(camera_index, true); // accept free camera
			return nullptr;
		}


		// =======================================================================
		// Game Getters
		// =======================================================================

		static inline chaos::AutoCastable<Game> GetGame(Player * src)
		{
			assert(src != nullptr);
			if (src->game_instance != nullptr)
				return GetGame(src->game_instance);
			return nullptr;
		}

		static inline chaos::AutoCastable<Game> GetGame(PlayerPawn* src)
		{
			assert(src != nullptr);
			if (src->player != nullptr)
				return GetGame(src->player);
			return nullptr;
		}

		static inline chaos::AutoCastable<Game> GetGame(GameInstance * src)
		{
			assert(src != nullptr);
			return chaos::meta::get_raw_pointer(src->game);
		}

		static inline chaos::AutoCastable<Game> GetGame(Game * src)
		{
			assert(src != nullptr);
			return src;
		}

		static inline chaos::AutoCastable<Game> GetGame(LevelInstance * src)
		{
			assert(src != nullptr);
			return chaos::meta::get_raw_pointer(src->game);
		}

		static inline chaos::AutoCastable<Game> GetGame(Camera * src)
		{
			assert(src != nullptr);
			if (src->level_instance != nullptr)
				return GetGame(src->level_instance);
			return nullptr;
		}

		static inline chaos::AutoCastable<Game> GetGame(CameraComponent * src)
		{
			assert(src != nullptr);
			if (src->camera != nullptr)
				return GetGame(src->camera);
			return nullptr;
		}

		static inline chaos::AutoConstCastable<Game> GetGame(Player const* src)
		{
			assert(src != nullptr);
			if (src->game_instance != nullptr)
				return GetGame(src->game_instance);
			return nullptr;
		}

		static inline chaos::AutoCastable<Game> GetGame(PlayerPawn const * src)
		{
			assert(src != nullptr);
			if (src->player != nullptr)
				return GetGame(src->player);
			return nullptr;
		}

		static inline chaos::AutoConstCastable<Game> GetGame(GameInstance const* src)
		{
			assert(src != nullptr);
			return chaos::meta::get_raw_pointer(src->game);
		}

		static inline chaos::AutoConstCastable<Game> GetGame(Game const* src)
		{
			assert(src != nullptr);
			return src;
		}

		static inline chaos::AutoConstCastable<Game> GetGame(LevelInstance const* src)
		{
			assert(src != nullptr);
			return chaos::meta::get_raw_pointer(src->game);
		}

		static inline chaos::AutoConstCastable<Game> GetGame(Camera const* src)
		{
			assert(src != nullptr);
			if (src->level_instance != nullptr)
				return GetGame(src->level_instance);
			return nullptr;
		}

		static inline chaos::AutoConstCastable<Game> GetGame(CameraComponent const* src)
		{
			assert(src != nullptr);
			if (src->camera != nullptr)
				return GetGame(src->camera);
			return nullptr;
		}

		// =======================================================================
		// GameInstance Getters
		// =======================================================================

		static inline chaos::AutoCastable<GameInstance> GetGameInstance(Player* src)
		{
			assert(src != nullptr);
			return chaos::meta::get_raw_pointer(src->game_instance);
		}

		static inline chaos::AutoCastable<GameInstance> GetGameInstance(PlayerPawn* src)
		{
			assert(src != nullptr);
			if (src->player != nullptr)
				return GetGameInstance(src->player);
			return nullptr;
		}

		static inline chaos::AutoCastable<GameInstance> GetGameInstance(GameInstance* src)
		{
			assert(src != nullptr);
			return src;
		}

		static inline chaos::AutoCastable<GameInstance> GetGameInstance(Game* src)
		{
			assert(src != nullptr);
			return chaos::meta::get_raw_pointer(src->game_instance);
		}

		static inline chaos::AutoCastable<GameInstance> GetGameInstance(LevelInstance* src)
		{
			assert(src != nullptr);
			if (src->game != nullptr)
				return GetGameInstance(src->game);
			return nullptr;
		}

		static inline chaos::AutoCastable<GameInstance> GetGameInstance(Camera* src)
		{
			assert(src != nullptr);
			if (src->level_instance != nullptr)
				return GetGameInstance(src->level_instance);
			return nullptr;
		}

		static inline chaos::AutoCastable<GameInstance> GetGameInstance(CameraComponent * src)
		{
			assert(src != nullptr);
			if (src->camera != nullptr)
				return GetGameInstance(src->camera);
			return nullptr;
		}

		static inline chaos::AutoConstCastable<GameInstance> GetGameInstance(Player const* src)
		{
			assert(src != nullptr);
			return chaos::meta::get_raw_pointer(src->game_instance);
		}

		static inline chaos::AutoCastable<GameInstance> GetGameInstance(PlayerPawn const* src)
		{
			assert(src != nullptr);
			if (src->player != nullptr)
				return GetGameInstance(src->player);
			return nullptr;
		}

		static inline chaos::AutoConstCastable<GameInstance> GetGameInstance(GameInstance const* src)
		{
			assert(src != nullptr);
			return src;
		}

		static inline chaos::AutoConstCastable<GameInstance> GetGameInstance(Game const* src)
		{
			assert(src != nullptr);
			return chaos::meta::get_raw_pointer(src->game_instance);
		}

		static inline chaos::AutoConstCastable<GameInstance> GetGameInstance(LevelInstance const* src)
		{
			assert(src != nullptr);
			if (src->game != nullptr)
				return GetGameInstance(src->game);
			return nullptr;
		}

		static inline chaos::AutoConstCastable<GameInstance> GetGameInstance(Camera const* src)
		{
			assert(src != nullptr);
			if (src->level_instance != nullptr)
				return GetGameInstance(src->level_instance);
			return nullptr;
		}

		static inline chaos::AutoConstCastable<GameInstance> GetGameInstance(CameraComponent const * src)
		{
			assert(src != nullptr);
			if (src->camera != nullptr)
				return GetGameInstance(src->camera);
			return nullptr;
		}

		// =======================================================================
		// LevelInstance Getters
		// =======================================================================

		static inline chaos::AutoCastable<LevelInstance> GetLevelInstance(Player* src)
		{
			assert(src != nullptr);
			if (src->game_instance != nullptr)
				return GetLevelInstance(src->game_instance);
			return nullptr;
		}

		static inline chaos::AutoCastable<LevelInstance> GetLevelInstance(PlayerPawn* src)
		{
			assert(src != nullptr);
			if (src->player != nullptr)
				return GetLevelInstance(src->player);
			return nullptr;
		}

		static inline chaos::AutoCastable<LevelInstance> GetLevelInstance(GameInstance* src)
		{
			assert(src != nullptr);
			if (src->game != nullptr)
				return GetLevelInstance(src->game);
			return nullptr;
		}

		static inline chaos::AutoCastable<LevelInstance> GetLevelInstance(Game* src)
		{
			assert(src != nullptr);
			return chaos::meta::get_raw_pointer(src->level_instance);
		}

		static inline chaos::AutoCastable<LevelInstance> GetLevelInstance(LevelInstance* src)
		{
			assert(src != nullptr);
			return src;
		}

		static inline chaos::AutoCastable<LevelInstance> GetLevelInstance(Camera* src)
		{
			assert(src != nullptr);
			return chaos::meta::get_raw_pointer(src->level_instance);
		}

		static inline chaos::AutoCastable<LevelInstance> GetLevelInstance(CameraComponent* src)
		{
			assert(src != nullptr);
			if (src->camera != nullptr)
				return GetLevelInstance(src->camera);
			return nullptr;
		}

		static inline chaos::AutoConstCastable<LevelInstance> GetLevelInstance(Player const* src)
		{
			assert(src != nullptr);
			if (src->game_instance != nullptr)
				return GetLevelInstance(src->game_instance);
			return nullptr;
		}

		static inline chaos::AutoCastable<LevelInstance> GetLevelInstance(PlayerPawn const* src)
		{
			assert(src != nullptr);
			if (src->player != nullptr)
				return GetLevelInstance(src->player);
			return nullptr;
		}

		static inline chaos::AutoConstCastable<LevelInstance> GetLevelInstance(GameInstance const* src)
		{
			assert(src != nullptr);
			if (src->game != nullptr)
				return GetLevelInstance(src->game);
			return nullptr;
		}

		static inline chaos::AutoConstCastable<LevelInstance> GetLevelInstance(Game const* src)
		{
			assert(src != nullptr);
			return chaos::meta::get_raw_pointer(src->level_instance);
		}

		static inline chaos::AutoConstCastable<LevelInstance> GetLevelInstance(LevelInstance const* src)
		{
			assert(src != nullptr);
			return src;
		}

		static inline chaos::AutoConstCastable<LevelInstance> GetLevelInstance(Camera const* src)
		{
			assert(src != nullptr);
			return chaos::meta::get_raw_pointer(src->level_instance);
		}

		static inline chaos::AutoConstCastable<LevelInstance> GetLevelInstance(CameraComponent const * src)
		{
			assert(src != nullptr);
			if (src->camera != nullptr)
				return GetLevelInstance(src->camera);
			return nullptr;
		}

		// =======================================================================
		// Level Getters
		// =======================================================================

		template<typename T>
		static inline chaos::AutoCastable<Level> GetLevel(T* src)
		{
			assert(src != nullptr);
			LevelInstance* li = GetLevelInstance(src);
			if (li != nullptr)
				return chaos::meta::get_raw_pointer(li->level);
			return nullptr;
		}

		template<typename T>
		static inline chaos::AutoConstCastable<Level> GetLevel(T const* src)
		{
			assert(src != nullptr);
			LevelInstance const * li = GetLevelInstance(src);
			if (li != nullptr)
				return chaos::meta::get_raw_pointer(li->level);
			return nullptr;
		}

	};


}; // namespace death
