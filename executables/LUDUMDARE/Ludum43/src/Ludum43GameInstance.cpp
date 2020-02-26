#include "Ludum43GameInstance.h"
#include "Ludum43Game.h"
#include "Ludum43Level.h"
#include "Ludum43LevelInstance.h"
#include "Ludum43Player.h"

#include <death/SoundContext.h>

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
	LudumGame * ludum_game = GetGame();
	if (ludum_game != nullptr)
	{
		ParticlePlayer const * particle_player = ludum_game->GetPlayerParticle(0);
		if (particle_player == nullptr)
			return true;		
	}
	return death::GameInstance::DoCheckGameOverCondition();
}

bool LudumGameInstance::DoTick(float delta_time)
{
	death::GameInstance::DoTick(delta_time);
	if (game->IsPlaying())
		TickHeartBeat(delta_time);
	return true;
}

void LudumGameInstance::TickHeartBeat(float delta_time)
{
	LudumGame * ludum_game = GetGame();
	if (ludum_game == nullptr)
		return;

	ParticlePlayer const * particle_player = ludum_game->GetPlayerParticle(0);
	if (particle_player == nullptr)
		return;

	float limit_value = 1.4f;

	if (particle_player->life < previous_frame_life)
		limit_value = 0.4f;
	previous_frame_life = particle_player->life;

	heart_beat_time += delta_time;
	if (heart_beat_time >= limit_value)
	{
		heart_beat_time = 0.0f;
		game->PlaySound("heartbeat", false, false, 0.0f, death::SoundContext::LEVEL);
	}
}


void LudumGameInstance::OnLevelChanged(death::Level * new_level, death::Level * old_level, death::LevelInstance * new_level_instance)
{
	// super method
	death::GameInstance::OnLevelChanged(new_level, old_level, new_level_instance);

	// internal
	LudumPlayer * player = GetPlayer(0);
	if (player != nullptr)
	{
		player->SetScore(waken_up_particle_count, true);
		player->OnLevelChanged(new_level, old_level, new_level_instance);
	}
	previous_frame_life = 0.0f;
	waken_up_particle_count = 0;
}

void LudumGameInstance::NotifyAtomCountChange(int delta)
{
	if (delta > 0)
		game->PlaySound("particle_add", false, false, 0.0f, death::SoundContext::LEVEL);
	else if (delta < 0)
		game->PlaySound("particle_removed", false, false, 0.0f, death::SoundContext::LEVEL);
	waken_up_particle_count += delta;
}