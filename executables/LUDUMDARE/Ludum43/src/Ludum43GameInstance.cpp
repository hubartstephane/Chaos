#include "Ludum43GameInstance.h"
#include "Ludum43Game.h"
#include "Ludum43Player.h"

LudumGameInstance::LudumGameInstance(death::Game * in_game) : 
	death::GameInstance(in_game)
{
	
}

death::Player * LudumGameInstance::DoCreatePlayer()
{
	return new LudumPlayer(this);
}


DEATH_GAMEFRAMEWORK_IMPLEMENT_GAMEINSTANCE(Ludum);

bool LudumGameInstance::CheckGameOverCondition()
{
	LudumGame * ludum_game = GetLudumGame();
	if (ludum_game != nullptr)
	{
		ParticlePlayer const * particle_player = ludum_game->GetPlayerParticle(0);
		if (particle_player == nullptr)
			return true;		
	}
	return false;
}
