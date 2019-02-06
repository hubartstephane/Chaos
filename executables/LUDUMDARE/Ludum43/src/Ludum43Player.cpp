#include "Ludum43Player.h"
#include "Ludum43Game.h"
#include "Ludum43GameInstance.h"
#include "Ludum43Particles.h"

#include <chaos/MathTools.h>
#include <chaos/ParticleLayer.h>

#include <death/GameLevel.h>

LudumPlayer::LudumPlayer(death::GameInstance * in_game_instance) : 
	death::Player(in_game_instance)
{
}

DEATH_GAMEFRAMEWORK_IMPLEMENT_PLAYER(Ludum);

ParticlePlayer * LudumPlayer::GetPlayerParticle()
{
	if (player_allocations == nullptr)
		return nullptr;
	chaos::ParticleAccessor<ParticlePlayer> player_particles = player_allocations->GetParticleAccessor<ParticlePlayer>();
	if (player_particles.GetCount() == 0)
		return nullptr;
	return &player_particles[0];
}

ParticlePlayer const * LudumPlayer::GetPlayerParticle() const
{
	if (player_allocations == nullptr)
		return nullptr;
	chaos::ParticleConstAccessor<ParticlePlayer> player_particles = player_allocations->GetParticleAccessor<ParticlePlayer>();
	if (player_particles.GetCount() == 0)
		return nullptr;
	return &player_particles[0];
}

void LudumPlayer::TickPlayerDisplacement(double delta_time)
{
	// displace the player
	UpdatePlayerAcceleration(delta_time);
	// dash values update
	TickDashValues(delta_time);
	// cooldown
	TickCooldown(delta_time);
}

void LudumPlayer::UpdatePlayerAcceleration(double delta_time)
{
	LudumGame const * ludum_game = GetLudumGame();
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
			left_stick_position / chaos::MathTools::Sqrt(left_length_2) :
			right_stick_position / chaos::MathTools::Sqrt(right_length_2);

		player_particle->acceleration = ludum_game->player_acceleration * glm::vec2(1.0f, -1.0f) * acceleration; // axis Y reversed
	}
}

void LudumPlayer::TickCooldown(double delta_time)
{
	current_cooldown = chaos::MathTools::Maximum(0.0f, current_cooldown - (float)delta_time);
}

void LudumPlayer::TickDashValues(double delta_time)
{
	// cooldow in progress
	if (current_dash_cooldown > 0.0f)
		current_dash_cooldown = chaos::MathTools::Maximum(0.0f, current_dash_cooldown - (float)delta_time);
	if (current_dash_duration > 0.0f)
	{
		current_dash_duration = chaos::MathTools::Maximum(0.0f, current_dash_duration - (float)delta_time);
		if (current_dash_duration == 0.0f)
			SetDashMode(false);
	}
}

void LudumPlayer::SetReverseMode(bool reversed_mode)
{
	LudumGame * ludum_game = GetLudumGame();
	assert(ludum_game != nullptr);
	assert(game_instance != nullptr);
	assert(game_instance != nullptr);

	death::GameLevelInstance * level_instance = ludum_game->GetLevelInstance();
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
			ludum_game->PlaySound("collision", false, false);
		}
		player_particle->reversed = reversed_mode;
	}
}

void LudumPlayer::SetDashMode(bool dash)
{
	LudumGame * ludum_game = GetLudumGame();
	assert(ludum_game != nullptr);
	assert(game_instance != nullptr);

	death::GameLevelInstance * level_instance = ludum_game->GetLevelInstance();
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
			ludum_game->PlaySound("thrust", false, false);
		}
		player_particle->dash = dash;
	}
}

void LudumPlayer::InternalHandleGamepadInputs(double delta_time, chaos::MyGLFW::GamepadData const * gpd)
{
	death::Player::InternalHandleGamepadInputs(delta_time, gpd);

	if (chaos::Application::GetApplicationInputMode() == chaos::InputMode::Gamepad)
	{
		bool dash = gpd->IsButtonPressed(chaos::MyGLFW::XBOX_BUTTON_A, false);
		SetDashMode(dash);
		bool reversed_mode = gpd->IsButtonPressed(chaos::MyGLFW::XBOX_BUTTON_B, false);
		SetReverseMode(reversed_mode);
	}
}

void LudumPlayer::HandleKeyboardInputs(double delta_time)
{
	death::Player::HandleKeyboardInputs(delta_time);

	if (chaos::Application::GetApplicationInputMode() == chaos::InputMode::Keyboard)
	{
		// get the data
		death::Game * game = GetGame();
		if (game == nullptr)
			return;

		GLFWwindow * glfw_window = game->GetGLFWWindow();
		if (glfw_window == nullptr)
			return;

		// get the mode
		bool dash_mode = (glfwGetKey(glfw_window, GLFW_KEY_SPACE) != GLFW_RELEASE);
		SetDashMode(dash_mode);

		bool reversed_mode = (glfwGetKey(glfw_window, GLFW_KEY_RIGHT_CONTROL) != GLFW_RELEASE);
		SetReverseMode(reversed_mode);
	}
}

void LudumPlayer::OnLevelChanged()
{
	current_cooldown = 0.0f;
	current_dash_cooldown = 0.0f;
	current_dash_duration = 0.0f;
}

void LudumPlayer::SetPlayerAllocation(chaos::ParticleAllocation * in_allocation)
{
	LudumGame * ludum_game = GetLudumGame();
	if (ludum_game == nullptr)
		return;

	Player::SetPlayerAllocation(in_allocation);
	if (in_allocation != nullptr)
	{
		chaos::ParticleAccessor<ParticlePlayer> player_particles = in_allocation->GetParticleAccessor<ParticlePlayer>();
		size_t count = player_particles.GetCount();
		for (size_t i = 0 ; i < count ; ++i)
			player_particles[i].life = ludum_game->initial_player_life;
	}
}
