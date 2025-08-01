#include "Ludum43PCH.h"
#include "Ludum43Player.h"
#include "Ludum43Level.h"
#include "Ludum43LevelInstance.h"
#include "Ludum43Game.h"
#include "Ludum43GameInstance.h"
#include "Ludum43Particles.h"

ParticlePlayer * LudumPlayer::GetPlayerParticle()
{
	if (pawn == nullptr || pawn->GetAllocation() == nullptr || pawn->GetAllocation()->GetParticleCount() == 0)
		return nullptr;

	chaos::ParticleAccessor<ParticlePlayer> player_particles = pawn->GetAllocation()->GetParticleAccessor();
	if (player_particles.GetDataCount() == 0)
		return nullptr;
	return &player_particles[0];
}

ParticlePlayer const * LudumPlayer::GetPlayerParticle() const
{
	if (pawn == nullptr || pawn->GetAllocation() == nullptr || pawn->GetAllocation()->GetParticleCount() == 0)
		return nullptr;

	chaos::ParticleConstAccessor<ParticlePlayer> player_particles = pawn->GetAllocation()->GetParticleAccessor();
	if (player_particles.GetDataCount() == 0)
		return nullptr;
	return &player_particles[0];
}

void LudumPlayer::TickInternal(float delta_time)
{
	chaos::Player::TickInternal(delta_time);

	// dash values update
	TickDashValues(delta_time);
	// cooldown
	TickCooldown(delta_time);
	// read health back from the particle
	GetHealthFromParticle();
}

void LudumPlayer::TickPlayerDisplacement(float delta_time)
{
	// displace the player
	UpdatePlayerAcceleration(delta_time);
}

void LudumPlayer::GetHealthFromParticle()
{
	ParticlePlayer const* player_particle = GetPlayerParticle();
	if (player_particle == nullptr)
		return;
	health = player_particle->life;
}

void LudumPlayer::UpdatePlayerAcceleration(float delta_time)
{
	LudumGame const * ludum_game = GetGame();
	if (ludum_game == nullptr)
		return;

	ParticlePlayer * player_particle = GetPlayerParticle();
	if (player_particle == nullptr)
		return;
	player_particle->acceleration = glm::vec2(0.0f, 0.0f);

	float left_length_2 = glm::length2(left_stick_position);
	float right_length_2 = glm::length2(right_stick_position);
	if (left_length_2 > 0.0f || right_length_2 > 0.0f)
	{
		glm::vec2 acceleration = (left_length_2 > right_length_2) ?
			left_stick_position / std::sqrt(left_length_2) :
			right_stick_position / std::sqrt(right_length_2);

		player_particle->acceleration = ludum_game->player_acceleration * acceleration;
	}
}

void LudumPlayer::TickCooldown(float delta_time)
{
	current_cooldown = std::max(0.0f, current_cooldown - delta_time);
}

void LudumPlayer::TickDashValues(float delta_time)
{
	// cooldow in progress
	if (current_dash_cooldown > 0.0f)
		current_dash_cooldown = std::max(0.0f, current_dash_cooldown - delta_time);
	if (current_dash_duration > 0.0f)
	{
		current_dash_duration = std::max(0.0f, current_dash_duration - delta_time);
		if (current_dash_duration == 0.0f)
			SetDashMode(false);
	}
}

void LudumPlayer::SetReverseMode(bool reversed_mode)
{
	LudumGame * ludum_game = GetGame();
	assert(ludum_game != nullptr);
	assert(game_instance != nullptr);
	assert(game_instance != nullptr);

	chaos::LevelInstance * level_instance = ludum_game->GetLevelInstance();
	if (level_instance == nullptr)
		return;

	if (level_instance->GetLevelClockTime() < 2.0) // because the player start could cause a repulsion
		return;

	ParticlePlayer * player_particle = GetPlayerParticle();
	if (player_particle != nullptr)
	{
		if (player_particle->reversed == reversed_mode) // no change, ignore
			return;

		if (reversed_mode) // only trigger a 'pulse' if no cooldown
		{
			if (current_cooldown > 0.0f)
				return;
			current_cooldown = ludum_game->cooldown;
			ludum_game->PlaySound("collision", false, false, 0.0f, chaos::SoundContext::LEVEL);
		}
		player_particle->reversed = reversed_mode;
	}
}

void LudumPlayer::SetDashMode(bool dash)
{
	LudumGame * ludum_game = GetGame();
	assert(ludum_game != nullptr);
	assert(game_instance != nullptr);

	chaos::LevelInstance * level_instance = ludum_game->GetLevelInstance();
	if (level_instance == nullptr)
		return;

	if (level_instance->GetLevelClockTime() < 2.0)// because the player start dash
		return;

	ParticlePlayer * player_particle = GetPlayerParticle();
	if (player_particle != nullptr)
	{
		if (player_particle->dash == dash) // no change, ignore
			return;

		if (dash) // only trigger a 'pulse' if no cooldown
		{
			if (current_dash_cooldown > 0.0f)
				return;
			current_dash_cooldown = ludum_game->dash_cooldown;
			current_dash_duration = ludum_game->dash_duration;
			ludum_game->PlaySound("thrust", false, false, 0.0f, chaos::SoundContext::LEVEL);
		}
		player_particle->dash = dash;
	}
}

void LudumPlayer::HandleInputs(float delta_time, chaos::GamepadState const * gpd)
{
	chaos::Player::HandleInputs(delta_time, gpd);

	chaos::Key const dash_keys[] = { chaos::KeyboardButton::SPACE, chaos::GamepadButton::A, chaos::Key() };

	bool dash_mode = CheckKeyDown(dash_keys);
	SetDashMode(dash_mode);

	chaos::Key const reverse_mode_keys[] = { chaos::KeyboardButton::RIGHT_CONTROL, chaos::GamepadButton::B, chaos::Key() };

	bool reversed_mode = CheckKeyDown(reverse_mode_keys);
	SetReverseMode(reversed_mode);
}

void LudumPlayer::OnLevelChanged(chaos::Level * new_level, chaos::Level * old_level, chaos::LevelInstance * new_level_instance)
{
	chaos::Player::OnLevelChanged(new_level, old_level, new_level_instance);

	current_cooldown = 0.0f;
	current_dash_cooldown = 0.0f;
	current_dash_duration = 0.0f;
}
