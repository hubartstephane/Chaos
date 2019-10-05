#include "Ludum45HUD.h"
#include "Ludum45Game.h"
#include "Ludum45Level.h"
#include "Ludum45Player.h"
#include "Ludum45LevelInstance.h"
#include "Ludum45GameInstance.h"

#include <death/GameHUDComponent.h>

DEATH_GAMEFRAMEWORK_IMPLEMENT_HUD(Ludum);


// ====================================================================
// LudumPlayingHUD
// ====================================================================

bool LudumPlayingHUD::FillHUDContent()
{
	if (!death::PlayingHUD::FillHUDContent())
		return false;	


	//RegisterComponent(death::GameHUDKeys::LIFE_VITAE_ID, new GameHUDLifeBarComponent());
	RegisterComponent(death::GameHUDKeys::LEVEL_TITLE_ID, new death::GameHUDLevelTitleComponent());
	//RegisterComponent(death::GameHUDKeys::POWER_UP_ID, new GameHUDPowerUpComponent());
	//RegisterComponent(death::GameHUDKeys::LIFE_ID, new GameHUDLifeCountComponent());
	//RegisterComponent(death::GameHUDKeys::NOTIFICATION_ID, new death::GameHUDNotificationComponent());

	return true;
}


bool LudumPlayingHUD::CreateHUDLayers()
{
	// call super method
	if (!death::PlayingHUD::CreateHUDLayers())
		return false;
	// create a layer for the life bar
	LudumGame * ludum_game = GetLudumGame();
	if (ludum_game != nullptr)
	{

	}

	return true;
}
