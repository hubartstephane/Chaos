#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/GeometryFramework.h>
#include <chaos/Renderable.h>
#include <chaos/Tickable.h>
#include <chaos/ClockManager.h>
#include <chaos/ParticleManager.h>

#include <death/GameFramework.h>

namespace death
{

	// =====================================
	// GameLevelInstance : this is the runtime part of a level
	// =====================================

	class GameLevelInstance : public chaos::Renderable
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

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

		/** get a player */
		Player * GetPlayer(int player_index);
		/** get a player */
		Player const * GetPlayer(int player_index) const;

		/** the camera box getter */
		chaos::box2 GetCameraBox() const { return camera_box; }
		/** the camera box setter */
		void SetCameraBox(chaos::box2 in_box) { camera_box = in_box; }
		
		/** the initial camera box getter */
		chaos::box2 GetInitialCameraBox() const { return initial_camera_box; }
		/** the initial camera box setter */
		void SetInitialCameraBox(chaos::box2 in_box) { initial_camera_box = in_box; }

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

		/** update the player and the camera position so that they remains inside the world */
		void RestrictCameraToPlayerAndWorld(int player_index);
		/** restrict an object to the world */
		void RestrictObjectToWorld(chaos::ParticleAllocationBase * allocation, size_t index);
		/** restrict an player to the world */
		void RestrictPlayerToWorld(int player_index);
		/** restrict an player to the world */
		void RestrictPlayerToWorld(Player * player);

		/** get the current time out */
		float GetLevelTimeout() const { return level_timeout; }

		/** get the camera safe zone */
		virtual glm::vec2 GetCameraSafeZone() const { return camera_safe_zone; }

	protected:

		/** override */
		virtual bool DoTick(double delta_time) override;

		/** additionnal initialization */
		virtual bool Initialize(Game * in_game, GameLevel * in_level);

		/** called for each player whenever a level is started */
		virtual void OnPlayerEntered(Player * player);
		/** called for each player whenever a level is ended */
		virtual void OnPlayerLeaved(Player * player);

		/** fill the rendering params before rendering */
		virtual void FillUniformProvider(chaos::GPUProgramProvider & main_uniform_provider);

		/** state changes */
		virtual void OnEnterPause();
		/** state changes */
		virtual void OnLeavePause();
		/** state changes */
		virtual void OnGameOver();

		/** check whether there is a game over */
		virtual bool DoCheckGameOverCondition();

	protected:

		/** a pointer on the level that generated this */
		GameLevel * level = nullptr;
		/** pointer on the game */
		Game * game = nullptr;

		/** the safe zone of the camera */
		glm::vec2 camera_safe_zone = glm::vec2(0.8f, 0.8f);

		/** the camera bounding box */
		chaos::box2 camera_box;
		/** the initial camera bounding box (at level startup) */
		chaos::box2 initial_camera_box;

		/** the time to run the level (no time out by default) */
		float level_timeout = -1.0f;

		/** the level clock */
		chaos::shared_ptr<chaos::Clock> level_clock;
	};

}; // namespace death
