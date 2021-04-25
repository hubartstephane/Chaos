#include <chaos/Chaos.h>

#include "Ludum48HUD.h"
#include "Ludum48Game.h"
#include "Ludum48Level.h"
#include "Ludum48Player.h"
#include "Ludum48LevelInstance.h"
#include "Ludum48GameInstance.h"

// ====================================================================
// LudumPlayingHUD
// ====================================================================

bool LudumPlayingHUD::FillHUDContent()
{
	if (!chaos::PlayingHUD::FillHUDContent())
		return false;	
	RegisterComponent(chaos::GameHUDKeys::LEVEL_TIMEOUT_ID, new chaos::GameHUDTimeoutComponent());
	return true;
}