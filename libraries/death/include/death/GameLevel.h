#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>

namespace death
{
	class GameLevel : public chaos::ReferencedObject
	{
		friend class Game;

	public:

		/** create a level instance for that level */
		class GameLevelInstance * CreateLevelInstance();

		/** the index of that level */
		int GetLevelIndex() const { return level_index; }

	protected:

		/** create a level instance for that level user specified function */
		virtual class GameLevelInstance * DoCreateLevelInstance();

	protected:

		/** the index of the level (maybe a key or an ordered index) */
		int level_index = 0;
	};

	class GameLevelInstance : public chaos::ReferencedObject
	{
		friend class GameLevel;

	public:

		/** get the level corresponding to this instance */
		GameLevel * GetLevel() { return level; }
		/** get the level corresponding to this instance */
		GameLevel const * GetLevel() const { return level; }

	protected:

		/** a pointer on the level that generated this */
		GameLevel * level = nullptr;
	};

}; // namespace death
