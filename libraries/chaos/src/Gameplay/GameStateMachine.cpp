#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	// =========================================================
	// GameTransition
	// =========================================================

	GameTransition::GameTransition(GameState * in_from_state, GameState * in_to_state, TagType in_triggering_event) :
		SM::Transition(in_from_state, in_to_state, in_triggering_event)
	{
	}

	Game * GameTransition::GetGame(SM::StateMachineInstance * sm_instance)
	{
		GameStateMachineInstance * game_sm_instance = auto_cast(sm_instance);
		if (game_sm_instance != nullptr)
			return game_sm_instance->GetGame();
		return nullptr;
	}

	Game const * GameTransition::GetGame(SM::StateMachineInstance * sm_instance) const
	{
		GameStateMachineInstance const * game_sm_instance = auto_cast(sm_instance);
		if (game_sm_instance != nullptr)
			return game_sm_instance->GetGame();
		return nullptr;
	}

	// =========================================================
	// GameState
	// =========================================================

	GameState::GameState(GameStateMachine * in_state_machine) :
		SM::State(in_state_machine)
	{
	}

	Game * GameState::GetGame(SM::StateMachineInstance * sm_instance)
	{
		GameStateMachineInstance * game_sm_instance = auto_cast(sm_instance);
		if (game_sm_instance != nullptr)
			return game_sm_instance->GetGame();
		return nullptr;
	}

	Game const * GameState::GetGame(SM::StateMachineInstance const * sm_instance) const
	{
		GameStateMachineInstance const * game_sm_instance = auto_cast(sm_instance);
		if (game_sm_instance != nullptr)
			return game_sm_instance->GetGame();
		return nullptr;
	}

	// =========================================================
	// All states
	// =========================================================

	InitialState::InitialState(GameStateMachine* in_state_machine) :
		GameState(in_state_machine)
	{
		SetTag(GameStateMachineKeys::STATE_INITIAL);
		SetName("Initial");
	}

	SM::StateAction InitialState::OnEnterImpl(SM::StateMachineInstance* sm_instance, SM::StateBase* from, Object* extra_data)
	{
		Game* game = GetGame(sm_instance);
		if (game != nullptr)
			game->OnGameInitialization();
		return SM::StateAction::ChangeStateEnabled;
	}

	MainMenuState::MainMenuState(GameStateMachine * in_state_machine) :
		GameState(in_state_machine)
	{
		SetTag(GameStateMachineKeys::STATE_MAINMENU);
		SetName("MainMenu");
	}

	SM::StateAction MainMenuState::OnEnterImpl(SM::StateMachineInstance * sm_instance, SM::StateBase * from, Object * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game != nullptr)
			game->OnEnterMainMenu(from == nullptr); // very first game ?
		return SM::StateAction::ChangeStateForbidden;
	}

	void MainMenuState::OnLeaveImpl(SM::StateMachineInstance * sm_instance, SM::StateBase * to, Object * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game != nullptr)
			game->OnLeaveMainMenu();
	}

	PlayingState::PlayingState(GameStateMachine * in_state_machine) :
		GameState(in_state_machine)
	{
		SetTag(GameStateMachineKeys::STATE_PLAYING);
		SetName("Playing");
	}

	SM::StateAction PlayingState::TickImpl(SM::StateMachineInstance * sm_instance, float delta_time, Object * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game != nullptr)
			game->TickGameLoop(delta_time);
		return SM::StateAction::ChangeStateEnabled;
	}

	PauseState::PauseState(GameStateMachine * in_state_machine) :
		GameState(in_state_machine)
	{
		SetTag(GameStateMachineKeys::STATE_PAUSE);
		SetName("Pause");
	}

	SM::StateAction PauseState::OnEnterImpl(SM::StateMachineInstance * sm_instance, SM::StateBase * from, Object * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game != nullptr)
			game->OnEnterPause();
		return SM::StateAction::ChangeStateForbidden;
	}

	void PauseState::OnLeaveImpl(SM::StateMachineInstance * sm_instance, SM::StateBase * to, Object * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game != nullptr)
			game->OnLeavePause();
	}


	// =========================================================
	// All transitions
	// =========================================================

	SM::StateAction InitialToMainMenuTransition::OnEnterImpl(SM::StateMachineInstance* sm_instance, SM::StateBase* from, Object* extra_data)
	{
		return SM::StateAction::ChangeStateEnabled;
	}

	SM::StateAction InitialToMainMenuTransition::CheckTransitionConditions(SM::StateMachineInstance* sm_instance, Object* extra_data)
	{
		return SM::StateAction::ChangeStateEnabled;
	}

	// ======

	SM::StateAction MainMenuToPlayingTransition::OnEnterImpl(SM::StateMachineInstance * sm_instance, SM::StateBase * from, Object * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game == nullptr)
			return SM::StateAction::ChangeStateForbidden; // can't go further
		// try get the physical gamepad
		PhysicalGamepadWrapper * wrapper = auto_cast(extra_data);
		// enter the game
		game->OnEnterGame((wrapper == nullptr)? nullptr : wrapper->data);
		return SM::StateAction::ChangeStateEnabled;
	}

	// ======

	SM::StateAction PlayingToMainMenuTransition::OnEnterImpl(SM::StateMachineInstance * sm_instance, SM::StateBase * from, Object * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game == nullptr)
			return SM::StateAction::ChangeStateForbidden; // can't go further
		game->OnLeaveGame();
		return SM::StateAction::ChangeStateEnabled;
	}

	// ======

	SM::StateAction PlayingToGameOverTransition::OnEnterImpl(SM::StateMachineInstance * sm_instance, SM::StateBase * from, Object * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game != nullptr)
		{
			sm_instance->SetContextData(game->PlaySound("gameover", false, false, 0.0f, SoundContext::GAME));
			game->OnGameOver();
		}
		return SM::StateAction::ChangeStateForbidden;
	}

	SM::StateAction PlayingToGameOverTransition::TickImpl(SM::StateMachineInstance * sm_instance, float delta_time, Object * extra_data)
	{
		// wait until game over sound is finished
		Sound * gameover_sound = auto_cast(sm_instance->GetContextData());
		if (gameover_sound != nullptr)
			if (!gameover_sound->IsFinished())
				return SM::StateAction::ChangeStateForbidden;
		return SM::StateAction::ChangeStateEnabled;
	}

	void PlayingToGameOverTransition::OnLeaveImpl(SM::StateMachineInstance * sm_instance, SM::StateBase * to, Object * extra_data)
	{
		// destroy the sound object
		sm_instance->SetContextData(nullptr);
		// notify the game that it is finished
		Game * game = GetGame(sm_instance);
		if (game != nullptr)
			game->OnLeaveGame();
	}

	// =========================================================
	// GameStateMachine
	// =========================================================

	bool GameStateMachine::Initialize()
	{
		initial_state = new InitialState(this);
		main_menu_state = new MainMenuState(this);
		playing_state = new PlayingState(this);
		pause_state = new PauseState(this);

		initial_to_main_menu = new InitialToMainMenuTransition(initial_state.get(), main_menu_state.get(), GameStateMachineKeys::EVENT_GAME_INITIALIZED);
		main_menu_to_playing = new MainMenuToPlayingTransition(main_menu_state.get(), playing_state.get(), GameStateMachineKeys::EVENT_START_GAME);
		playing_to_main_menu = new PlayingToMainMenuTransition(playing_state.get(), main_menu_state.get(), GameStateMachineKeys::EVENT_EXIT_GAME);
		playing_to_pause = new GameTransition(playing_state.get(), pause_state.get(), GameStateMachineKeys::EVENT_TOGGLE_PAUSE);
		pause_to_playing = new GameTransition(pause_state.get(), playing_state.get(), GameStateMachineKeys::EVENT_TOGGLE_PAUSE);
		playing_to_gameover = new PlayingToGameOverTransition(playing_state.get(), main_menu_state.get(), GameStateMachineKeys::EVENT_GAME_OVER);

		SetInitialState(initial_state.get());

		return true;
	}

	// =========================================================
	// GameStateMachineInstance
	// =========================================================

	GameStateMachineInstance::GameStateMachineInstance(Game * in_game, SM::StateMachine * in_state_machine) :
		SM::StateMachineInstance(in_state_machine),
		game(in_game)
	{
		assert(in_game != nullptr);
	}

}; // namespace chaos

