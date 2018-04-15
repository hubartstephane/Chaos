#include "LudumStateMachine.h"
#include "LudumGame.h"

// =========================================================
// States
// =========================================================

LudumState::LudumState(chaos::StateMachine::Automata * in_automata) :
	chaos::StateMachine::State(in_automata)
{
}

LudumGame * LudumState::GetGame()
{
	LudumAutomata * ludum_automata = (LudumAutomata *)automata;
	if (ludum_automata == nullptr)
		return nullptr;
	return ludum_automata->GetGame();
}

MainMenuState::MainMenuState(chaos::StateMachine::Automata * in_automata):
	LudumState(in_automata)
{
	SetStateID(LudumAutomata::STATE_MAINMENU);
	SetName("MainMenu");
}

bool MainMenuState::OnEnterImpl(chaos::StateMachine::State * from)
{
	if (from == nullptr)
	{
		LudumGame * game = GetGame();
		if (game != nullptr)
			game->OnStartGame();
	}
	return true;
}




PlayingState::PlayingState(chaos::StateMachine::Automata * in_automata):
	LudumState(in_automata)
{
	SetStateID(LudumAutomata::STATE_PLAYING);
	SetName("Playing");
}

PauseState::PauseState(chaos::StateMachine::Automata * in_automata):
	LudumState(in_automata)
{
	SetStateID(LudumAutomata::STATE_PAUSE);
	SetName("Pause");
}

GameOverState::GameOverState(chaos::StateMachine::Automata * in_automata):
	LudumState(in_automata)
{
	SetStateID(LudumAutomata::STATE_GAMEOVER);
	SetName("GameOver");
}

// =========================================================
// Transitions
// =========================================================

LudumTransition::LudumTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state) :
	chaos::StateMachine::Transition(in_from_state, in_to_state)
{
}

LudumGame * LudumTransition::GetGame()
{
	LudumAutomata * ludum_automata = (LudumAutomata *)automata;
	if (ludum_automata == nullptr)
		return nullptr;
	return ludum_automata->GetGame();
}

MainMenuToPlayingTransition::MainMenuToPlayingTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state):
	LudumTransition(in_from_state, in_to_state)
{
}

PlayingToMainMenuTransition::PlayingToMainMenuTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state):
	LudumTransition(in_from_state, in_to_state)
{
}

PlayingToPauseTransition::PlayingToPauseTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state):
	LudumTransition(in_from_state, in_to_state)
{
}

PauseToPlayingTransition::PauseToPlayingTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state):
	LudumTransition(in_from_state, in_to_state)
{
}

PlayingToGameOverTransition::PlayingToGameOverTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state):
	LudumTransition(in_from_state, in_to_state)
{
}

GameOverToMainMenuTransition::GameOverToMainMenuTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state):
	LudumTransition(in_from_state, in_to_state)
{
}

// =========================================================
// Automata
// =========================================================

LudumAutomata::LudumAutomata(class LudumGame * in_game):
	game(in_game)
{
	assert(in_game != nullptr);

	main_menu_state = new MainMenuState(this);
	playing_state   = new PlayingState(this);
	pause_state     = new PauseState(this);
	gameover_state  = new GameOverState(this);

	main_menu_to_playing  = new MainMenuToPlayingTransition(main_menu_state, playing_state);
	playing_to_main_menu  = new PlayingToMainMenuTransition(playing_state, main_menu_state);
	playing_to_pause      = new PlayingToPauseTransition(playing_state, pause_state);
	pause_to_playing      = new PauseToPlayingTransition(pause_state, playing_state);
	playing_to_gameover   = new PlayingToGameOverTransition(playing_state, gameover_state);
	gameover_to_main_menu = new GameOverToMainMenuTransition(gameover_state, main_menu_state);

	SetInitialState(main_menu_state);
}