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

protected:

	/** override */
	virtual void TickPlayerDisplacement(double delta_time) override;
	/** override */
	virtual bool OnMouseMove(double x, double y) override;
};
