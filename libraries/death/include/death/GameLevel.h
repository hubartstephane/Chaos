#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>

#include <death/GameFramework.h>

namespace death
{
	// =====================================
	// GameLevel : a simple game level
	// =====================================

	class GameLevel : public chaos::ReferencedObject
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

	public:

		/** create a level instance for that level */
		GameLevelInstance * CreateLevelInstance(Game * in_game);

		/** the index of that level */
		int GetLevelIndex() const { return level_index; }

	protected:

		/** create a level instance for that level user specified function */
		virtual class GameLevelInstance * DoCreateLevelInstance(Game * in_game);

	protected:

		/** the index of the level (maybe a key or an ordered index) */
		int level_index = 0;
	};

}; // namespace death
