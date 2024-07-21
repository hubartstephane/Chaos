#include "Ludum46PCH.h"
#include "Ludum46HUD.h"
#include "Ludum46Game.h"
#include "Ludum46Level.h"
#include "Ludum46Player.h"
#include "Ludum46LevelInstance.h"
#include "Ludum46GameInstance.h"

// ====================================================================
// LudumPlayingHUD
// ====================================================================

bool LudumPlayingHUD::FillHUDContent()
{
	if (!chaos::PlayingHUD::FillHUDContent())
		return false;
	return true;
}