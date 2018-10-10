#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/StateMachine.h>

#include <death/Game.h>

namespace death
{

	// =========================================================
	// GameState
	// =========================================================

	class GameState : public chaos::SM::State
	{
	public:

		/** constructor */
		GameState(class GameStateMachine * in_state_machine);
		/** get the game */
		Game * GetGame(chaos::SM::StateMachineInstance * sm_instance);
		/** get the game */
		Game const * GetGame(chaos::SM::StateMachineInstance const * sm_instance) const;
	};

	// =========================================================
	// GameTransition
	// =========================================================

	class GameTransition : public chaos::SM::Transition
	{
	public:

		/** constructor */
		GameTransition(GameState * in_from_state, GameState * in_to_state);
		/** get the game */
		Game * GetGame(chaos::SM::StateMachineInstance * sm_instance);
		/** get the game */
		Game const * GetGame(chaos::SM::StateMachineInstance * sm_instance) const;
	};

	// =========================================================
	// All states
	// =========================================================

	class MainMenuState : public GameState
	{
	public:

		/** constructor */
		MainMenuState(GameStateMachine * in_state_machine);
		/** override */
		virtual bool OnEnterImpl(chaos::SM::StateBase * from_state, chaos::SM::StateMachineInstance * sm_instance) override;
	};

	class PlayingState : public GameState
	{
	public:

		/** constructor */
		PlayingState(GameStateMachine * in_state_machine);
		/** override */
		virtual bool TickImpl(double delta_time, chaos::SM::StateMachineInstance * sm_instance) override;
	};

	class PauseState : public GameState
	{
	public:

		/** constructor */
		PauseState(GameStateMachine * in_state_machine);
	};

	// =========================================================
	// Transitions
	// =========================================================

	class MainMenuToPlayingTransition : public GameTransition
	{
	public:

		/** constructor */
		MainMenuToPlayingTransition(GameState * in_from_state, GameState * in_to_state);

	protected:

		/** overriding */
		virtual bool OnEnterImpl(chaos::SM::StateBase * from, chaos::SM::StateMachineInstance * sm_instance) override;
		/** overriding */
		virtual bool TickImpl(double delta_time, chaos::SM::StateMachineInstance * sm_instance) override;
	};

	class PlayingToMainMenuTransition : public GameTransition
	{

	public:

		/** constructor */
		PlayingToMainMenuTransition(GameState * in_from_state, GameState * in_to_state);

	protected:

		/** overriding */
		virtual bool OnEnterImpl(chaos::SM::StateBase * from, chaos::SM::StateMachineInstance * sm_instance) override;
		/** overriding */
		virtual bool OnLeaveImpl(chaos::SM::StateBase * to, chaos::SM::StateMachineInstance * sm_instance) override;
		/** overriding */
		virtual bool TickImpl(double delta_time, chaos::SM::StateMachineInstance * sm_instance) override;
	};

	class PlayingToPauseTransition : public GameTransition
	{

	public:

		/** constructor */
		PlayingToPauseTransition(GameState * in_from_state, GameState * in_to_state);

	protected:

		/** overriding */
		virtual bool OnEnterImpl(chaos::SM::StateBase * from, chaos::SM::StateMachineInstance * sm_instance) override;
		/** overriding */
		virtual bool TickImpl(double delta_time, chaos::SM::StateMachineInstance * sm_instance) override;
	};


	class PauseToPlayingTransition : public GameTransition
	{

	public:

		/** constructor */
		PauseToPlayingTransition(GameState * in_from_state, GameState * in_to_state);

	protected:

		/** overriding */
		virtual bool OnEnterImpl(chaos::SM::StateBase * from, chaos::SM::StateMachineInstance * sm_instance) override;
		/** overriding */
		virtual bool TickImpl(double delta_time, chaos::SM::StateMachineInstance * sm_instance) override;
	};

	class PlayingToGameOverTransition : public GameTransition
	{

	public:

		/** constructor */
		PlayingToGameOverTransition(GameState * in_from_state, GameState * in_to_state);

		/** overriding */
		virtual bool OnEnterImpl(chaos::SM::StateBase * from, chaos::SM::StateMachineInstance * sm_instance) override;
		/** overriding */
		virtual bool OnLeaveImpl(chaos::SM::StateBase * to, chaos::SM::StateMachineInstance * sm_instance) override;
		/** overriding */
		virtual bool TickImpl(double delta_time, chaos::SM::StateMachineInstance * sm_instance) override;
	};

	// =========================================================
	// GameStateMachine
	// =========================================================

	class GameStateMachine : public chaos::SM::StateMachine
	{
		friend class Game;

	public:

		// the possible states
		static int const STATE_MAINMENU = 1;
		static int const STATE_PAUSE = 2;
		static int const STATE_PLAYING = 3;

		// the possible transition-states
		static int const STATE_TRANSITION_MAINMENU_TO_PLAYING = 5;
		static int const STATE_TRANSITION_PLAYING_TO_PAUSE = 6;
		static int const STATE_TRANSITION_PAUSE_TO_PLAYING = 7;

		// the events
		static int const EVENT_PRESS_START  = 1;
		static int const EVENT_TOGGLE_PAUSE = 2;
		static int const EVENT_EXIT_GAME    = 3;
		static int const EVENT_START_GAME   = 4;
		static int const EVENT_GAME_OVER    = 5;

		/** constructor */
		GameStateMachine(Game * in_game);

		/** get the game */
		Game * GetGame() { return game; }
		/** get the game */
		Game const * GetGame() const { return game; }

		/** create states and transitions */
		virtual bool InitializeStateMachine() override;

	protected:

		/** the game */
		Game * game = nullptr;

	public:

		/** the states */
		boost::intrusive_ptr<GameState> main_menu_state;
		boost::intrusive_ptr<GameState> playing_state;
		boost::intrusive_ptr<GameState> pause_state;

		/** the transitions */
		boost::intrusive_ptr<GameTransition> main_menu_to_playing;
		boost::intrusive_ptr<GameTransition> playing_to_main_menu;
		boost::intrusive_ptr<GameTransition> playing_to_pause;
		boost::intrusive_ptr<GameTransition> pause_to_playing;
		boost::intrusive_ptr<GameTransition> playing_to_gameover;
	};

	// =========================================================
	// GameStateMachineInstance
	// =========================================================

	class GameStateMachineInstance : public chaos::SM::StateMachineInstance
	{
	public:

		/** constructor */
		GameStateMachineInstance(Game * in_game, chaos::SM::StateMachine * in_state_machine);

		/** get the game */
		Game * GetGame()
		{
			return game;
		}
		/** get the game */
		Game const * GetGame() const
		{
			return game;		
		}

	protected:

		/** the game */
		Game * game = nullptr;	
	};

}; // namespace death
