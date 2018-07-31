#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/StateMachine.h>
#include <chaos/SoundManager.h>

#include <death/GameAutomata.h>

// =========================================================
// Automata
// =========================================================

class LudumAutomata : public death::GameAutomata
{
	friend class LudumGame;

public:

	/** constructor */
	LudumAutomata(LudumGame * in_game);
};
