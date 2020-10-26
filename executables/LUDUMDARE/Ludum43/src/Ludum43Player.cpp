#include "Ludum43Player.h"
#include "Ludum43Level.h"
#include "Ludum43LevelInstance.h"
#include "Ludum43Game.h"
#include "Ludum43GameInstance.h"
#include "Ludum43Particles.h"

#include <chaos/Chaos.h>

#include <death/Level.h>
#include <death/SoundContext.h>

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
	death::Player::TickInternal(delta_time);

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

		player_particle->acceleration = ludum_game->player_acceleration * glm::vec2(1.0f, -1.0f) * acceleration; // axis Y reversed
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

	death::LevelInstance * level_instance = ludum_game->GetLevelInstance();
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
			ludum_game->PlaySound("collision", false, false, 0.0f, death::SoundContext::LEVEL);
		}
		player_particle->reversed = reversed_mode;
	}
}

void LudumPlayer::SetDashMode(bool dash)
{
	LudumGame * ludum_game = GetGame();
	assert(ludum_game != nullptr);
	assert(game_instance != nullptr);

	death::LevelInstance * level_instance = ludum_game->GetLevelInstance();
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
			ludum_game->PlaySound("thrust", false, false, 0.0f, death::SoundContext::LEVEL);
		}
		player_particle->dash = dash;
	}
}

void LudumPlayer::InternalHandleGamepadInputs(float delta_time, chaos::MyGLFW::GamepadData const * gpd)
{
	death::Player::InternalHandleGamepadInputs(delta_time, gpd);

	bool dash = gpd->IsButtonPressed(chaos::XBoxButton::BUTTON_A, false);
	SetDashMode(dash);
	bool reversed_mode = gpd->IsButtonPressed(chaos::XBoxButton::BUTTON_B, false);
	SetReverseMode(reversed_mode);
}

void LudumPlayer::HandleKeyboardInputs(float delta_time)
{
	death::Player::HandleKeyboardInputs(delta_time);

	bool dash_mode = CheckKeyPressed(GLFW_KEY_SPACE);
	SetDashMode(dash_mode);
	bool reversed_mode = CheckKeyPressed(GLFW_KEY_RIGHT_CONTROL);
	SetReverseMode(reversed_mode);
}

void LudumPlayer::OnLevelChanged(death::Level * new_level, death::Level * old_level, death::LevelInstance * new_level_instance)
{
	death::Player::OnLevelChanged(new_level, old_level, new_level_instance);

	current_cooldown = 0.0f;
	current_dash_cooldown = 0.0f;
	current_dash_duration = 0.0f;
}
