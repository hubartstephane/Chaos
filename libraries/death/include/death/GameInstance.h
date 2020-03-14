#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/Tickable.h>
#include <chaos/InputEventReceiver.h>
#include <chaos/SoundManager.h>
#include <chaos/AutoCast.h>

#include <death/GameFramework.h>
#include <death/Player.h>
#include <death/Game.h>
#include <death/GameInstanceEntity.h>

namespace death
{

	// =============================================
	// GameInstance
	// =============================================

	class GameInstance : public chaos::Tickable, public chaos::InputEventReceiver, public CheckpointObject<GameCheckpoint>
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

	public:

		DEATH_GAMEENTITY_IMPLEMENT();

		/** constructor */
		GameInstance(Game * in_game);

		/** returns the game */
		chaos::AutoCastable<Game> GetGame() { return game; }
		/** returns the game */
		chaos::AutoConstCastable<Game> GetGame() const { return game; }

		/** returns the level */
		chaos::AutoCastable<Level> GetLevel();
		/** returns the level */
		chaos::AutoConstCastable<Level> GetLevel() const;

		/** returns the level instance */
		chaos::AutoCastable<LevelInstance> GetLevelInstance();
		/** returns the level instance */
		chaos::AutoConstCastable<LevelInstance> GetLevelInstance() const;

		/** get the player by its index */
		chaos::AutoCastable<Player> GetPlayer(size_t player_index);
		/** get the player by its index */
		chaos::AutoConstCastable<Player> GetPlayer(size_t player_index) const;
		/** get the player count */
		size_t GetPlayerCount() const;

		/** create one player and give it the gamepad provided if any */
		Player * CreatePlayer(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad);

		/** try to give a physical to any player (returns the player) */
		Player * GivePhysicalGamepadToPlayer(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad);

		/** get the best score among players */
		int GetBestPlayerScore() const;

		// The clocks: 
		//   - root  clock : the top level clock. never reseted, never paused
		//   - main  clock : reseted whenever a new game starts/ends. never paused
		//   - game  clock : reseted whenever a new game starts/ends. paused in MainMenu and Pause
		//   - level clock : reseted whenever a new level starts/ends. paused in MainMenu and Pause
		//   - pause clock : reseted whenever we enter/leave pause. only running during pause

		/** returns main clock */
		chaos::Clock * GetMainClock() { return main_clock.get(); }
		/** returns main clock */
		chaos::Clock const * GetMainClock() const { return main_clock.get(); }

		/** returns game clock */
		chaos::Clock * GetGameClock() { return game_clock.get(); }
		/** returns game clock */
		chaos::Clock const * GetGameClock() const { return game_clock.get(); }

		/** returns pause clock */
		chaos::Clock * GetPauseClock() { return pause_clock.get(); }
		/** returns pause clock */
		chaos::Clock const * GetPauseClock() const { return pause_clock.get(); }

		/** returns the main time */
		double GetMainClockTime() const;
		/** returns the game time */
		double GetGameClockTime() const;
		/** returns the pause time */
		double GetPauseClockTime() const;

		/** create a respawn checkpoint */
		bool CreateRespawnCheckpoint();
		/** restart from the respawn checkpoint */
		bool RestartFromRespawnCheckpoint();

		/** returns the sound category */
		chaos::SoundCategory * GetSoundCategory();
		/** returns the sound category */
		chaos::SoundCategory const * GetSoundCategory() const;

		/** initialization of the game instance */
		virtual bool InitializeGameValues(nlohmann::json const& config, boost::filesystem::path const& config_path, bool hot_reload);
		/** called after game instance configuration has been (re)loaded */
		virtual void OnGameValuesChanged(bool hot_reload);

	protected:

		/** initialize the game instance */
		virtual bool Initialize(death::Game * in_game);

		/** override */
		virtual bool DoTick(float delta_time) override;

		/** handle an uncatched gamepad input incomming */
		virtual bool OnGamepadInput(chaos::MyGLFW::PhysicalGamepad* in_physical_gamepad); 
		/** handle keyboard input */
		virtual bool OnKeyEventImpl(chaos::KeyEvent const & event) override;
		/** handle keyboard input */
		virtual bool OnCharEventImpl(unsigned int c) override;
		/** handle mouse input */
		virtual bool OnMouseButtonImpl(int button, int action, int modifier) override;
		/** handle mouse movement */
		virtual bool OnMouseMoveImpl(double x, double y) override;

		/** returns the maximum number of player */
		virtual size_t GetMaxPlayerCount() const;

		/** return a new player */
		virtual Player * DoCreatePlayer();

		/** fill the rendering params before rendering */
		virtual void FillUniformProvider(chaos::GPUProgramProvider & main_uniform_provider);

		/** state changes */
		virtual void OnEnterPause();
		/** state changes */
		virtual void OnLeavePause();
		/** state changes */
		virtual void OnGameOver();

		/* returns true whether we can change level */
		virtual bool CanCompleteLevel() const;

		/** pause/resume pause/game clocks */
		void OnPauseStateUpdateClocks(bool enter_pause);

		/** called whenever the level is being changed */
		virtual void OnLevelChanged(Level * new_level, Level * old_level, LevelInstance * new_level_instance);

		/** check whether there is a game over */
		virtual bool DoCheckGameOverCondition();

		/** called for each player whenever a level is started */
		virtual void OnPlayerEntered(Player * player);
		/** called for each player whenever a level is ended */
		virtual void OnPlayerLeaved(Player * player);

		/** called whenever the game is started */
		virtual void OnEnterGame();
		/** called whenever the game is finished */
		virtual void OnLeaveGame();
		
		/** override */
		virtual bool DoSaveIntoCheckpoint(GameCheckpoint * checkpoint) const override;
		/** override */
		virtual bool DoLoadFromCheckpoint(GameCheckpoint const * checkpoint) override;

		/** override */
		virtual GLFWwindow* GetGLFWWindow() const override;

	protected:

		/** the game */
		Game * game = nullptr;

		/** the configuration object to use for players */
		nlohmann::json player_configuration;

		/** all the players present in the game */
		std::vector<chaos::shared_ptr<Player>> players;

		/** the clocks */
		chaos::shared_ptr<chaos::Clock> main_clock;
		chaos::shared_ptr<chaos::Clock> game_clock;
		chaos::shared_ptr<chaos::Clock> pause_clock;

		/** a sound category for the game instance */
		chaos::shared_ptr<chaos::SoundCategory> sound_category;

		/** respawn checkpoint */
		chaos::shared_ptr<GameCheckpoint> respawn_checkpoint;
	};

}; // namespace death
