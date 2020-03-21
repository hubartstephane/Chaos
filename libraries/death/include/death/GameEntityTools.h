#pragma once

// XXX : Due to class declaration order and dependencies, this must only be included from .cpp !!
//       usefull for classes using DEATH_GAMEENTITY_IMPLEMENTGETTERS(...)

#include <chaos/StandardHeaders.h>
#include <chaos/AutoCast.h>
#include <chaos/MetaProgramming.h>

#include <death/GameFramework.h>
#include <death/Player.h>
#include <death/PlayerPawn.h>
#include <death/Camera.h>
#include <death/Game.h>
#include <death/GameInstance.h>
#include <death/LevelInstance.h>
#include <death/Level.h>

CHAOS_GENERATE_HAS_MEMBER(level_instance);
CHAOS_GENERATE_HAS_MEMBER(game_instance);
CHAOS_GENERATE_HAS_MEMBER(game);
CHAOS_GENERATE_HAS_MEMBER(level);
CHAOS_GENERATE_HAS_MEMBER(players);

//
//    Player <----> GameInstance <----> Game <----> LevelInstance <----> Camera 
//      ^                                 |              |                 ^
//      |                                 v              |                 |
//      v                               Level <----------+                 v
//   PlayerPawn                                                      CameraComponent

namespace death
{
	class GameEntityTools
	{
	public:


		// =======================================================================
		// Player Getters
		// =======================================================================

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
		// Camera Getters
		// =======================================================================
		
		template<typename T>
		static inline chaos::AutoCastable<Camera> GetCamera(T* src, size_t camera_index)
		{
			assert(src != nullptr);
			LevelInstance* li = GetLevelInstance(src);
			if (li != nullptr)
			{
				if (camera_index >= li->cameras.size())
					return nullptr;
				return chaos::meta::get_raw_pointer(li->cameras[camera_index]);
			}
			return nullptr;
		}
		
		template<typename T>
		static inline chaos::AutoConstCastable<Camera> GetCamera(T const* src, size_t camera_index)
		{
			assert(src != nullptr);
			LevelInstance const* li = GetLevelInstance(src);
			if (li != nullptr)
			{
				if (camera_index >= li->cameras.size())
					return nullptr;
				return chaos::meta::get_raw_pointer(li->cameras[camera_index]);
			}
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

		static inline chaos::AutoConstCastable<Game> GetGame(Player const* src)
		{
			assert(src != nullptr);
			if (src->game_instance != nullptr)
				return GetGame(src->game_instance);
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



























		/** returns the game instance */
		template<typename T>
		static inline chaos::AutoCastable<GameInstance> GetGameInstance(T* src)
		{
			assert(src != nullptr);
			if constexpr (std::is_convertible_v<T, GameInstance>)
				return (GameInstance*)src;

			if constexpr (has_game_instance_v<T>)
				return chaos::meta::get_raw_pointer(src->game_instance);

			return nullptr;
		}
		/** returns the game instance */
		template<typename T>
		static inline chaos::AutoConstCastable<GameInstance> GetGameInstance(T const* src)
		{
			assert(src != nullptr);
			if constexpr (std::is_convertible_v<T, GameInstance const>)
				return (GameInstance const*)src;

			if constexpr (has_game_instance_v<T>)
				return chaos::meta::get_raw_pointer(src->game_instance);

			return nullptr;
		}
















		/** returns the level */
		template<typename T>
		static inline chaos::AutoCastable<Level> GetLevel(T* src)
		{
			assert(src != nullptr);
			if constexpr (std::is_convertible_v<T, Level>)
				return (Level*)src;


			return nullptr;
		}
		/** returns the level */
		template<typename T>
		static inline chaos::AutoConstCastable<Level> GetLevel(T const* src)
		{
			assert(src != nullptr);
			if constexpr (std::is_convertible_v<T, Level const>)
				return (Level const*)src;

			return nullptr;
		}

		/** returns the level instance */
		template<typename T>
		static inline chaos::AutoCastable<LevelInstance> GetLevelInstance(T* src)
		{
			if constexpr (std::is_convertible_v<T, LevelInstance>)
				return (LevelInstance*)src;

			if constexpr (has_level_instance_v<T>)
				return chaos::meta::get_raw_pointer(src->level_instance);





			return nullptr;
		}
		/** returns the level */
		template<typename T>
		static inline chaos::AutoConstCastable<LevelInstance> GetLevelInstance(T const* src)
		{
			if constexpr (std::is_convertible_v<T, LevelInstance const>)
				return (LevelInstance const*)src;

			if constexpr (has_level_instance_v<T>)
				return chaos::meta::get_raw_pointer(src->level_instance);





			return nullptr;
		}
	};


}; // namespace death