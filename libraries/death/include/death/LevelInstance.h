#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/GeometryFramework.h>
#include <chaos/GPURenderable.h>
#include <chaos/Tickable.h>
#include <chaos/ClockManager.h>
#include <chaos/ParticleManager.h>
#include <chaos/SoundManager.h>

#include <death/Camera.h>
#include <death/GameFramework.h>
#include <death/GameCheckpoint.h>

namespace death
{

	// =====================================
	// LevelInstance : this is the runtime part of a level
	// =====================================

	class LevelInstance : public chaos::GPURenderable, public CheckpointObject<LevelCheckpoint>
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

	public:

		/** get the level corresponding to this instance */
		Level * GetLevel();
		/** get the level corresponding to this instance */
		Level const * GetLevel() const;

		/** get the game */
		Game * GetGame();
		/** get the game */
		Game const * GetGame() const;

		/** get the game instance */
		GameInstance * GetGameInstance();
		/** get the game instance */
		GameInstance const * GetGameInstance() const;

		/** get a player */
		Player * GetPlayer(size_t player_index);
		/** get a player */
		Player const * GetPlayer(size_t player_index) const;

		/** destroy cameras when the level is finished */
		virtual void DestroyCameras();
		/** create cameras when the level is started */
		virtual void CreateCameras();

		/** returns the number of cameras */
		size_t GetCameraCount() const;
		/** gets a camera by its index (or free camera) */
		Camera * GetCamera(size_t index);
		/** gets a camera by its index (or free camera) */
		Camera const * GetCamera(size_t index) const;

		/** get the transformation world to camera */
		chaos::obox2 GetCameraOBox(size_t index, bool apply_modifiers = true) const;
		/** the camera transform whenever no camera exists */
		chaos::obox2 GetDefaultCameraOBox() const;
		/** the initial camera box getter */		
		chaos::obox2 GetInitialCameraOBox(size_t index) const;

		/** the camera box getter */
		chaos::box2 GetCameraBox(size_t index, bool apply_modifiers = true) const;
		/** the camera box setter */
		void SetCameraBox(size_t index, chaos::box2 in_box);

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
		virtual bool CheckLevelCompletion() const;
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

		/** flag the level as completed */
		void SetLevelCompletionFlag();
		/** get the completion flag */
		bool GetLevelCompletionFlag() const;

		/** returns the sound category */
		chaos::SoundCategory * GetSoundCategory();
		/** returns the sound category */
		chaos::SoundCategory const * GetSoundCategory() const;

	protected:

		/** gets a camera by its index */
		Camera * DoGetCamera(size_t index);
		/** gets a camera by its index */
		Camera const * DoGetCamera(size_t index) const;

		/** override */
		virtual bool DoTick(float delta_time) override;
		/** override */
		virtual bool CanTick() override;

		/** additionnal initialization */
		virtual bool Initialize(Game * in_game, Level * in_level);

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

		/** change the background */
		virtual void CreateBackgroundImage();
		/** change the music */
		virtual void SetInGameMusic();

		/** override */
		virtual bool DoSaveIntoCheckpoint(LevelCheckpoint * checkpoint) const override;
		/** override */
		virtual bool DoLoadFromCheckpoint(LevelCheckpoint const * checkpoint) override;

	protected:

		/** a pointer on the level that generated this */
		Level * level = nullptr;
		/** pointer on the game */
		Game * game = nullptr;
		/** the time to run the level (no time out by default) */
		float level_timeout = -1.0f;
		/** the level completion flag */
		bool level_completion_flag = false;

		/** a category for all sound started during that level */
		chaos::shared_ptr<chaos::SoundCategory> sound_category;

		/** the level clock */
		chaos::shared_ptr<chaos::Clock> level_clock;
		/** the camera */
		std::vector<chaos::shared_ptr<Camera>> cameras;
	};

}; // namespace death