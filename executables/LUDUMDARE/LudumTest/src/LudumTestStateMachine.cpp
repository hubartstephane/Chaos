#include "LudumTestStateMachine.h"
#include "LudumTestGame.h"

LudumStateMachine::LudumStateMachine(class LudumGame * in_game) : 
	death::GameStateMachine(in_game)
{
}