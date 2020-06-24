#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/StateMachine.h>

#include <death/Game.h>

namespace death
{
	namespace GameStateMachineKeys
	{
		// the possible states
		CHAOS_DECLARE_TAG(STATE_MAINMENU);
		CHAOS_DECLARE_TAG(STATE_PAUSE);
		CHAOS_DECLARE_TAG(STATE_PLAYING);

		// the events
		CHAOS_DECLARE_TAG(EVENT_TOGGLE_PAUSE);
		CHAOS_DECLARE_TAG(EVENT_EXIT_GAME);
		CHAOS_DECLARE_TAG(EVENT_START_GAME);
		CHAOS_DECLARE_TAG(EVENT_GAME_OVER);
	};

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
		GameTransition(GameState * in_from_state, GameState * in_to_state, chaos::TagType in_triggering_event);
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
		virtual bool OnEnterImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * from_state, chaos::Object * extra_data) override;
		/** override */
		virtual bool OnLeaveImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * to, chaos::Object * extra_data) override;
	};

	class PlayingState : public GameState
	{
	public:

		/** constructor */
		PlayingState(GameStateMachine * in_state_machine);
		/** override */
		virtual bool TickImpl(chaos::SM::StateMachineInstance * sm_instance, float delta_time, chaos::Object * extra_data) override;
	};

	class PauseState : public GameState
	{
	public:

		/** constructor */
		PauseState(GameStateMachine * in_state_machine);
		/** override */
		virtual bool OnEnterImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * from_state, chaos::Object * extra_data) override;
		/** override */
		virtual bool OnLeaveImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * to, chaos::Object * extra_data) override;
	};

	// =========================================================
	// Transitions
	// =========================================================

	class MainMenuToPlayingTransition : public GameTransition
	{
	public:

		using GameTransition::GameTransition; // inherit constructor

	protected:

		/** overriding */
		virtual bool OnEnterImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * from, chaos::Object * extra_data) override;
	};

	class PlayingToMainMenuTransition : public GameTransition
	{

	public:

		using GameTransition::GameTransition; // inherit constructor

	protected:

		/** overriding */
		virtual bool OnEnterImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * from, chaos::Object * extra_data) override;
	};
	
	class PlayingToGameOverTransition : public GameTransition
	{

	public:

		using GameTransition::GameTransition; // inherit constructor

	protected:

		/** overriding */
		virtual bool OnEnterImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * from, chaos::Object * extra_data) override;
		/** overriding */
		virtual bool OnLeaveImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * to, chaos::Object * extra_data) override;
		/** overriding */
		virtual bool TickImpl(chaos::SM::StateMachineInstance * sm_instance, float delta_time, chaos::Object * extra_data) override;
	};

	// =========================================================
	// GameStateMachine
	// =========================================================

	class GameStateMachine : public chaos::SM::StateMachine
	{
		friend class Game;

		CHAOS_OBJECT_DECLARE_CLASS2(GameStateMachine, chaos::SM::StateMachine);

	public:

		/** create states and transitions */
		virtual bool Initialize() override;

	public:

		/** the states */
		chaos::shared_ptr<GameState> main_menu_state;
		chaos::shared_ptr<GameState> playing_state;
		chaos::shared_ptr<GameState> pause_state;

		/** the transitions */
		chaos::shared_ptr<GameTransition> main_menu_to_playing;
		chaos::shared_ptr<GameTransition> playing_to_main_menu;
		chaos::shared_ptr<GameTransition> playing_to_pause;
		chaos::shared_ptr<GameTransition> pause_to_playing;
		chaos::shared_ptr<GameTransition> playing_to_gameover;
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
