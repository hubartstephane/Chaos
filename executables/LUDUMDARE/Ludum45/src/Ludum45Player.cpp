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
#include <death/SoundContext.h>

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
	// player fire particles
	UpdatePlayerFire(delta_time);



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

void LudumPlayer::UpdatePlayerAcceleration(double delta_time)
{
	float dt = (float)delta_time;

	LudumGame const * ludum_game = GetLudumGame();
	if (ludum_game == nullptr)
		return;

	ParticlePlayer * player_particle = GetPlayerParticle();
	if (player_particle == nullptr)
		return;

	float left_length_2 = glm::length2(left_stick_position);	
	float right_length_2 = glm::length2(right_stick_position);
	if (left_length_2 > 0.0f || right_length_2 > 0.0f)
	{
		glm::vec2 direction = (left_length_2 > right_length_2) ?
			left_stick_position / chaos::MathTools::Sqrt(left_length_2) :
			right_stick_position / chaos::MathTools::Sqrt(right_length_2);

		player_particle->velocity += ludum_game->player_speeds[current_speed_index] * ludum_game->player_acceleration_factor * glm::vec2(1.0f, -1.0f) * direction; // axis Y reversed


		float l = glm::length(player_particle->velocity);
		if (l > ludum_game->player_speed_factor)
			player_particle->velocity *= ludum_game->player_speed_factor / l;
		
		player_particle->orientation = -atan2f(direction.y, direction.x) - (float)M_PI * 0.5f;
	}
	else
		player_particle->velocity *= powf(ludum_game->player_speed_damping, dt);

	// displace the player
	player_particle->bounding_box.position += dt * player_particle->velocity;
}

void LudumPlayer::UpdatePlayerFire(double delta_time)
{
	// decrease normal fire cool down
	fire_timer -= (float)delta_time;
	if (fire_timer < 0.0f)
		fire_timer = 0.0f;

	LudumGame * ludum_game = GetLudumGame();
	if (ludum_game == nullptr)
		return;

	int const fire_key_buttons[] = {GLFW_KEY_SPACE, -1};
	if (fire_timer <= 0.0f)
	{
		bool fire_pressed = CheckButtonPressed(fire_key_buttons, chaos::MyGLFW::XBOX_BUTTON_A);
		if (fire_pressed)
		{
			FireProjectile();					
			fire_timer = ludum_game->player_fire_cooldowns[current_fire_cooldown_index];
		}								
	}			
}

ParticleFire * LudumPlayer::FireProjectile()
{
	LudumGame const * ludum_game = GetLudumGame();
	if (ludum_game == nullptr)
		return nullptr;

	int count = ludum_game->player_fire_rates[current_fire_rate_index]; 
	ParticleFire * p = FireProjectile(fire_bitmap_layout, 0.3f, count, "fire", 0.1f, ludum_game->fire_velocity);
	if (p != nullptr)
	{
		for (int i = 0 ; i < count ; ++i)
		{
			p[i].damage = ludum_game->player_damages[current_damage_index];
			p[i].trample = false;
		}
	}
	return p;
}

ParticleFire * LudumPlayer::FireProjectile(chaos::BitmapAtlas::BitmapLayout const & layout, float ratio_to_player, int count, char const * sound_name, float delta_rotation, float velocity)
{
	ParticlePlayer const * player_particle = GetPlayerParticle();
	if (player_particle == nullptr)
		return nullptr;

	return GetLudumGameInstance()->FireProjectile(fire_allocation.get(), GetPlayerBox(), layout, ratio_to_player, count, sound_name, delta_rotation, true, velocity, player_particle->orientation);
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

void LudumPlayer::OnDamagedReceived(float damage)
{
	//SetLifeBarValue(-particle->damage, true);


	GetGame()->Play("player_touched", false, false, 0.0f, death::SoundContext::LEVEL);
}

void LudumPlayer::RegisterUpgrades()
{

}




void LudumPlayer::SetPlayerAllocation(chaos::ParticleAllocationBase * in_allocation)
{
	LudumGame * ludum_game = GetLudumGame();
	if (ludum_game == nullptr)
		return;

	Player::SetPlayerAllocation(in_allocation);
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
