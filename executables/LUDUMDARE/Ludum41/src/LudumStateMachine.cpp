#include "LudumStateMachine.h"


LudumAutomata::LudumAutomata(class LudumGame * in_game):
	game(in_game)
{
	assert(in_game != nullptr);

}