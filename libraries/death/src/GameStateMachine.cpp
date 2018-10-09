#include <death/GameStateMachine.h>

namespace death
{

	// =========================================================
	// GameState
	// =========================================================

	GameState::GameState(GameStateMachine * in_state_machine) :
		chaos::SM::State(in_state_machine)
	{
	}

	Game * GameState::GetGame()
	{
		GameStateMachine * game_state_machine = (GameStateMachine *)state_machine;
		if (game_state_machine == nullptr)
			return nullptr;
		return game_state_machine->GetGame();
	}

	Game const * GameState::GetGame() const
	{
		GameStateMachine const * game_state_machine = (GameStateMachine const *)state_machine;
		if (game_state_machine == nullptr)
			return nullptr;
		return game_state_machine->GetGame();
	}

	// =========================================================
	// GameTransition
	// =========================================================

	GameTransition::GameTransition(GameState * in_from_state, GameState * in_to_state) :
		chaos::SM::Transition(in_from_state, in_to_state)
	{
	}

	Game * GameTransition::GetGame()
	{
		GameStateMachine * game_state_machine = (GameStateMachine *)state_machine;
		if (game_state_machine == nullptr)
			return nullptr;
		return game_state_machine->GetGame();
	}

	Game const * GameTransition::GetGame() const
	{
		GameStateMachine const * game_state_machine = (GameStateMachine const *)state_machine;
		if (game_state_machine == nullptr)
			return nullptr;
		return game_state_machine->GetGame();
	}

	// =========================================================
	// All states
	// =========================================================

	MainMenuState::MainMenuState(GameStateMachine * in_state_machine) :
		GameState(in_state_machine)
	{
		SetStateID(GameStateMachine::STATE_MAINMENU);
		SetName("MainMenu");
	}

	bool MainMenuState::OnEnterImpl(chaos::SM::State * from, chaos::SM::StateMachineInstance * sm_instance)
	{
		Game * game = GetGame();
		if (game != nullptr)
			game->OnEnterMainMenu(from == nullptr); // very first game ?
		return false;
	}

	PlayingState::PlayingState(GameStateMachine * in_state_machine) :
		GameState(in_state_machine)
	{
		SetStateID(GameStateMachine::STATE_PLAYING);
		SetName("Playing");
	}

	bool PlayingState::TickImpl(double delta_time, chaos::SM::StateMachineInstance * sm_instance)
	{
		Game * game = GetGame();
		if (game != nullptr)
			game->TickGameLoop(delta_time);
		return true;
	}

	PauseState::PauseState(GameStateMachine * in_state_machine) :
		GameState(in_state_machine)
	{
		SetStateID(GameStateMachine::STATE_PAUSE);
		SetName("Pause");
	}

	// =========================================================
	// All transitions
	// =========================================================

	MainMenuToPlayingTransition::MainMenuToPlayingTransition(GameState * in_from_state, GameState * in_to_state) :
		GameTransition(in_from_state, in_to_state)
	{
	}

	bool MainMenuToPlayingTransition::OnEnterImpl(chaos::SM::State * from, chaos::SM::StateMachineInstance * sm_instance)
	{
		Game * game = GetGame();
		if (game == nullptr)
			return true;
		game->OnEnterGame();
		return false;
	}

	bool MainMenuToPlayingTransition::TickImpl(double delta_time, chaos::SM::StateMachineInstance * sm_instance)
	{
		Game * game = GetGame();
		if (game == nullptr)
			return true;
		return game->IsGameEnterComplete();
	}

	PlayingToMainMenuTransition::PlayingToMainMenuTransition(GameState * in_from_state, GameState * in_to_state) :
		GameTransition(in_from_state, in_to_state)
	{
	}

	bool PlayingToMainMenuTransition::OnEnterImpl(chaos::SM::State * from, chaos::SM::StateMachineInstance * sm_instance)
	{
		Game * game = GetGame();
		if (game == nullptr)
			return true;
		game->OnLeaveGame(false);
		return false;
	}

	bool PlayingToMainMenuTransition::TickImpl(double delta_time, chaos::SM::StateMachineInstance * sm_instance)
	{
		Game * game = GetGame();
		if (game == nullptr)
			return true;
		return game->IsGameLeaveComplete();
	}

	bool PlayingToMainMenuTransition::OnLeaveImpl(chaos::SM::State * to, chaos::SM::StateMachineInstance * sm_instance)
	{
		Game * game = GetGame();
		if (game == nullptr)
			return true;
		game->OnAbordGame();
		return false;
	}

	PlayingToPauseTransition::PlayingToPauseTransition(GameState * in_from_state, GameState * in_to_state) :
		GameTransition(in_from_state, in_to_state)
	{
	}


	bool PlayingToPauseTransition::OnEnterImpl(chaos::SM::State * from, chaos::SM::StateMachineInstance * sm_instance)
	{
		Game * game = GetGame();
		if (game == nullptr)
			return true;
		game->OnEnterPause();
		return false;
	}

	bool PlayingToPauseTransition::TickImpl(double delta_time, chaos::SM::StateMachineInstance * sm_instance)
	{
		Game * game = GetGame();
		if (game == nullptr)
			return true;
		return game->IsPauseEnterComplete();
	}

	PauseToPlayingTransition::PauseToPlayingTransition(GameState * in_from_state, GameState * in_to_state) :
		GameTransition(in_from_state, in_to_state)
	{
	}

	bool PauseToPlayingTransition::OnEnterImpl(chaos::SM::State * from, chaos::SM::StateMachineInstance * sm_instance)
	{
		Game * game = GetGame();
		if (game == nullptr)
			return true;
		game->OnLeavePause();
		return false;
	}

	bool PauseToPlayingTransition::TickImpl(double delta_time, chaos::SM::StateMachineInstance * sm_instance)
	{
		Game * game = GetGame();
		if (game == nullptr)
			return true;
		return game->IsPauseLeaveComplete();
	}

	PlayingToGameOverTransition::PlayingToGameOverTransition(GameState * in_from_state, GameState * in_to_state) :
		GameTransition(in_from_state, in_to_state)
	{
	}

	bool PlayingToGameOverTransition::OnEnterImpl(chaos::SM::State * from, chaos::SM::StateMachineInstance * sm_instance)
	{
		Game * game = GetGame();
		if (game != nullptr)
		{
			sm_instance->SetContextData(game->PlaySound("gameover", false, false));
			game->OnLeaveGame(true);
		}
		return false;
	}

	bool PlayingToGameOverTransition::OnLeaveImpl(chaos::SM::State * to, chaos::SM::StateMachineInstance * sm_instance)
	{
		// destroy the sound object
		sm_instance->SetContextData(nullptr);
		// notify the game that it is finished
		Game * game = GetGame();
		if (game != nullptr)
			game->OnGameOver();
		return true;
	}

	bool PlayingToGameOverTransition::TickImpl(double delta_time, chaos::SM::StateMachineInstance * sm_instance)
	{
		// wait until game over sound is finished
		chaos::Sound * gameover_sound = dynamic_cast<chaos::Sound*>(sm_instance->GetContextData());
		if (gameover_sound != nullptr)
			if (!gameover_sound->IsFinished())
				return false;
		// wait until the music is blend correctly
		Game * game = GetGame();
		if (game != nullptr)
			return game->IsGameLeaveComplete();
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

	bool GameStateMachine::CreateStateMachine()
	{
		main_menu_state = new MainMenuState(this);
		playing_state = new PlayingState(this);
		pause_state = new PauseState(this);

		main_menu_to_playing = new MainMenuToPlayingTransition(main_menu_state.get(), playing_state.get());
		playing_to_main_menu = new PlayingToMainMenuTransition(playing_state.get(), main_menu_state.get());
		playing_to_pause = new PlayingToPauseTransition(playing_state.get(), pause_state.get());
		pause_to_playing = new PauseToPlayingTransition(pause_state.get(), playing_state.get());
		playing_to_gameover = new PlayingToGameOverTransition(playing_state.get(), main_menu_state.get());

		SetInitialState(main_menu_state.get());

		return true;
	}

}; // namespace death

