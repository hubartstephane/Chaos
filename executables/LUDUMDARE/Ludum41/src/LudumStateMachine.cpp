#include "LudumStateMachine.h"

// =========================================================
// States
// =========================================================

MainMenuState::MainMenuState(chaos::StateMachine::Automata * in_automata):
	chaos::StateMachine::State(in_automata)
{

}

PlayingState::PlayingState(chaos::StateMachine::Automata * in_automata):
	chaos::StateMachine::State(in_automata)
{

}

PauseState::PauseState(chaos::StateMachine::Automata * in_automata):
	chaos::StateMachine::State(in_automata)
{

}

GameOverState::GameOverState(chaos::StateMachine::Automata * in_automata):
	chaos::StateMachine::State(in_automata)
{

}

// =========================================================
// Transitions
// =========================================================

MainMenuToPlayingTransition::MainMenuToPlayingTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state):
	chaos::StateMachine::Transition(in_from_state, in_to_state)
{
}

PlayingToMainMenuTransition::PlayingToMainMenuTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state):
	chaos::StateMachine::Transition(in_from_state, in_to_state)
{
}

PlayingToPauseTransition::PlayingToPauseTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state):
	chaos::StateMachine::Transition(in_from_state, in_to_state)
{
}

PauseToPlayingTransition::PauseToPlayingTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state):
	chaos::StateMachine::Transition(in_from_state, in_to_state)
{
}

PlayingToGameOverTransition::PlayingToGameOverTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state):
	chaos::StateMachine::Transition(in_from_state, in_to_state)
{
}

GameOverToMainMenuTransition::GameOverToMainMenuTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state):
	chaos::StateMachine::Transition(in_from_state, in_to_state)
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