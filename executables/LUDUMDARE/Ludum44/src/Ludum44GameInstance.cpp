#include "Ludum44GameInstance.h"
#include "Ludum44Game.h"
#include "Ludum44Level.h"
#include "Ludum44LevelInstance.h"
#include "Ludum44Player.h"

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
	LudumGame * ludum_game = GetLudumGame();
	if (ludum_game != nullptr)
	{
		ParticlePlayer const * particle_player = ludum_game->GetPlayerParticle(0);
		if (particle_player == nullptr)
			return true;		
	}
	return death::GameInstance::DoCheckGameOverCondition();
}

bool LudumGameInstance::DoTick(double delta_time)
{
	death::GameInstance::DoTick(delta_time);


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

	LudumPlayer * ludum_player = dynamic_cast<LudumPlayer *>(player);
	if (ludum_player == nullptr)
		return;

	ludum_player->current_life = ludum_game->player_life.initial_value * 0.66f; // shuxxx
	ludum_player->current_max_life = ludum_game->player_life.initial_value;

	ludum_player->current_speed = ludum_game->player_speed.initial_value;
	ludum_player->current_damage = ludum_game->player_damage.initial_value;
	ludum_player->current_fire_rate = ludum_game->player_fire_rate.initial_value;








	if (!ludum_game->power_ups.empty())
		current_power_up = ludum_game->power_ups[0];


}

