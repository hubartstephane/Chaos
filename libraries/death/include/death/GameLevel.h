#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/GeometryFramework.h>

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

		/** ticking the level instance */
		virtual void Tick(double delta_time);

		/** get the level corresponding to this instance */
		GameLevel * GetLevel() { return level; }
		/** get the level corresponding to this instance */
		GameLevel const * GetLevel() const { return level; }

		/** get the world box */
		chaos::box2 GetWorldBox() const { return world_box; }
		/** set the world box */
		void SetWorldBox(chaos::box2 const & in_world_box) { world_box = in_world_box; }

	protected:

		/** a pointer on the level that generated this */
		GameLevel * level = nullptr;
		/** the world box */
		chaos::box2 world_box;
	};

}; // namespace death
