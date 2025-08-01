#include "Ludum44PCH.h"
#include "Ludum44Player.h"
#include "Ludum44Level.h"
#include "Ludum44LevelInstance.h"
#include "Ludum44Game.h"
#include "Ludum44GameInstance.h"
#include "Ludum44Particles.h"

void LudumPlayer::TickPlayerDisplacement(float delta_time)
{
	// displace the player
	UpdatePlayerAcceleration(delta_time);
}

void LudumPlayer::TickInternal(float delta_time)
{
	chaos::Player::TickInternal(delta_time);

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
	if (pawn == nullptr || pawn->GetAllocation() == nullptr || pawn->GetAllocation()->GetParticleCount() == 0)
		return;

	chaos::ParticleAccessor<ParticlePlayer> particles = pawn->GetAllocation()->GetParticleAccessor();


	ParticlePlayer * player_particle = &particles[0];

	player_particle->velocity = glm::vec2(0.0f, 0.0f);

	float left_length_2 = glm::length2(left_stick_position);
	float right_length_2 = glm::length2(right_stick_position);
	if (left_length_2 > 0.0f || right_length_2 > 0.0f)
	{
		glm::vec2 speed = (left_length_2 > right_length_2) ?
			left_stick_position / std::sqrt(left_length_2) :
			right_stick_position / std::sqrt(right_length_2);

		player_particle->velocity = ludum_game->player_speeds[current_speed_index] * ludum_game->player_speed_factor * speed;
	}
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
    ludum_game_instance->FireProjectile("charged_fire", GetPawn()->GetBoundingBox(), 1.0f, count, "thrust", 0.0f, 0.1f, ludum_game->fire_velocity, ludum_game->player_charged_damages[current_charged_damage_index], true, true);
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
    ludum_game_instance->FireProjectile("fire", GetPawn()->GetBoundingBox(), 0.3f, count, "fire", 0.0f, 0.1f, ludum_game->fire_velocity, ludum_game->player_damages[current_damage_index], false, true);
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

	chaos::Key const fire_key_buttons[] = { chaos::KeyboardButton::SPACE, chaos::GamepadButton::A, chaos::Key()};
	chaos::Key const charged_key_buttons[] = {chaos::KeyboardButton::LEFT_CONTROL, chaos::KeyboardButton::RIGHT_CONTROL, chaos::GamepadButton::B, chaos::Key()};

	bool charged_pressed = CheckKeyDown(charged_key_buttons);
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
				bool fire_pressed = CheckKeyDown(fire_key_buttons);
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
	if (ludum_game_instance == nullptr || ludum_game_instance->current_power_up == nullptr || ludum_game_instance->current_powerup_trigger == nullptr)
		return;

	bool decreasing_power_up = ludum_game_instance->current_powerup_trigger->decrease_power; // GetGeometricObject()->FindPropertyBool("DECREASE_POWER_UP", false);

	if (!ludum_game_instance->current_power_up->CanPowerUp(GetGame(), this, decreasing_power_up))
		return;

	chaos::Key const buy_key_buttons[] = { chaos::KeyboardButton::LEFT_ALT, chaos::KeyboardButton::RIGHT_ALT, chaos::GamepadButton::Y, chaos::Key()};

	bool buy_pressed = CheckKeyDown(buy_key_buttons);
	if (buy_pressed && !buylocked)
	{
		buy_timer += delta_time;
		if (buy_timer >= ludum_game->buy_upgrade_time)
		{
			// reset the corresponding trigger surface
			ludum_game_instance->current_power_up->ApplyPowerUp(GetGame(), this, decreasing_power_up);
			buylocked = true;
			// shuxxx ludum_game_instance->current_powerup_trigger->SetEnabled(false);

			PowerUpTrigger * power_up_trigger_surface = auto_cast(ludum_game_instance->current_powerup_trigger.get());
			if (power_up_trigger_surface != nullptr)
				power_up_trigger_surface->ResetTrigger();


			ludum_game_instance->current_power_up = nullptr;
			ludum_game_instance->current_powerup_trigger = nullptr;
			buy_timer = 0.0f;
		}
	}
	else
	{
		if (buy_timer > 0.0f)
		{
			PowerUpTrigger * power_up_trigger_surface = auto_cast(ludum_game_instance->current_powerup_trigger.get());
			if (power_up_trigger_surface != nullptr)
				power_up_trigger_surface->ResetTrigger();
		}
		buy_timer = 0.0f;
	}

	if (!buy_pressed)
		buylocked = false;
}

bool LudumPlayer::SerializeFromJSON(chaos::JSONReadConfiguration config)
{
	if (!chaos::Player::SerializeFromJSON(config))
		return false;
	chaos::JSONTools::GetAttribute(config, "SPEED_INDEX", current_speed_index);
	chaos::JSONTools::GetAttribute(config, "DAMAGE_INDEX", current_damage_index);
	chaos::JSONTools::GetAttribute(config, "CHARGED_DAMAGE_INDEX", current_charged_damage_index);
	chaos::JSONTools::GetAttribute(config, "FIRE_RATE_INDEX", current_fire_rate_index);
	return true;
}

bool LudumPlayer::SerializeIntoJSON(nlohmann::json * json) const
{
	if (!chaos::Player::SerializeIntoJSON(json))
		return false;
	chaos::JSONTools::SetAttribute(json, "SPEED_INDEX", current_speed_index);
	chaos::JSONTools::SetAttribute(json, "DAMAGE_INDEX", current_damage_index);
	chaos::JSONTools::SetAttribute(json, "CHARGED_DAMAGE_INDEX", current_charged_damage_index);
	chaos::JSONTools::SetAttribute(json, "FIRE_RATE_INDEX", current_fire_rate_index);
	return true;
}
