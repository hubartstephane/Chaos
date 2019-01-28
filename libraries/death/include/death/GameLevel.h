#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/GeometryFramework.h>
#include <chaos/Renderable.h>
#include <chaos/Tickable.h>
#include <chaos/ClockManager.h>

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
		friend class Game;
		friend class GameInstance;

	public:

		/** get the level corresponding to this instance */
		GameLevel * GetLevel();
		/** get the level corresponding to this instance */
		GameLevel const * GetLevel() const;

		/** get the game */
		Game * GetGame();
		/** get the game */
		Game const * GetGame() const;

		/** get the game instance */
		GameInstance * GetGameInstance();
		/** get the game instance */
		GameInstance const * GetGameInstance() const;

		/** the camera box */
		chaos::box2 GetCameraBox() const { return camera_box; }
		/** the initial camera box */
		chaos::box2 GetInitialCameraBox() const { return initial_camera_box; }

		/** returns level clock */
		chaos::Clock * GetLevelClock() { return level_clock.get();}
		/** returns level clock */
		chaos::Clock const * GetLevelClock() const { return level_clock.get(); }
		/** returns the level time */
		double GetLevelClockTime() const;

		/** get the world box */
		virtual chaos::box2 GetBoundingBox() const { return chaos::box2(); }

		/** some callbacks */
		virtual void OnLevelEnded();
		/** some callbacks */
		virtual void OnLevelStarted();

		/** returns whether the level is completed */
		virtual bool IsLevelCompleted() const;
		/** returns whether we can go to next level */
		virtual bool CanCompleteLevel() const;

	protected:

		/** additionnal initialization */
		virtual bool Initialize(Game * in_game);

		/** called for each player whenever a level is started */
		virtual void OnPlayerLevelStarted(class Player * player);
		/** called for each player whenever a level is ended */
		virtual void OnPlayerLevelEnded(class Player * player);

		/** fill the rendering params before rendering */
		virtual void FillUniformProvider(chaos::GPUProgramProvider & main_uniform_provider);

		/** state changes */
		virtual void OnEnterPause();
		/** state changes */
		virtual void OnLeavePause();
		/** state changes */
		virtual void OnGameOver();

	protected:

		/** a pointer on the level that generated this */
		GameLevel * level = nullptr;
		/** pointer on the game */
		Game * game = nullptr;

		/** the camera bounding box */
		chaos::box2 camera_box;
		/** the initial camera bounding box (at level startup) */
		chaos::box2 initial_camera_box;

		/** the level clock */
		chaos::shared_ptr<chaos::Clock> level_clock;

	};

}; // namespace death
