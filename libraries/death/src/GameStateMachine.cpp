#include <death/GameStateMachine.h>
#include <death/SoundContext.h>

namespace death
{

	// =========================================================
	// GameTransition
	// =========================================================

	GameTransition::GameTransition(GameState * in_from_state, GameState * in_to_state, chaos::TagType in_triggering_event) :
		chaos::SM::Transition(in_from_state, in_to_state, in_triggering_event)
	{
	}

	Game * GameTransition::GetGame(chaos::SM::StateMachineInstance * sm_instance)
	{
		GameStateMachineInstance * game_state_machine_instance = auto_cast(sm_instance);
		if (game_state_machine_instance != nullptr)
			return game_state_machine_instance->GetGame();
		return nullptr;
	}

	Game const * GameTransition::GetGame(chaos::SM::StateMachineInstance * sm_instance) const
	{
		GameStateMachineInstance const * game_state_machine_instance = auto_cast(sm_instance);
		if (game_state_machine_instance != nullptr)
			return game_state_machine_instance->GetGame();
		return nullptr;
	}

	// =========================================================
	// GameState
	// =========================================================

	GameState::GameState(GameStateMachine * in_state_machine) :
		chaos::SM::State(in_state_machine)
	{
	}

	Game * GameState::GetGame(chaos::SM::StateMachineInstance * sm_instance)
	{
		GameStateMachineInstance * game_state_machine_instance = auto_cast(sm_instance);
		if (game_state_machine_instance != nullptr)
			return game_state_machine_instance->GetGame();
		return nullptr;
	}

	Game const * GameState::GetGame(chaos::SM::StateMachineInstance const * sm_instance) const
	{
		GameStateMachineInstance const * game_state_machine_instance = auto_cast(sm_instance);
		if (game_state_machine_instance != nullptr)
			return game_state_machine_instance->GetGame();
		return nullptr;
	}

	// =========================================================
	// All states
	// =========================================================

	MainMenuState::MainMenuState(GameStateMachine * in_state_machine) :
		GameState(in_state_machine)
	{
		SetTag(GameStateMachineKeys::STATE_MAINMENU);
		SetName("MainMenu");
	}

	bool MainMenuState::OnEnterImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * from, chaos::ReferencedObject * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game != nullptr)
			game->OnEnterMainMenu(from == nullptr); // very first game ?
		return false;
	}

	bool MainMenuState::OnLeaveImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * to, chaos::ReferencedObject * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game != nullptr)
			game->OnLeaveMainMenu();
		return true;
	}

	PlayingState::PlayingState(GameStateMachine * in_state_machine) :
		GameState(in_state_machine)
	{
		SetTag(GameStateMachineKeys::STATE_PLAYING);
		SetName("Playing");
	}

	bool PlayingState::TickImpl(chaos::SM::StateMachineInstance * sm_instance, double delta_time, chaos::ReferencedObject * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game != nullptr)
			game->TickGameLoop(delta_time);
		return true;
	}

	PauseState::PauseState(GameStateMachine * in_state_machine) :
		GameState(in_state_machine)
	{
		SetTag(GameStateMachineKeys::STATE_PAUSE);
		SetName("Pause");
	}

	bool PauseState::OnEnterImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * from, chaos::ReferencedObject * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game != nullptr)
			game->OnEnterPause();
		return false;
	}

	bool PauseState::OnLeaveImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * to, chaos::ReferencedObject * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game != nullptr)
			game->OnLeavePause();
		return true;
	}


	// =========================================================
	// All transitions
	// =========================================================

	bool MainMenuToPlayingTransition::OnEnterImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * from, chaos::ReferencedObject * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game == nullptr)
			return true;
		// try get the physical gamepad
		PhysicalGamepadWrapper * wrapper = auto_cast(extra_data); 		
		// enter the game
		game->OnEnterGame((wrapper == nullptr)? nullptr : wrapper->data);
		return false;
	}

	// ======

	bool PlayingToMainMenuTransition::OnEnterImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * from, chaos::ReferencedObject * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game == nullptr)
			return true;
		game->OnLeaveGame();
		return false;
	}

	// ======

	bool PlayingToGameOverTransition::OnEnterImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * from, chaos::ReferencedObject * extra_data)
	{
		Game * game = GetGame(sm_instance);
		if (game != nullptr)
		{
			sm_instance->SetContextData(game->Play("gameover", false, false, 0.0f, SoundContext::GAME));
			game->OnGameOver();
		}
		return false;
	}

	bool PlayingToGameOverTransition::TickImpl(chaos::SM::StateMachineInstance * sm_instance, double delta_time, chaos::ReferencedObject * extra_data)
	{
		// wait until game over sound is finished
		chaos::Sound * gameover_sound = auto_cast(sm_instance->GetContextData());
		if (gameover_sound != nullptr)
			if (!gameover_sound->IsFinished())
				return false;
		return true;
	}

	bool PlayingToGameOverTransition::OnLeaveImpl(chaos::SM::StateMachineInstance * sm_instance, chaos::SM::StateBase * to, chaos::ReferencedObject * extra_data)
	{
		// destroy the sound object
		sm_instance->SetContextData(nullptr);
		// notify the game that it is finished
		Game * game = GetGame(sm_instance);
		if (game != nullptr)
			game->OnLeaveGame();
		return true;
	}

	// =========================================================
	// GameStateMachine
	// =========================================================

	GameStateMachine::GameStateMachine(class Game * in_game) :
		game(in_game)
	{
		assert(in_game != nullptr);
	}

	bool GameStateMachine::InitializeStateMachine()
	{
		main_menu_state = new MainMenuState(this);
		playing_state = new PlayingState(this);
		pause_state = new PauseState(this);

		main_menu_to_playing = new MainMenuToPlayingTransition(main_menu_state.get(), playing_state.get(), GameStateMachineKeys::EVENT_START_GAME);
		playing_to_main_menu = new PlayingToMainMenuTransition(playing_state.get(), main_menu_state.get(), GameStateMachineKeys::EVENT_EXIT_GAME);
		playing_to_pause = new GameTransition(playing_state.get(), pause_state.get(), GameStateMachineKeys::EVENT_TOGGLE_PAUSE);
		pause_to_playing = new GameTransition(pause_state.get(), playing_state.get(), GameStateMachineKeys::EVENT_TOGGLE_PAUSE);
		playing_to_gameover = new PlayingToGameOverTransition(playing_state.get(), main_menu_state.get(), GameStateMachineKeys::EVENT_GAME_OVER);

		SetInitialState(main_menu_state.get());

		return true;
	}

	// =========================================================
	// GameStateMachineInstance
	// =========================================================

	GameStateMachineInstance::GameStateMachineInstance(Game * in_game, chaos::SM::StateMachine * in_state_machine) : 
		chaos::SM::StateMachineInstance(in_state_machine),
		game(in_game)
	{
		assert(in_game != nullptr);
	}

}; // namespace death

