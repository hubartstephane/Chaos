#include "Ludum47HUD.h"
#include "Ludum47Game.h"
#include "Ludum47Level.h"
#include "Ludum47Player.h"
#include "Ludum47LevelInstance.h"
#include "Ludum47GameInstance.h"

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
