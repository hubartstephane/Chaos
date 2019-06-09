#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/GeometryFramework.h>
#include <chaos/Renderable.h>
#include <chaos/Tickable.h>
#include <chaos/ClockManager.h>
#include <chaos/ParticleManager.h>
#include <death/Camera.h>
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

		/** destroy cameras when the level is finished */
		virtual void DestroyCameras();
		/** create cameras when the level is started */
		virtual void CreateCameras();

		/** returns the number of cameras */
		size_t GetCameraCount() const;
		/** gets a camera by its index */
		Camera * GetCamera(size_t index);
		/** gets a camera by its index */
		Camera const * GetCamera(size_t index) const;

		/** get the transformation world to camera */
		CameraTransform GetCameraTransform(size_t index) const;


		/** the camera box getter */
		chaos::box2 GetCameraBox(size_t index) const;
		/** the camera box setter */
		void SetCameraBox(size_t index, chaos::box2 in_box);
		/** the initial camera box getter */		
		chaos::box2 GetInitialCameraBox(size_t index) const;
		/** get the camera safe zone */
		virtual glm::vec2 GetCameraSafeZone(size_t index) const;

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
		virtual bool IsLevelCompleted(bool & loop_levels) const;
		/** returns whether we can go to next level */
		virtual bool CanCompleteLevel() const;

		/** update the player and the camera position so that they remains inside the world */
		void RestrictCameraToPlayerAndWorld(int player_index, size_t camera_index);
		/** restrict an object to the world */
		void RestrictObjectToWorld(chaos::ParticleAllocationBase * allocation, size_t index);
		/** restrict an player to the world */
		void RestrictPlayerToWorld(int player_index);
		/** restrict an player to the world */
		void RestrictPlayerToWorld(Player * player);

		/** get the current time out */
		float GetLevelTimeout() const { return level_timeout; }

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
		/** the time to run the level (no time out by default) */
		float level_timeout = -1.0f;
		/** the level clock */
		chaos::shared_ptr<chaos::Clock> level_clock;
		/** the camera */
		std::vector<chaos::shared_ptr<Camera>> cameras;
	};

}; // namespace death
