#include "LudumTestGameInstance.h"
#include "LudumTestGame.h"
#include "LudumTestLevel.h"
#include "LudumTestLevelInstance.h"
#include "LudumTestPlayer.h"

#include <death/SoundContext.h>

LudumGameInstance::LudumGameInstance(death::Game * in_game) : 
	death::GameInstance(in_game)
{
	
}

death::Player * LudumGameInstance::DoCreatePlayer()
{
	return new LudumPlayer(this);
}

bool LudumGameInstance::DoCheckGameOverCondition() // shuludum : mututaliser le code
{
	LudumPlayer * ludum_player = GetPlayer(0);
	if (ludum_player != nullptr)
	{


	}
	return false;
}


void LudumGameInstance::OnLevelChanged(death::Level * new_level, death::Level * old_level, death::LevelInstance * new_level_instance)
{
	death::GameInstance::OnLevelChanged(new_level, old_level, new_level_instance);
	// play a sound
	if (new_level != nullptr && old_level != nullptr)
		game->PlaySound("next_level", false, false, 0.0f, death::SoundContext::GAME);
}
