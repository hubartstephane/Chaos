#include <chaos/Chaos.h>

#include "Ludum49HUD.h"
#include "Ludum49Game.h"
#include "Ludum49Level.h"
#include "Ludum49Player.h"
#include "Ludum49LevelInstance.h"
#include "Ludum49GameInstance.h"


// ====================================================================
// LudumPlayingHUD
// ====================================================================

bool LudumPlayingHUD::FillHUDContent()
{
	if (!PlayingHUD::FillHUDContent())
		return false;	



	return true;
}