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

class LudumPlayer * LudumGameInstance::GetLudumPlayer(int player_index)
{
	return dynamic_cast<LudumPlayer*>(GetPlayer(player_index));
}

class LudumPlayer const * LudumGameInstance::GetLudumPlayer(int player_index) const
{
	return dynamic_cast<LudumPlayer const *>(GetPlayer(player_index));
}

class LudumGame * LudumGameInstance::GetLudumGame()
{
	return dynamic_cast<LudumGame*>(GetGame());
}

class LudumGame const * LudumGameInstance::GetLudumGame() const
{
	return dynamic_cast<LudumGame const *>(GetGame());
}


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
