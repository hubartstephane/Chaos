#include "Ludum43GameInstance.h"
#include "Ludum43Game.h"
#include "Ludum43Level.h"
#include "Ludum43LevelInstance.h"
#include "Ludum43Player.h"

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
	if (game->IsPlaying())
		TickHeartBeat(delta_time);
	return true;
}

void LudumGameInstance::TickHeartBeat(double delta_time)
{
	LudumGame * ludum_game = GetLudumGame();
	if (ludum_game == nullptr)
		return;

	ParticlePlayer const * particle_player = ludum_game->GetPlayerParticle(0);
	if (particle_player == nullptr)
		return;

	float limit_value = 1.4f;

	if (particle_player->life < previous_frame_life)
		limit_value = 0.4f;
	previous_frame_life = particle_player->life;

	heart_beat_time += (float)delta_time;
	if (heart_beat_time >= limit_value)
	{
		heart_beat_time = 0.0f;
		game->Play("heartbeat", false, false);
	}
}


void LudumGameInstance::OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance)
{
	// super method
	death::GameInstance::OnLevelChanged(new_level, old_level, new_level_instance);

	// internal
	LudumPlayer * player = GetLudumPlayer(0);
	if (player != nullptr)
	{
		player->SetScore(waken_up_particle_count, true);
		player->OnLevelChanged();
	}
	previous_frame_life = 0.0f;
	waken_up_particle_count = 0;
}

void LudumGameInstance::NotifyAtomCountChange(int delta)
{
	if (delta > 0)
		game->Play("particle_add", false, false);
	else if (delta < 0)
		game->Play("particle_removed", false, false);
	waken_up_particle_count += delta;
}