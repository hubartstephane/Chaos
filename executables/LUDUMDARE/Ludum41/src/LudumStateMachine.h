#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/StateMachine.h>

// =========================================================
// States
// =========================================================

class MainMenuState : public chaos::StateMachine::State
{

};

class PlayingState : public chaos::StateMachine::State
{

};

class PauseState : public chaos::StateMachine::State
{

};

class GameOverState : public chaos::StateMachine::State
{

};

// =========================================================
// Transitions
// =========================================================

class MainMenuToPlayingTransition : public chaos::StateMachine::Transition
{

};

class PlayingToMainMenuTransition : public chaos::StateMachine::Transition
{

};

class PlayingToPauseTransition : public chaos::StateMachine::Transition
{

};


class PauseToPlayingTransition : public chaos::StateMachine::Transition
{

};

class PlayingToGameOverTransition : public chaos::StateMachine::Transition
{

};

class GameOverToMainMenuTransition : public chaos::StateMachine::Transition
{

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
};
