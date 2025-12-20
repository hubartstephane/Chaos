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

bool LudumPlayer::EnumerateInputActions(chaos::InputActionEnumerator& in_action_enumerator, chaos::EnumerateInputActionContext in_context)
{
	if (GetGame() != nullptr && !GetGame()->IsPaused())
	{
		bool dash_key1 = false;
		bool dash_key2 = false;
		auto DashRequest = Or(QueryInput(chaos::Key::SPACE, &dash_key1), QueryInput(chaos::Key::GAMEPAD_A, &dash_key2));

		if (in_action_enumerator.CheckAndProcess(DashRequest, "Dash", [&]()
		{
			SetDashMode(dash_key1 || dash_key2);
		}))
		{
			return true;
		}

		bool reverse_mode_key1 = false;
		bool reverse_mode_key2 = false;
		auto ReverseModeRequest = Or(QueryInput(chaos::Key::RIGHT_CONTROL, &reverse_mode_key1), QueryInput(chaos::Key::GAMEPAD_B, &reverse_mode_key2));

		if (in_action_enumerator.CheckAndProcess(ReverseModeRequest, "Reverse Mode", [&]()
		{
			SetReverseMode(reverse_mode_key1 || reverse_mode_key2);
			
		}))
		{
			return true;
		}

		if (LudumGame const* ludum_game = GetGame())
		{
			if (ParticlePlayer* player_particle = GetPlayerParticle())
			{
				glm::vec2 left_stick_value  = { 0.0f, 0.0f };
				glm::vec2 right_stick_value = { 0.0f, 0.0f };
				bool      up_value1    = false;
				bool      up_value2    = false;
				bool      down_value1  = false;
				bool      down_value2  = false;
				bool      left_value1  = false;
				bool      left_value2  = false;
				bool      right_value1 = false;
				bool      right_value2 = false;

				auto StickRequest = Or(
					QueryInput(chaos::Input2D::GAMEPAD_LEFT_STICK, &left_stick_value),
					QueryInput(chaos::Input2D::GAMEPAD_RIGHT_STICK, &right_stick_value),

					QueryInput(chaos::Key::GAMEPAD_DPAD_LEFT,  &left_value1),
					QueryInput(chaos::Key::GAMEPAD_DPAD_RIGHT, &right_value1),
					QueryInput(chaos::Key::GAMEPAD_DPAD_UP,    &up_value1),
					QueryInput(chaos::Key::GAMEPAD_DPAD_DOWN,  &down_value1),

					QueryInput(chaos::Key::LEFT, &left_value2),
					QueryInput(chaos::Key::RIGHT, &right_value2),
					QueryInput(chaos::Key::UP, &up_value2),
					QueryInput(chaos::Key::DOWN, &down_value2)
				);

				if (in_action_enumerator.CheckAndProcess(StickRequest, "Move", [&]()
				{	
					glm::vec2 stick_value = { 0.0f, 0.0f };

					bool left  = left_value1  || left_value2;
					bool right = right_value1 || right_value2;
					bool up    = up_value1    || up_value2;
					bool down  = down_value1  || down_value2;
			
					if (left && !right)
						stick_value.x = -1.0f;
					else if (!left && right)
						stick_value.x = +1.0f;

					if (up && !down)
						stick_value.y = 1.0f;
					else if (!up && down)
						stick_value.y = -1.0f;

					if (stick_value == glm::vec2(0.0f, 0.0f))
					{
						float left_length_2  = glm::length2(left_stick_value);
						float right_length_2 = glm::length2(right_stick_value);
						if (left_length_2 > right_length_2)
							stick_value = left_stick_value / std::sqrt(left_length_2);
						else if (right_length_2 > left_length_2)
							stick_value = right_stick_value / std::sqrt(right_length_2);
						else if (right_length_2 != 0.0f)
							stick_value = right_stick_value / std::sqrt(right_length_2);
					}

					player_particle->acceleration = ludum_game->player_acceleration * stick_value;
				}))
				{
					return true;
				}
			}
		}
	}

	return chaos::Player::EnumerateInputActions(in_action_enumerator, in_context);
}
void LudumPlayer::OnLevelChanged(chaos::Level * new_level, chaos::Level * old_level, chaos::LevelInstance * new_level_instance)
{
	chaos::Player::OnLevelChanged(new_level, old_level, new_level_instance);

	current_cooldown = 0.0f;
	current_dash_cooldown = 0.0f;
	current_dash_duration = 0.0f;
}
