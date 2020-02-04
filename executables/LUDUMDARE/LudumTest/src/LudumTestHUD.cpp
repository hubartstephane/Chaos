#include "LudumTestHUD.h"
#include "LudumTestGame.h"
#include "LudumTestLevel.h"
#include "LudumTestPlayer.h"
#include "LudumTestLevelInstance.h"
#include "LudumTestGameInstance.h"

#include <death/GameHUDComponent.h>

DEATH_GAMEFRAMEWORK_IMPLEMENT_HUD(Ludum);


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
    RegisterComponent(death::GameHUDKeys::LEVEL_TITLE_ID, new death::GameHUDLevelTitleComponent());

	return true;
}


bool LudumPlayingHUD::CreateHUDLayers()
{
	// call super method
	if (!death::PlayingHUD::CreateHUDLayers())
		return false;




	return true;
}
