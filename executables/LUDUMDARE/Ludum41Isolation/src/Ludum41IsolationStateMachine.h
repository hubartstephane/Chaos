#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/StateMachine.h>
#include <chaos/SoundManager.h>

#include <death/GameStateMachine.h>

// =========================================================
// StateMachine
// =========================================================

class LudumStateMachine : public death::GameStateMachine
{
	friend class LudumGame;

	CHAOS_OBJECT_DECLARE_CLASS2(LudumStateMachine, death::GameStateMachine);
};
