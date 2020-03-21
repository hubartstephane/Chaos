#pragma once

// XXX : only include from .cpp !!

#include <chaos/StandardHeaders.h>
#include <chaos/AutoCast.h>
#include <death/GameFramework.h>

#include <death/Player.h>
#include <death/PlayerPawn.h>
#include <death/Camera.h>
#include <death/Game.h>
#include <death/GameInstance.h>
#include <death/LevelInstance.h>
#include <death/Level.h>

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
			if constexpr (std::is_convertible_v<T, Game>)
				return (Game*)src;


			return nullptr;
		}
		/** returns the game */
		template<typename T>
		static chaos::AutoConstCastable<Game> GetGame(T const* src)
		{
		//	if constexpr (std::is_convertible_v<T, Game const>)
		//		return (Game const*)src;

			return nullptr;
		}

		/** returns the game instance */
		template<typename T>
		static chaos::AutoCastable<GameInstance> GetGameInstance(T* src)
		{
		//	if constexpr (std::is_convertible_v<T, GameInstance>)
		//		return (GameInstance*)src;

			return nullptr;
		}
		/** returns the game instance */
		template<typename T>
		static chaos::AutoConstCastable<GameInstance> GetGameInstance(T const* src)
		{
		//	if constexpr (std::is_convertible_v<T, GameInstance const>)
		//		return (GameInstance const*)src;

			return nullptr;
		}

		/** returns the level */
		template<typename T>
		static chaos::AutoCastable<Level> GetLevel(T* src)
		{
		//	if constexpr (std::is_convertible_v<T, Level>)
		//		return (Level*)src;

			return nullptr;
		}
		/** returns the level */
		template<typename T>
		static chaos::AutoConstCastable<Level> GetLevel(T const* src)
		{
		//	if constexpr (std::is_convertible_v<T, Level const>)
		//		return (Level const*)src;

			return nullptr;
		}

		/** returns the level instance */
		template<typename T>
		static chaos::AutoCastable<LevelInstance> GetLevelInstance(T* src)
		{
		//	if constexpr (std::is_convertible_v<T, LevelInstance>)
		//		return (LevelInstance*)src;

			return nullptr;
		}
		/** returns the level */
		template<typename T>
		static chaos::AutoConstCastable<LevelInstance> GetLevelInstance(T const* src)
		{
		//	if constexpr (std::is_convertible_v<T, LevelInstance const>)
		//		return (LevelInstance const*)src;

			return nullptr;
		}
	};


}; // namespace death
