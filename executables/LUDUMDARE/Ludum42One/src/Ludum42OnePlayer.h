#pragma once

#include <chaos/StandardHeaders.h> 
#include <death/Game.h>
#include <death/GameInstance.h>
#include <death/Player.h>

// =========================================================
// LudumPlayer
// =========================================================

class LudumPlayer : public death::Player
{

public:

	/** constructor */
	LudumPlayer(death::GameInstance * in_game_instance);
};
