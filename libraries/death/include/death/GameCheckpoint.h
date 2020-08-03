#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Object.h>
#include <chaos/GeometryFramework.h>
#include <chaos/Class.h>

#include <death/GameFramework.h>

namespace death
{

	// =============================================
	// CheckpointObject
	// =============================================

	template<typename T>
	class CheckpointObject
	{
	public:

		/** the saving entry point */
		virtual T * SaveIntoCheckpoint() const
		{
			T * result = DoCreateCheckpoint();
			if (result == nullptr)
				return nullptr;
			if (!DoSaveIntoCheckpoint(result))
			{
				delete(result);
				return nullptr;
			}
			return result;
		}

		/** the loading entry point */
		virtual bool LoadFromCheckpoint(T const * checkpoint) 
		{
			if (checkpoint == nullptr)
				return false;
			return DoLoadFromCheckpoint(checkpoint);
		}

	protected:

		/** checkpoint instanciation method */
		virtual T * DoCreateCheckpoint() const
		{
			return new T();
		}
		/** internal method for saving */
		virtual bool DoSaveIntoCheckpoint(T * checkpoint) const
		{
			return true;
		}
		/** internal method for loading */
		virtual bool DoLoadFromCheckpoint(T const * checkpoint)
		{
			return true;
		}
	};

	// =============================================
	// PlayerPawnCheckpoint
	// =============================================

	class PlayerPawnCheckpoint : public chaos::Object
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(PlayerPawnCheckpoint, chaos::Object);

	public:

		/** the pawn box */
		chaos::box2 pawn_box;
	};

	// =============================================
	// PlayerCheckpoint
	// =============================================

	class PlayerCheckpoint : public chaos::Object
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(PlayerCheckpoint, chaos::Object);

	public:

		/** the life for the player */
		int life_count = 0;

		/** the health of the player */
		float health = 1.0f;
		/** the maximum health of the player */
		float max_health = 1.0f;

		/** the current invulnerability for the player */
		float invulnerability_timer = 0.0f;
		/** the duration invulnerability for the player */
		float invulnerability_duration = 1.0f;

		/** the score for the player */
		int score = 0;

		/** the data for the pawn */
		chaos::shared_ptr<PlayerPawnCheckpoint> pawn_checkpoint;
	};

	// =============================================
	// LevelCheckpoint
	// =============================================

	class LevelCheckpoint : public chaos::Object
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(LevelCheckpoint, chaos::Object);

	public:

		/** the camera 0 box */
		chaos::box2 camera_box;
		/** the current time out */
		float level_timeout = -1;
		/** the level clock time */
		double level_clock_time = 0.0;
	};

	// =============================================
	// GameCheckpoint
	// =============================================

	class GameCheckpoint : public chaos::Object
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(GameCheckpoint, chaos::Object);

	public:

		/** the player encoded into a JSON */
		nlohmann::json player_save;
		/** the game state encoded into a JSON */
		nlohmann::json level_save;

		/** the game state encoded into a JSON */
		nlohmann::json main_clock_save;
		/** the game state encoded into a JSON */
		nlohmann::json game_clock_save;

		




		/** the current level index */
		int level_index = 0;
		/** the game clock value */
		double game_clock_time = 0.0;
		/** the game clock value */
		double main_clock_time = 0.0;

		/** the data for the player */
		chaos::shared_ptr<PlayerCheckpoint> player_checkpoint;
		/** the data for the level */
		chaos::shared_ptr<LevelCheckpoint> level_checkpoint;
	};


}; // namespace death
