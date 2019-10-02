#include "Ludum45GameInstance.h"
#include "Ludum45Game.h"
#include "Ludum45Level.h"
#include "Ludum45LevelInstance.h"
#include "Ludum45Player.h"

#include <death/SoundContext.h>

DEATH_GAMEFRAMEWORK_IMPLEMENT_GAMEINSTANCE(Ludum);

LudumGameInstance::LudumGameInstance(death::Game * in_game) : 
	death::GameInstance(in_game)
{
	
}

death::Player * LudumGameInstance::DoCreatePlayer()
{
	return new LudumPlayer(this);
}

bool LudumGameInstance::DoCheckGameOverCondition()
{
	LudumPlayer * ludum_player = GetLudumPlayer(0);
	if (ludum_player != nullptr)
	{




	}
	return false;
}

bool LudumGameInstance::DoSaveIntoCheckpoint(death::GameCheckpoint * checkpoint) const
{
	if (!death::GameInstance::DoSaveIntoCheckpoint(checkpoint))
		return false;




	return true;
}

bool LudumGameInstance::DoLoadFromCheckpoint(death::GameCheckpoint const * checkpoint)
{
	if (!death::GameInstance::DoLoadFromCheckpoint(checkpoint))
		return false;





	return true;
}


















void LudumGameInstance::OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance)
{
	// super method
	death::GameInstance::OnLevelChanged(new_level, old_level, new_level_instance);


}

void LudumGameInstance::OnPlayerEntered(death::Player * player)
{
	death::GameInstance::OnPlayerEntered(player);

	LudumGame * ludum_game = GetLudumGame();
	if (ludum_game == nullptr)
		return;

	LudumPlayer * ludum_player = auto_cast(player);
	if (ludum_player == nullptr)
		return;



}
