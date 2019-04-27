#include "Ludum44Player.h"
#include "Ludum44Level.h"
#include "Ludum44LevelInstance.h"
#include "Ludum44Game.h"
#include "Ludum44GameInstance.h"
#include "Ludum44Particles.h"

#include <chaos/MathTools.h>
#include <chaos/ParticleLayer.h>

#include <death/GameLevel.h>

DEATH_GAMEFRAMEWORK_IMPLEMENT_PLAYER(Ludum);

LudumPlayer::LudumPlayer(death::GameInstance * in_game_instance) : 
	death::Player(in_game_instance)
{
}

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
}

void LudumPlayer::UpdatePlayerAcceleration(double delta_time)
{
	LudumGame const * ludum_game = GetLudumGame();
	if (ludum_game == nullptr)
		return;

	ParticlePlayer * player_particle = GetPlayerParticle();
	if (player_particle == nullptr)
		return;
	player_particle->velocity = glm::vec2(0.0f, 0.0f);

	float left_length_2 = glm::length2(left_stick_position);
	float right_length_2 = glm::length2(right_stick_position);
	if (left_length_2 > 0.0f || right_length_2 > 0.0f)
	{
		glm::vec2 speed = (left_length_2 > right_length_2) ?
			left_stick_position / chaos::MathTools::Sqrt(left_length_2) :
			right_stick_position / chaos::MathTools::Sqrt(right_length_2);

		player_particle->velocity = current_player_speed * ludum_game->player_speed_factor * glm::vec2(1.0f, -1.0f) * speed; // axis Y reversed
	}
}


void LudumPlayer::InternalHandleGamepadInputs(double delta_time, chaos::MyGLFW::GamepadData const * gpd)
{
	death::Player::InternalHandleGamepadInputs(delta_time, gpd);

	if (chaos::Application::GetApplicationInputMode() == chaos::InputMode::Gamepad)
	{

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




	}
}

void LudumPlayer::OnLevelChanged()
{


}

void LudumPlayer::SetPlayerAllocation(chaos::ParticleAllocationBase * in_allocation)
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
