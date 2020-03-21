#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/FileResource.h>

#include <death/GameFramework.h>
#include <death/GameInstanceEntity.h>

namespace death
{
	// =====================================
	// Level : a simple game level
	// =====================================

	class Level : public chaos::ReferencedObject, public chaos::FileResource
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

	public:

		/** create a level instance for that level */
		LevelInstance * CreateLevelInstance(Game * in_game);

		/** returns the level timeout */
		float GetLevelTimeout() const { return level_timeout; }
		/** returns the title for that level */
		std::string const & GetLevelTitle() const { return level_title; }
		/** the index of that level */
		int GetLevelIndex() const { return level_index; }

	protected:

		/** create a level instance for that level user specified function */
		virtual class LevelInstance * DoCreateLevelInstance(Game * in_game);

	protected:

		/** get the level timeout */
		float level_timeout = -1.0f;
		/** the title of the level*/
		std::string level_title;
		/** the index of the level (maybe a key or an ordered index) */
		int level_index = 0;
	};

}; // namespace death
