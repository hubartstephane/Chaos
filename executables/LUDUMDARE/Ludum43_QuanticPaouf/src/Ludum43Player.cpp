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

void LudumPlayer::GetHealthFromParticle()
{
	ParticlePlayer const* player_particle = GetPlayerParticle();
	if (player_particle == nullptr)
		return;
	health = player_particle->life;
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

bool LudumPlayer::EnumerateInputActions(chaos::InputActionProcessor& in_action_processor, chaos::EnumerateInputActionContext in_context)
{
	if (GetGame() != nullptr && !GetGame()->IsPaused())
	{
		auto DashInput = chaos::CombinedInput(chaos::Key::Space, chaos::Key::GamepadA);

		bool dash = false;
		if (in_action_processor.CheckAndProcess(chaos::QueryInput(DashInput, &dash), "Dash", [&]()
		{
			SetDashMode(dash);
		}))
		{
			return true;
		}

		bool reverse = false;
		auto ReverseInput = chaos::CombinedInput(chaos::Key::RightControl, chaos::Key::GamepadB);

		if (in_action_processor.CheckAndProcess(chaos::QueryInput(ReverseInput, &reverse), "Reverse Mode", [&]()
		{
			SetReverseMode(reverse);
			
		}))
		{
			return true;
		}

		if (LudumGame const* ludum_game = GetGame())
		{
			if (ParticlePlayer* player_particle = GetPlayerParticle())
			{
				auto Input = chaos::CombinedInput(
					chaos::MappedInput2D::keyboard_arrows,
					chaos::Input2D::GamepadLeftStick,
					chaos::Input2D::GamepadRightStick,
					chaos::MappedInput2D::gamepad_dpad
				);

				glm::vec2 stick_value = { 0.0f, 0.0f };

				if (in_action_processor.CheckAndProcess(QueryInput(Input, &stick_value), "Move", [&]()
				{
					player_particle->acceleration = ludum_game->player_acceleration * stick_value;
				}))
				{
					return true;
				}
			}
		}
	}

	return chaos::Player::EnumerateInputActions(in_action_processor, in_context);
}
void LudumPlayer::OnLevelChanged(chaos::Level * new_level, chaos::Level * old_level, chaos::LevelInstance * new_level_instance)
{
	chaos::Player::OnLevelChanged(new_level, old_level, new_level_instance);

	current_cooldown = 0.0f;
	current_dash_cooldown = 0.0f;
	current_dash_duration = 0.0f;
}
