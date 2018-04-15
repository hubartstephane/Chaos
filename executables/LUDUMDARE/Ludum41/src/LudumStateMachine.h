#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/StateMachine.h>

// =========================================================
// States
// =========================================================

class LudumState : public chaos::StateMachine::State
{
public:

	/** constructor */
	LudumState(chaos::StateMachine::Automata * in_automata);
	/** get the game */
	class LudumGame * GetGame();
};

class MainMenuState : public LudumState
{
public:

	/** constructor */
	MainMenuState(chaos::StateMachine::Automata * in_automata);
	/** override */
	virtual bool OnEnterImpl(chaos::StateMachine::State * state) override;

};

class PlayingState : public LudumState
{
public:

	/** constructor */
	PlayingState(chaos::StateMachine::Automata * in_automata);

};

class PauseState : public LudumState
{
public:

	/** constructor */
	PauseState(chaos::StateMachine::Automata * in_automata);
};

class GameOverState : public LudumState
{
public:

	/** constructor */
	GameOverState(chaos::StateMachine::Automata * in_automata);
};

// =========================================================
// Transitions
// =========================================================

class LudumTransition : public chaos::StateMachine::Transition
{
public:

	/** constructor */
	LudumTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state);
	/** get the game */
	class LudumGame * GetGame();
};

class MainMenuToPlayingTransition : public LudumTransition
{
public:

	/** constructor */
	MainMenuToPlayingTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state);

protected:

	/** overriding */
	virtual bool OnEnterImpl(chaos::StateMachine::State * from) override;
	/** overriding */
	virtual bool TickImpl(double delta_time) override;
};

class PlayingToMainMenuTransition : public LudumTransition
{

public:

	/** constructor */
	PlayingToMainMenuTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state);

protected:

	/** overriding */
	virtual bool OnEnterImpl(chaos::StateMachine::State * from) override;
	/** overriding */
	virtual bool TickImpl(double delta_time) override;
};

class PlayingToPauseTransition : public LudumTransition
{

public:

	/** constructor */
	PlayingToPauseTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state);

protected:

	/** overriding */
	virtual bool OnEnterImpl(chaos::StateMachine::State * from) override;
	/** overriding */
	virtual bool TickImpl(double delta_time) override;
};


class PauseToPlayingTransition : public LudumTransition
{

public:

	/** constructor */
	PauseToPlayingTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state);

protected:

	/** overriding */
	virtual bool OnEnterImpl(chaos::StateMachine::State * from) override;
	/** overriding */
	virtual bool TickImpl(double delta_time) override;
};

class PlayingToGameOverTransition : public LudumTransition
{

public:

	/** constructor */
	PlayingToGameOverTransition(chaos::StateMachine::State * in_from_state, chaos::StateMachine::State * in_to_state);

};

class GameOverToMainMenuTransition : public LudumTransition
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
	friend class LudumGame;

public:

	// the possible states
	static int const STATE_MAINMENU = 1;
	static int const STATE_PAUSE = 2;
	static int const STATE_PLAYING = 3;
	static int const STATE_GAMEOVER = 4;

	// the possible transition-states
	static int const STATE_TRANSITION_MAINMENU_TO_PLAYING = 5;
	static int const STATE_TRANSITION_PLAYING_TO_PAUSE = 6;
	static int const STATE_TRANSITION_PAUSE_TO_PLAYING = 7;

	/** constructor */
	LudumAutomata(LudumGame * in_game);

	/** get the game */
	LudumGame * GetGame() { return game; }
	/** get the game */
	LudumGame const * GetGame() const { return game; }

protected:

	/** the game */
	LudumGame * game = nullptr;

	/** the states */
	chaos::StateMachine::State * main_menu_state = nullptr;
	chaos::StateMachine::State * playing_state   = nullptr;
	chaos::StateMachine::State * pause_state     = nullptr;
	chaos::StateMachine::State * gameover_state  = nullptr;

	chaos::StateMachine::Transition * main_menu_to_playing  = nullptr;
	chaos::StateMachine::Transition * playing_to_main_menu  = nullptr;
	chaos::StateMachine::Transition * playing_to_pause      = nullptr;
	chaos::StateMachine::Transition * pause_to_playing      = nullptr;
	chaos::StateMachine::Transition * playing_to_gameover   = nullptr;
	chaos::StateMachine::Transition * gameover_to_main_menu = nullptr;
};
