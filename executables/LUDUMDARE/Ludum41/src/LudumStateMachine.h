#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/StateMachine.h>

// =========================================================
// States
// =========================================================

class MainMenuState : public chaos::StateMachine::State
{
public:

	/** constructor */
	MainMenuState(chaos::StateMachine::Automata * in_automata);


};

class PlayingState : public chaos::StateMachine::State
{
public:

	/** constructor */
	PlayingState(chaos::StateMachine::Automata * in_automata);

};

class PauseState : public chaos::StateMachine::State
{
public:

	/** constructor */
	PauseState(chaos::StateMachine::Automata * in_automata);
};

class GameOverState : public chaos::StateMachine::State
{
public:

	/** constructor */
	GameOverState(chaos::StateMachine::Automata * in_automata);
};

// =========================================================
// Transitions
// =========================================================

class MainMenuToPlayingTransition : public chaos::StateMachine::Transition
{
public:

	/** constructor */
	MainMenuToPlayingTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state);


};

class PlayingToMainMenuTransition : public chaos::StateMachine::Transition
{

public:

	/** constructor */
	PlayingToMainMenuTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state);

};

class PlayingToPauseTransition : public chaos::StateMachine::Transition
{

public:

	/** constructor */
	PlayingToPauseTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state);

};


class PauseToPlayingTransition : public chaos::StateMachine::Transition
{

public:

	/** constructor */
	PauseToPlayingTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state);

};

class PlayingToGameOverTransition : public chaos::StateMachine::Transition
{

public:

	/** constructor */
	PlayingToGameOverTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state);

};

class GameOverToMainMenuTransition : public chaos::StateMachine::Transition
{

public:

	/** constructor */
	GameOverToMainMenuTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state);

};

// =========================================================
// Automata
// =========================================================

class LudumAutomata : public chaos::StateMachine::Automata
{
public:

	/** constructor */
	LudumAutomata(class LudumGame * in_game);


protected:

	/** the game */
	class LudumGame * game = nullptr;

	/** the states */
	chaos::StateMachine::State * main_menu_state = nullptr;
	chaos::StateMachine::State * playing_state   = nullptr;
	chaos::StateMachine::State * pause_state     = nullptr;
	chaos::StateMachine::State * gameover_state  = nullptr;

	chaos::StateMachine::State * main_menu_to_playing  = nullptr;
	chaos::StateMachine::State * playing_to_main_menu  = nullptr;
	chaos::StateMachine::State * playing_to_pause      = nullptr;
	chaos::StateMachine::State * pause_to_playing      = nullptr;
	chaos::StateMachine::State * playing_to_gameover   = nullptr;
	chaos::StateMachine::State * gameover_to_main_menu = nullptr;
};
