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
	// fire 
	UpdatePlayerFire(delta_time);
	// buy items
	UpdatePlayerBuyingItem(delta_time);
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

		player_particle->velocity = current_speed * ludum_game->player_speed_factor * glm::vec2(1.0f, -1.0f) * speed; // axis Y reversed
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

#if 0
	if (in_allocation != nullptr)
	{
		chaos::ParticleAccessor<ParticlePlayer> player_particles = in_allocation->GetParticleAccessor<ParticlePlayer>();
		size_t count = player_particles.GetCount();
		for (size_t i = 0 ; i < count ; ++i)
			player_particles[i].life = ludum_game->player_life.initial_value;
	}
#endif
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

ParticleFire * LudumPlayer::FireProjectile(chaos::BitmapAtlas::BitmapLayout const & layout, char const * sound_name)
{
	if (fire_allocation == nullptr)
		return nullptr;
	if (!fire_allocation->AddParticles(1))
		return nullptr;
	chaos::ParticleAccessor<ParticleFire> particles = fire_allocation->GetParticleAccessor<ParticleFire>();
	if (particles.GetCount() == 0)
		return nullptr;

	chaos::box2 box = GetPlayerBox();

	ParticleFire & p = particles[particles.GetCount() - 1];

	p.bounding_box.position = box.position;
	p.bounding_box.half_size = box.half_size;
	p.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	LudumGame * ludum_game = GetLudumGame();

	if (sound_name != nullptr)	
		ludum_game->PlaySound(sound_name, false, false);

	p.texcoords = chaos::ParticleTools::GetParticleTexcoords(layout, ludum_game->GetTextureAtlas()->GetAtlasDimension());

	p.velocity = glm::vec2(ludum_game->fire_velocity, 0.0f);
	p.player_owner_ship = true;

	return &p;
}

ParticleFire * LudumPlayer::FireChargedProjectile()
{
	ParticleFire * p = FireProjectile(charged_fire_bitmap_layout, "charged_fire");
	if (p == nullptr)
	{
		p->damage = current_charged_damage;
		p->trample = false;
	}
	return p;
}

ParticleFire * LudumPlayer::FireNormalProjectile()
{
	ParticleFire * p = FireProjectile(fire_bitmap_layout, "fire");
	if (p == nullptr)
	{
		p->damage = current_damage;
		p->trample = true;
	}
	return p;
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
	int const charged_key_buttons[] = {GLFW_KEY_LEFT_CONTROL, GLFW_KEY_RIGHT_CONTROL, -1};

	bool charged_pressed = CheckButtonPressed(charged_key_buttons, chaos::MyGLFW::XBOX_BUTTON_B);
	if (charged_pressed)
	{
		charged_fire_timer += (float)delta_time;
		if (charged_fire_timer >= ludum_game->charged_fire_time)
			charged_fire_timer = ludum_game->charged_fire_time;	
	}
	else
	{
		if (charged_fire_timer >= ludum_game->charged_fire_time) // charged fire is only fired when button is UP
		{
			FireChargedProjectile();		
			charged_fire_timer = 0.0f;
		}
		else
		{
			charged_fire_timer = 0.0f;
			if (fire_timer == 0.0f)
			{
				bool fire_pressed = CheckButtonPressed(fire_key_buttons, chaos::MyGLFW::XBOX_BUTTON_A);
				if (fire_pressed)
				{
					FireNormalProjectile();					
					fire_timer = (1.0f / current_fire_rate);
				}								
			}			
		}
	}
}

void LudumPlayer::UpdatePlayerBuyingItem(double delta_time)
{
	LudumGame * ludum_game = GetLudumGame();
	if (ludum_game == nullptr)
		return;

	LudumGameInstance * ludum_game_instance = GetLudumGameInstance();
	if (ludum_game_instance == nullptr || ludum_game_instance->current_power_up == nullptr || ludum_game_instance->current_power_up_surface == nullptr)
		return;
	if (!ludum_game_instance->current_power_up->CanPowerUp(GetLudumGame(), this))
		return;

	int const buy_key_buttons[] = {GLFW_KEY_LEFT_ALT, GLFW_KEY_RIGHT_ALT, -1};

	bool buy_pressed = CheckButtonPressed(buy_key_buttons, chaos::MyGLFW::XBOX_BUTTON_Y);
	if (buy_pressed)
	{
		buy_timer += (float)delta_time;
		if (buy_timer >= ludum_game->buy_upgrade_time)
		{
			// XXX : HACK : destroy all power up zone in the camera view : not the best but should work if a single zone in the same time
			LudumLevelInstance * ludum_level_instance = GetLudumLevelInstance();
			if (ludum_level_instance != nullptr)
			{
				death::TiledMap::LayerInstance * layer_instance = ludum_level_instance->FindLayerInstance("Zones");
				if (layer_instance != nullptr)
				{
					layer_instance->FindTileCollisions(ludum_level_instance->GetCameraBox(), [](death::TiledMap::TileParticle & particle)
					{
						particle.gid = 0;
						return true;
					});			
				}
			}

			// reset the corresponding trigger surface
			ludum_game_instance->current_power_up->ApplyPowerUp(GetLudumGame(), this);			
			ludum_game_instance->current_power_up_surface->SetEnabled(false);
			ludum_game_instance->current_power_up = nullptr;
			ludum_game_instance->current_power_up_surface = nullptr;
			buy_timer = 0.0f;
		}	
	}
	else
		buy_timer = 0.0f;
}
