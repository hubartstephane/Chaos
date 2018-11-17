#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/GeometryFramework.h>
#include <chaos/Renderable.h>
#include <chaos/Tickable.h>

namespace death
{
	// =====================================
	// GameLevel : a simple game level
	// =====================================

	class GameLevel : public chaos::ReferencedObject
	{
		friend class Game;

	public:

		/** create a level instance for that level */
		class GameLevelInstance * CreateLevelInstance(Game * in_game);

		/** the index of that level */
		int GetLevelIndex() const { return level_index; }

	protected:

		/** create a level instance for that level user specified function */
		virtual class GameLevelInstance * DoCreateLevelInstance(Game * in_game);

	protected:

		/** the index of the level (maybe a key or an ordered index) */
		int level_index = 0;
	};

	// =====================================
	// GameLevelInstance : this is the runtime part of a level
	// =====================================

	class GameLevelInstance : public chaos::Renderable
	{
		friend class GameLevel;

	public:

		/** get the level corresponding to this instance */
		GameLevel * GetLevel() { return level; }
		/** get the level corresponding to this instance */
		GameLevel const * GetLevel() const { return level; }

		/** get the world box */
		chaos::box2 GetWorldBox() const { return world_box; }
		/** set the world box */
		void SetWorldBox(chaos::box2 const & in_world_box) { world_box = in_world_box; }

		/** some callbacks */
		virtual void OnLevelEnded();
		/** some callbacks */
		virtual void OnLevelStarted();

		/** returns whether the level is completed */
		virtual bool IsLevelCompleted() const;

	protected:

		/** additionnal initialization */
		virtual bool Initialize(Game * in_game);

	protected:

		/** a pointer on the level that generated this */
		GameLevel * level = nullptr;
		/** the world box */
		chaos::box2 world_box;
	};

}; // namespace death
