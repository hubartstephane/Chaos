#include <chaos/Chaos.h>

#include "Ludum46HUD.h"
#include "Ludum46Game.h"
#include "Ludum46Level.h"
#include "Ludum46Player.h"
#include "Ludum46LevelInstance.h"
#include "Ludum46GameInstance.h"

namespace chaos
{
	namespace GameHUDKeys
	{

	};
};

// ====================================================================
// LudumPlayingHUD
// ====================================================================

bool LudumPlayingHUD::FillHUDContent()
{
	if (!chaos::PlayingHUD::FillHUDContent())
		return false;	
   // RegisterComponent(chaos::GameHUDKeys::LEVEL_TITLE_ID, new chaos::GameHUDLevelTitleComponent());

	return true;
}