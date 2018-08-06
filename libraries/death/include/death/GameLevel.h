#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>

namespace death
{
	class GameLevel : public chaos::ReferencedObject
	{
	public:

		/** create a level instance for that level */
		GameLevelInstance * CreateLevelInstance();

	protected:

		/** create a level instance for that level user specified function */
		virtual GameLevelInstance * DoCreateLevelInstance();
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

		GameLevel * level = nullptr;

	};

}; // namespace death
