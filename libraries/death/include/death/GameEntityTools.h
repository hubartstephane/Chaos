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
CHAOS_GENERATE_HAS_MEMBER(players);

namespace death
{
	class GameEntityTools
	{
	public:

		/** returns the player */
		template<typename T>
		static chaos::AutoCastable<Player> GetPlayer(T* src, size_t player_index)
		{

			return nullptr;

#if 0
			GameInstance* gi = GetGameInstance(stc);
			if (gi == nullptr)
				return nullptr;
			return gi->GetPlayer(player_index);
#endif
		}
		/** returns the player */
		template<typename T>
		static chaos::AutoConstCastable<Player> GetPlayer(T const* src, size_t player_index)
		{
			return nullptr;

#if 0
			GameInstance const* gi = GetGameInstance(stc);
			if (gi == nullptr)
				return nullptr;
			return gi->GetPlayer(player_index);
#endif
		}

		/** returns the game */
		template<typename T>
		static chaos::AutoCastable<Game> GetGame(T* src)
		{
			if constexpr (std::is_convertible_v<T, Game>)
				return (Game*)src;

			if constexpr (has_game_v<T>)
				return chaos::meta::get_raw_pointer(src->game);



			return nullptr;
		}
		/** returns the game */
		template<typename T>
		static chaos::AutoConstCastable<Game> GetGame(T const* src)
		{
			if constexpr (std::is_convertible_v<T, Game const>)
				return (Game const*)src;

			if constexpr (has_game_v<T>)
				return chaos::meta::get_raw_pointer(src->game);

			return nullptr;
		}

		/** returns the game instance */
		template<typename T>
		static chaos::AutoCastable<GameInstance> GetGameInstance(T* src)
		{
			if constexpr (std::is_convertible_v<T, GameInstance>)
				return (GameInstance*)src;

			if constexpr (has_game_instance_v<T>)
				return chaos::meta::get_raw_pointer(src->game_instance);

			return nullptr;
		}
		/** returns the game instance */
		template<typename T>
		static chaos::AutoConstCastable<GameInstance> GetGameInstance(T const* src)
		{
			if constexpr (std::is_convertible_v<T, GameInstance const>)
				return (GameInstance const*)src;

			if constexpr (has_game_instance_v<T>)
				return chaos::meta::get_raw_pointer(src->game_instance);

			return nullptr;
		}

		/** returns the level */
		template<typename T>
		static chaos::AutoCastable<Level> GetLevel(T* src)
		{
			if constexpr (std::is_convertible_v<T, Level>)
				return (Level*)src;

			return nullptr;
		}
		/** returns the level */
		template<typename T>
		static chaos::AutoConstCastable<Level> GetLevel(T const* src)
		{
			if constexpr (std::is_convertible_v<T, Level const>)
				return (Level const*)src;

			return nullptr;
		}

		/** returns the level instance */
		template<typename T>
		static chaos::AutoCastable<LevelInstance> GetLevelInstance(T* src)
		{
			if constexpr (std::is_convertible_v<T, LevelInstance>)
				return (LevelInstance*)src;

			if constexpr (has_level_instance_v<T>)
				return chaos::meta::get_raw_pointer(src->level_instance);





			return nullptr;
		}
		/** returns the level */
		template<typename T>
		static chaos::AutoConstCastable<LevelInstance> GetLevelInstance(T const* src)
		{
			if constexpr (std::is_convertible_v<T, LevelInstance const>)
				return (LevelInstance const*)src;

			if constexpr (has_level_instance_v<T>)
				return chaos::meta::get_raw_pointer(src->level_instance);





			return nullptr;
		}
	};


}; // namespace death
