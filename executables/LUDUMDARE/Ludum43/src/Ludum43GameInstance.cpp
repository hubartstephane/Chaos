#include "Ludum43PCH.h"
#include "Ludum43GameInstance.h"
#include "Ludum43Game.h"
#include "Ludum43Level.h"
#include "Ludum43LevelInstance.h"
#include "Ludum43Player.h"


LudumGameInstance::LudumGameInstance()
{
	player_class = LudumPlayer::GetStaticClass();
}

bool LudumGameInstance::DoTick(float delta_time)
{
	chaos::GameInstance::DoTick(delta_time);
	if (game->IsPlaying())
		TickHeartBeat(delta_time);
	return true;
}

void LudumGameInstance::TickHeartBeat(float delta_time)
{
	LudumPlayer* ludum_player = GetPlayer(0);
	if (ludum_player == nullptr)
		return;

	ParticlePlayer const * particle_player = ludum_player->GetPlayerParticle();
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
		game->PlaySound("heartbeat", false, false, 0.0f, chaos::SoundContext::LEVEL);
	}
}


void LudumGameInstance::OnLevelChanged(chaos::Level * new_level, chaos::Level * old_level, chaos::LevelInstance * new_level_instance)
{
	// super method
	chaos::GameInstance::OnLevelChanged(new_level, old_level, new_level_instance);
	// play a sound
	if (new_level != nullptr && old_level != nullptr)
		game->PlaySound("next_level", false, false, 0.0f, chaos::SoundContext::GAME);
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
		game->PlaySound("particle_add", false, false, 0.0f, chaos::SoundContext::LEVEL);
	else if (delta < 0)
		game->PlaySound("particle_removed", false, false, 0.0f, chaos::SoundContext::LEVEL);
	waken_up_particle_count += delta;
}