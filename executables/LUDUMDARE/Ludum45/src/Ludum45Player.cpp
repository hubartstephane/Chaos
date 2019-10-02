#include "Ludum45Player.h"
#include "Ludum45Level.h"
#include "Ludum45LevelInstance.h"
#include "Ludum45Game.h"
#include "Ludum45GameInstance.h"
#include "Ludum45Particles.h"
#include "Ludum45GameCheckpoint.h"

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

void LudumPlayer::OnLifeLost()
{
	death::Player::OnLifeLost();



}


void LudumPlayer::OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance)
{
	death::Player::OnLevelChanged(new_level, old_level, new_level_instance);




}

void LudumPlayer::SetPlayerAllocation(chaos::ParticleAllocationBase * in_allocation)
{
	LudumGame * ludum_game = GetLudumGame();
	if (ludum_game == nullptr)
		return;

	Player::SetPlayerAllocation(in_allocation);
}

bool LudumPlayer::CheckButtonPressed(int const * keyboard_buttons, int gamepad_button)
{
	// keyboard input
	if (keyboard_buttons != nullptr)
	{
		death::Game * game = GetGame();
		if (game != nullptr)
		{
			GLFWwindow * glfw_window = game->GetGLFWWindow();
			if (glfw_window != nullptr)
			{
				int i = 0;
				while (keyboard_buttons[i] >= 0)
				{
					if (glfwGetKey(glfw_window, keyboard_buttons[i]) != GLFW_RELEASE)
						return true;					
					++i;
				}
			}
		}
	}

	// gamepad input
	if (gamepad_button >= 0)
		if (gamepad != nullptr && gamepad->IsButtonPressed(gamepad_button))
			return true;	
	return false;
}



death::PlayerCheckpoint * LudumPlayer::DoCreateCheckpoint() const
{
	return new LudumPlayerCheckpoint();
}

bool LudumPlayer::DoLoadFromCheckpoint(death::PlayerCheckpoint const * checkpoint)
{
	LudumPlayerCheckpoint const * ludum_checkpoint = auto_cast(checkpoint);
	if (ludum_checkpoint == nullptr)
		return false;

	if (!death::Player::DoLoadFromCheckpoint(checkpoint))
		return false;





	return true;
}

bool LudumPlayer::DoSaveIntoCheckpoint(death::PlayerCheckpoint * checkpoint) const
{
	LudumPlayerCheckpoint * ludum_checkpoint = auto_cast(checkpoint);
	if (ludum_checkpoint == nullptr)
		return false;

	if (!death::Player::DoSaveIntoCheckpoint(checkpoint))
		return false;



	return true;
}