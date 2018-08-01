#include <death/GameAutomata.h>

namespace death
{

	// =========================================================
	// GameState
	// =========================================================

	GameState::GameState(GameAutomata * in_automata) :
		chaos::StateMachine::State(in_automata)
	{
	}

	Game * GameState::GetGame()
	{
		GameAutomata * game_automata = (GameAutomata *)automata;
		if (game_automata == nullptr)
			return nullptr;
		return game_automata->GetGame();
	}

	Game const * GameState::GetGame() const
	{
		GameAutomata const * game_automata = (GameAutomata const *)automata;
		if (game_automata == nullptr)
			return nullptr;
		return game_automata->GetGame();
	}

	// =========================================================
	// GameTransition
	// =========================================================

	GameTransition::GameTransition(GameState * in_from_state, GameState * in_to_state) :
		chaos::StateMachine::Transition(in_from_state, in_to_state)
	{
	}

	Game * GameTransition::GetGame()
	{
		GameAutomata * game_automata = (GameAutomata *)automata;
		if (game_automata == nullptr)
			return nullptr;
		return game_automata->GetGame();
	}

	Game const * GameTransition::GetGame() const
	{
		GameAutomata const * game_automata = (GameAutomata const *)automata;
		if (game_automata == nullptr)
			return nullptr;
		return game_automata->GetGame();
	}

	// =========================================================
	// All states
	// =========================================================

	MainMenuState::MainMenuState(GameAutomata * in_automata) :
		GameState(in_automata)
	{
		SetStateID(GameAutomata::STATE_MAINMENU);
		SetName("MainMenu");
	}

	bool MainMenuState::OnEnterImpl(chaos::StateMachine::State * from)
	{
		Game * game = GetGame();
		if (game != nullptr)
			game->OnEnterMainMenu(from == nullptr); // very first game ?
		return false;
	}

	PlayingState::PlayingState(GameAutomata * in_automata) :
		GameState(in_automata)
	{
		SetStateID(GameAutomata::STATE_PLAYING);
		SetName("Playing");
	}

	bool PlayingState::TickImpl(double delta_time)
	{
		Game * game = GetGame();
		if (game != nullptr)
			game->TickGameLoop(delta_time);
		return true;
	}

	PauseState::PauseState(GameAutomata * in_automata) :
		GameState(in_automata)
	{
		SetStateID(GameAutomata::STATE_PAUSE);
		SetName("Pause");
	}

	// =========================================================
	// All transitions
	// =========================================================

	MainMenuToPlayingTransition::MainMenuToPlayingTransition(GameState * in_from_state, GameState * in_to_state) :
		GameTransition(in_from_state, in_to_state)
	{
	}

	bool MainMenuToPlayingTransition::OnEnterImpl(chaos::StateMachine::State * from)
	{
		Game * game = GetGame();
		if (game == nullptr)
			return true;
		game->OnEnterGame();
		return false;
	}

	bool MainMenuToPlayingTransition::TickImpl(double delta_time)
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

	bool PlayingToMainMenuTransition::OnEnterImpl(chaos::StateMachine::State * from)
	{
		Game * game = GetGame();
		if (game == nullptr)
			return true;
		game->OnLeaveGame(false);
		return false;
	}

	bool PlayingToMainMenuTransition::TickImpl(double delta_time)
	{
		Game * game = GetGame();
		if (game == nullptr)
			return true;
		return game->IsGameLeaveComplete();
	}

	bool PlayingToMainMenuTransition::OnLeaveImpl(chaos::StateMachine::State * to)
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


	bool PlayingToPauseTransition::OnEnterImpl(chaos::StateMachine::State * from)
	{
		Game * game = GetGame();
		if (game == nullptr)
			return true;
		game->OnEnterPause();
		return false;
	}

	bool PlayingToPauseTransition::TickImpl(double delta_time)
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

	bool PauseToPlayingTransition::OnEnterImpl(chaos::StateMachine::State * from)
	{
		Game * game = GetGame();
		if (game == nullptr)
			return true;
		game->OnLeavePause();
		return false;
	}

	bool PauseToPlayingTransition::TickImpl(double delta_time)
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

	bool PlayingToGameOverTransition::OnEnterImpl(chaos::StateMachine::State * from)
	{
		Game * game = GetGame();
		if (game != nullptr)
		{
			gameover_sound = game->PlaySound("gameover", false, false);
			game->OnLeaveGame(true);
		}
		return false;
	}

	bool PlayingToGameOverTransition::OnLeaveImpl(chaos::StateMachine::State * to)
	{
		// notify the game that it is finished
		Game * game = GetGame();
		if (game != nullptr)
			game->OnGameOver();
		// destroy the sound object
		gameover_sound = nullptr;
		return true;
	}

	bool PlayingToGameOverTransition::TickImpl(double delta_time)
	{
		// wait until game over sound is finished
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
	// GameAutomata
	// =========================================================

	GameAutomata::GameAutomata(class Game * in_game) :
		game(in_game)
	{
		assert(in_game != nullptr);
	}

	bool GameAutomata::CreateAutomata()
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

