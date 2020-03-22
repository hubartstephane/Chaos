#include "Ludum44Player.h"
#include "Ludum44Level.h"
#include "Ludum44LevelInstance.h"
#include "Ludum44Game.h"
#include "Ludum44GameInstance.h"
#include "Ludum44Particles.h"
#include "Ludum44GameCheckpoint.h"

#include <chaos/MathTools.h>
#include <chaos/ParticleLayer.h>

#include <death/Level.h>

LudumPlayer::LudumPlayer(death::GameInstance * in_game_instance) : 
	death::Player(in_game_instance)
{
}

ParticlePlayer * LudumPlayer::GetPlayerParticle()
{
	if (player_allocations == nullptr)
		return nullptr;
	chaos::ParticleAccessor<ParticlePlayer> player_particles = player_allocations->GetParticleAccessor();
	if (player_particles.GetCount() == 0)
		return nullptr;
	return &player_particles[0];
}

ParticlePlayer const * LudumPlayer::GetPlayerParticle() const
{
	if (player_allocations == nullptr)
		return nullptr;
	chaos::ParticleConstAccessor<ParticlePlayer> player_particles = player_allocations->GetParticleAccessor();
	if (player_particles.GetCount() == 0)
		return nullptr;
	return &player_particles[0];
}

void LudumPlayer::TickPlayerDisplacement(float delta_time)
{
	// displace the player
	UpdatePlayerAcceleration(delta_time);
	// fire 
	UpdatePlayerFire(delta_time);
	// buy items
	UpdatePlayerBuyingItem(delta_time);
}

void LudumPlayer::UpdatePlayerAcceleration(float delta_time)
{
	LudumGame const * ludum_game = GetGame();
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
			left_stick_position / std::sqrt(left_length_2) :
			right_stick_position / std::sqrt(right_length_2);

		player_particle->velocity = ludum_game->player_speeds[current_speed_index] * ludum_game->player_speed_factor * glm::vec2(1.0f, -1.0f) * speed; // axis Y reversed
	}
}

void LudumPlayer::SetPlayerAllocation(chaos::ParticleAllocationBase * in_allocation)
{
	LudumGame * ludum_game = GetGame();
	if (ludum_game == nullptr)
		return;

	Player::SetPlayerAllocation(in_allocation);

#if 0
	if (in_allocation != nullptr)
	{
		chaos::ParticleAccessor<ParticlePlayer> player_particles = in_allocation->GetParticleAccessor();
		size_t count = player_particles.GetCount();
		for (size_t i = 0 ; i < count ; ++i)
			player_particles[i].life = ludum_game->player_life.initial_value;
	}
#endif
}

void LudumPlayer::FireChargedProjectile()
{
    LudumGame const* ludum_game = GetGame();
    if (ludum_game == nullptr)
        return;
    LudumGameInstance* ludum_game_instance = GetGameInstance();
    if (ludum_game_instance == nullptr)
        return;

    int count = 1;
    ludum_game_instance->FireProjectile("charged_fire", GetPlayerBox(), 1.0f, count, "thrust", 0.0f, 0.1f, ludum_game->fire_velocity, ludum_game->player_charged_damages[current_charged_damage_index], true, true);
}

void LudumPlayer::FireNormalProjectile()
{
    LudumGame const* ludum_game = GetGame();
    if (ludum_game == nullptr)
        return;
    LudumGameInstance* ludum_game_instance = GetGameInstance();
    if (ludum_game_instance == nullptr)
        return;

    int count = ludum_game->player_fire_rates[current_fire_rate_index];
    ludum_game_instance->FireProjectile("fire", GetPlayerBox(), 0.3f, count, "fire", 0.0f, 0.1f, ludum_game->fire_velocity, ludum_game->player_damages[current_damage_index], false, true);
}

void LudumPlayer::UpdatePlayerFire(float delta_time)
{
	// decrease normal fire cool down
	fire_timer -= delta_time;
	if (fire_timer < 0.0f)
		fire_timer = 0.0f;
	
	LudumGame * ludum_game = GetGame();
	if (ludum_game == nullptr)
		return;

	int const fire_key_buttons[] = {GLFW_KEY_SPACE, -1};
	int const charged_key_buttons[] = {GLFW_KEY_LEFT_CONTROL, GLFW_KEY_RIGHT_CONTROL, -1};

	bool charged_pressed = CheckButtonPressed(charged_key_buttons, chaos::XBoxButton::BUTTON_B);
	if (charged_pressed)
	{
		charged_fire_timer += delta_time;
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
				bool fire_pressed = CheckButtonPressed(fire_key_buttons, chaos::XBoxButton::BUTTON_A);
				if (fire_pressed)
				{
					FireNormalProjectile();					
					fire_timer = ludum_game->normal_fire_time;
				}								
			}			
		}
	}
}

void LudumPlayer::UpdatePlayerBuyingItem(float delta_time)
{
	LudumGame * ludum_game = GetGame();
	if (ludum_game == nullptr)
		return;

	LudumGameInstance * ludum_game_instance = GetGameInstance();
	if (ludum_game_instance == nullptr || ludum_game_instance->current_power_up == nullptr || ludum_game_instance->current_power_up_surface == nullptr)
		return;

	bool decreasing_power_up = ludum_game_instance->current_power_up_surface->GetGeometricObject()->FindPropertyBool("DECREASE_POWER_UP", false);

	if (!ludum_game_instance->current_power_up->CanPowerUp(GetGame(), this, decreasing_power_up))
		return;

	int const buy_key_buttons[] = { GLFW_KEY_LEFT_ALT, GLFW_KEY_RIGHT_ALT, -1 };

	bool buy_pressed = CheckButtonPressed(buy_key_buttons, chaos::XBoxButton::BUTTON_Y);
	if (buy_pressed && !buylocked)
	{
		buy_timer += delta_time;
		if (buy_timer >= ludum_game->buy_upgrade_time)
		{
			// XXX : HACK : destroy all power up zone in the camera view : not the best but should work if a single zone in the same time
			LudumLevelInstance * ludum_level_instance = GetLevelInstance();
			if (ludum_level_instance != nullptr)
			{
				death::TiledMapLayerInstance * layer_instance = ludum_level_instance->FindLayerInstance("Zones");
				if (layer_instance != nullptr)
				{
					death::Camera const* camera = ludum_level_instance->GetCamera(0);
					if (camera != nullptr)
					{
						layer_instance->FindTileCollisions(camera->GetCameraBox(true), [](death::TiledMapParticle& particle)
						{
							// shuxxx particle.gid = 0;
							return true;
						});
					}
				}
			}

			// reset the corresponding trigger surface
			ludum_game_instance->current_power_up->ApplyPowerUp(GetGame(), this, decreasing_power_up);
			buylocked = true;
			// shuxxx ludum_game_instance->current_power_up_surface->SetEnabled(false);

			PowerUpTriggerObject * power_up_trigger_surface = auto_cast(ludum_game_instance->current_power_up_surface.get());
			if (power_up_trigger_surface != nullptr)
				power_up_trigger_surface->ResetTrigger();


			ludum_game_instance->current_power_up = nullptr;
			ludum_game_instance->current_power_up_surface = nullptr;
			buy_timer = 0.0f;
		}
	}
	else
	{
		if (buy_timer > 0.0f)
		{
			PowerUpTriggerObject * power_up_trigger_surface = auto_cast(ludum_game_instance->current_power_up_surface.get());
			if (power_up_trigger_surface != nullptr)
				power_up_trigger_surface->ResetTrigger();
		}
		buy_timer = 0.0f;
	}

	if (!buy_pressed)
		buylocked = false;
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

	health               = ludum_checkpoint->health;
	max_health           = ludum_checkpoint->max_health;
	current_speed_index          = ludum_checkpoint->current_speed_index;
	current_damage_index         = ludum_checkpoint->current_damage_index;
	current_charged_damage_index = ludum_checkpoint->current_charged_damage_index;
	current_fire_rate_index      = ludum_checkpoint->current_fire_rate_index;

	return true;
}

bool LudumPlayer::DoSaveIntoCheckpoint(death::PlayerCheckpoint * checkpoint) const
{
	LudumPlayerCheckpoint * ludum_checkpoint = auto_cast(checkpoint);
	if (ludum_checkpoint == nullptr)
		return false;

	if (!death::Player::DoSaveIntoCheckpoint(checkpoint))
		return false;

    ludum_checkpoint->health = health;
    ludum_checkpoint->max_health = max_health;
    ludum_checkpoint->current_speed_index = current_speed_index;
    ludum_checkpoint->current_damage_index = current_damage_index;
    ludum_checkpoint->current_charged_damage_index = current_charged_damage_index;
    ludum_checkpoint->current_fire_rate_index = current_fire_rate_index;

	return true;
}