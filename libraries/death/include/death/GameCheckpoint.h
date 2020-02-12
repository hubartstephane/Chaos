#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/GeometryFramework.h>

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
	// PlayerCheckpoint
	// =============================================

	class PlayerCheckpoint : public chaos::ReferencedObject
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

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

		/** the player box */
		chaos::box2 player_box;
	};

	// =============================================
	// LevelCheckpoint
	// =============================================

	class LevelCheckpoint : public chaos::ReferencedObject
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

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

	class GameCheckpoint : public chaos::ReferencedObject
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

	public:

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
