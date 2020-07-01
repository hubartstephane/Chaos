#include "Ludum46HUD.h"
#include "Ludum46Game.h"
#include "Ludum46Level.h"
#include "Ludum46Player.h"
#include "Ludum46LevelInstance.h"
#include "Ludum46GameInstance.h"

#include <death/GameHUDComponent.h>

namespace death
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
	if (!death::PlayingHUD::FillHUDContent())
		return false;	
   // RegisterComponent(death::GameHUDKeys::LEVEL_TITLE_ID, new death::GameHUDLevelTitleComponent());

	return true;
}


int LudumPlayingHUD::CreateHUDLayers()
{
	// call super method
	int render_order = death::PlayingHUD::CreateHUDLayers();
	if (render_order < 0)
		return render_order;




	return render_order;
}
