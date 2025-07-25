#include "Ludum45PCH.h"
#include "Ludum45Player.h"
#include "Ludum45Level.h"
#include "Ludum45LevelInstance.h"
#include "Ludum45Game.h"
#include "Ludum45GameInstance.h"
#include "Ludum45Particles.h"

bool LudumPlayer::Initialize(chaos::GameInstance * in_game_instance)
{
	if (!chaos::Player::Initialize(in_game_instance))
		return false;
	RegisterUpgrades();
	return true;
}


void LudumPlayer::UpdateBrightSideOfLife(float delta_time)
{
	if (GetSpecialPowerLevel() > 0)
	{
		chaos::Key const bright_key_buttons[] = { chaos::KeyboardButton::LEFT_ALT, chaos::KeyboardButton::RIGHT_ALT, chaos::GamepadButton::Y, chaos::Key()};
		bool bright_key_pressed = CheckKeyDown(bright_key_buttons);
		if (bright_key_pressed)
		{
			DoUpdateBrightSideOfLife(true);
			return;
		}
	}
}

void LudumPlayer::DoUpdateBrightSideOfLife(bool value)
{

	if (value && !brightsideoflife)
	{
		GetGame()->SetInGameMusic("brightsideoflife");

	}
	brightsideoflife = value;
}

void LudumPlayer::TickInternal(float delta_time)
{
	chaos::Player::TickInternal(delta_time);

	// player fire particles
	UpdatePlayerFire(delta_time);
	// player fire particles
	UpdateBrightSideOfLife(delta_time);
}

void LudumPlayer::TickPlayerDisplacement(float delta_time)
{
	// displace the player
	UpdatePlayerAcceleration(delta_time);
}


void LudumPlayer::UpdatePlayerAcceleration(float delta_time)
{

	// shufixme
	LudumGame const * ludum_game = GetGame();
	if (ludum_game == nullptr)
		return;

	if (pawn == nullptr || pawn->GetAllocation() == nullptr || pawn->GetAllocation()->GetParticleCount() == 0)
		return;

	chaos::ParticleAccessor<ParticlePlayer> particles = pawn->GetAllocation()->GetParticleAccessor();

	ParticlePlayer* player_particle = &particles[0];
	if (player_particle == nullptr)
		return;

	// update dash timer
	dash_timer -= delta_time;
	if (dash_timer < 0.0f)
		dash_timer = 0.0f;
	dash_cooldown -= delta_time;
	if (dash_cooldown < 0.0f)
		dash_cooldown = 0.0f;

	// get the dash input

	float left_length_2 = glm::length2(left_stick_position);
	float right_length_2 = glm::length2(right_stick_position);

	chaos::Key const dash_key_buttons[] = { chaos::KeyboardButton::LEFT_CONTROL, chaos::KeyboardButton::RIGHT_CONTROL, chaos::GamepadButton::B, chaos::Key()};
	bool dash_pressed = CheckKeyDown(dash_key_buttons);

	if (dash_pressed)
	{

		if (GetDashLevel() > 0 && (left_length_2 > 0.0f || right_length_2 > 0.0f))
		{
			if (GetDashLevel() > 0)
			{
				if (dash_cooldown <= 0.0f && !dash_locked)
				{
					dash_timer = ludum_game->player_dash_duration;
					dash_cooldown = GetCurrentDashValue();
					GetGame()->PlaySound("dash", false, false, 0.0f, chaos::SoundContext::LEVEL);
				}
			}
			dash_locked = true; // dash is locked until the key is released
		}
	}
	else
	{
		dash_locked = false;
	}

	// compute max velocity and extra dash boost
	float input_max_velocity = GetCurrentSpeedValue() * ludum_game->player_speed_factor;
	float max_velocity       = input_max_velocity;

	float dash_velocity_boost = 0.0f;

	bool dashing = (dash_timer > 0.0f);
	if (dashing)
	{
		max_velocity += ludum_game->player_dash_velocity_boost;
		dash_velocity_boost = ludum_game->player_dash_velocity_boost;
	}

	glm::vec2 player_velocity = player_particle->velocity;

	// update the player
	if (left_length_2 > 0.0f || right_length_2 > 0.0f || dashing)
	{

		float input_factor = 1.0f;

		glm::vec2 direction = glm::vec2(0.0f, 0.0f);
		// compute the normalized direction
		if (left_length_2 > 0.0f || right_length_2 > 0.0f)
		{
			direction = (left_length_2 > right_length_2) ?
				left_stick_position / std::sqrt(left_length_2) :
				right_stick_position / std::sqrt(right_length_2);
		}
		else
		{
			float l = glm::length(player_velocity);
			if (l)
				direction = glm::normalize(player_velocity);
			else
				direction = glm::vec2(0.0f, 0.0f);

			input_factor = 0.0f;
		}

		// split current velocity into normal and its tangeantial
		glm::vec2 normal_velocity      = glm::vec2(0.0f, 0.0f);
		glm::vec2 tangeantial_velocity = glm::vec2(0.0f, 0.0f);
		if (glm::length2(player_velocity) > 0.0f)
		{
			normal_velocity      = player_velocity * glm::dot(glm::normalize(player_velocity), direction);
			tangeantial_velocity = player_velocity - normal_velocity;
		}

		player_velocity =
			input_factor * input_max_velocity * direction
			+
			dash_velocity_boost * direction
			+
			tangeantial_velocity * powf(ludum_game->player_tan_speed_damping, delta_time);
	}
	else
		player_velocity *= powf(ludum_game->player_speed_damping, delta_time);

	// clamp the final velocity
	float len = glm::length(player_velocity);
	if (len > max_velocity)
		player_velocity *= max_velocity / len;
	player_particle->velocity = player_velocity;

	// displace the player
	player_particle->bounding_box.position += delta_time * player_particle->velocity;
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
	if (fire_timer <= 0.0f)
	{
		bool fire_pressed = CheckKeyDown(fire_key_buttons);
		if (fire_pressed && GetCurrentPowerRateValue() > 0)
		{
            FireProjectiles();
			fire_timer = GetCurrentPowerRateValue();
		}
	}
}

void LudumPlayer::FireProjectiles()
{
    // early exit
    LudumGame * ludum_game = GetGame();
    if (ludum_game == nullptr)
        return;

	if (pawn == nullptr)
		return;

	LudumLevelInstance* ludum_level_instance = GetLevelInstance();

    chaos::ParticleSpawner fire_spawner = ludum_level_instance->GetParticleSpawner("PlayerFire", "fire");
    if (!fire_spawner.IsValid() || !fire_spawner.HasBitmap())
        return;

    chaos::AtlasBitmapLayout const layout = *fire_spawner.GetBitmapInfo();

    // get the spread value
    int count = GetCurrentPowerSpreadValue();
    if (count == 0)
        return;
    // get the power ratio
    float power_ratio = 1.0f;

    PlayerUpgrade* upgrade = FindPlayerUpgrade(UpgradeKeys::DAMAGE);
    if (upgrade != nullptr)
    {
        float power_level = (float)GetCurrentDamageValue();
        float power_max = (float)upgrade->max_level;

        power_ratio = power_level / power_max;
    }

    // create particles (keep the same allocation)
    float ratio_to_player = ludum_game->fire_size_ratio * power_ratio;
    float velocity = ludum_game->fire_velocity;
    float damage = GetCurrentDamageValue();

    chaos::box2 particle_box = pawn->GetBoundingBox();
    particle_box.half_size = ratio_to_player * particle_box.half_size;
    particle_box = chaos::SetBoxAspect(particle_box, chaos::MathTools::CastAndDiv<float>(layout.width, layout.height), chaos::SetBoxAspectMethod::PREFER_UPDATE_WIDTH);

    fire_spawner.SpawnParticles(count, false).Process([velocity, damage, particle_box](chaos::ParticleAccessor<ParticleFire> accessor)
    {
        float delta_rotation  = 0.1f;
        float offset_rotation = -(float)M_PI_2;

        for (ParticleFire & particle : accessor)
        {
            size_t particle_index = accessor.GetDataIndex(&particle);

            float rotation = delta_rotation * ((float)particle_index) - delta_rotation * (float)(accessor.GetDataCount() / 2);

            particle.bounding_box = particle_box;
            particle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

            int scroll_direction = 0;
            float particle_orientation = rotation + offset_rotation;
            float particle_velocity_orientation = rotation + offset_rotation + (float)M_PI_2;

            glm::vec2 direction = glm::vec2(std::cos(particle_velocity_orientation), std::sin(particle_velocity_orientation));
            particle.velocity = velocity * direction;
            particle.rotation = particle_orientation;

            particle.bounding_box.position += direction * particle_box.half_size[scroll_direction];

            particle.player_ownership = true;

            particle.damage = damage;
            particle.player_ownership = true;
            particle.trample = false;
        }
    });

    // play the sound
    ludum_game->PlaySound("fire", false, false, 0.0f, chaos::SoundContext::LEVEL);
}

void LudumPlayer::OnLevelChanged(chaos::Level * new_level, chaos::Level * old_level, chaos::LevelInstance * new_level_instance)
{
	chaos::Player::OnLevelChanged(new_level, old_level, new_level_instance);
	DoUpdateBrightSideOfLife(false);
}

void LudumPlayer::OnHealthChanged(float old_health, float new_health, bool invulnerable)
{
	chaos::Player::OnHealthChanged(old_health, new_health, invulnerable);
	if (old_health < new_health)
		GetGame()->PlaySound("player_touched", false, false, 0.0f, chaos::SoundContext::LEVEL);
}

void LudumPlayer::OnPlayerUpgrade(chaos::TagType upgrade_type)
{
	PlayerUpgrade * upgrade = FindPlayerUpgrade(upgrade_type);
	if (upgrade != nullptr)
		upgrade->Upgrade(GetGame());
}

void LudumPlayer::RegisterUpgrades()
{
	LudumGame * ludum_game = GetGame();
	if (ludum_game == nullptr)
		return;

	upgrades.push_back(new PlayerUpgrade(UpgradeKeys::SPEED, ludum_game->player_speeds.size() , "SPEED", "gods_speed"));
	upgrades.push_back(new PlayerUpgrade(UpgradeKeys::DAMAGE, ludum_game->player_damages.size(), "DAMAGE", "gods_power"));
	//upgrades.push_back(new PlayerUpgrade(UpgradeKeys::DASH, ludum_game->player_dash_cooldowns.size(), "DASH"));
	upgrades.push_back(new PlayerUpgrade(UpgradeKeys::DASH, 1, "DASH", "gods_dash"));
	upgrades.push_back(new PlayerUpgrade(UpgradeKeys::GHOST, 1, "GHOST", "gods_ghost"));
	upgrades.push_back(new PlayerUpgrade(UpgradeKeys::POWERRATE, ludum_game->player_power_rates.size(), "POWERRATE", "gods_blast"));
	upgrades.push_back(new PlayerUpgrade(UpgradeKeys::POWERSPREAD, ludum_game->player_power_spreads.size(), "POWERSPREAD", "gods_spread"));
	upgrades.push_back(new PlayerUpgrade(UpgradeKeys::SPECIALPOWER, 1, "SPECIALPOWER", "gods_sideoflife"));
};

PlayerUpgrade * LudumPlayer::FindPlayerUpgrade(chaos::TagType upgrade_type)
{
	size_t count = upgrades.size();
	for (size_t i = 0 ; i < count ; ++i)
	{
		PlayerUpgrade * upgrade = upgrades[i].get();
		if (upgrade == nullptr)
			continue;
		if (upgrade->type == upgrade_type)
			return upgrade;
	}
	return nullptr;
}

PlayerUpgrade const * LudumPlayer::FindPlayerUpgrade(chaos::TagType upgrade_type) const
{
	size_t count = upgrades.size();
	for (size_t i = 0 ; i < count ; ++i)
	{
		PlayerUpgrade const * upgrade = upgrades[i].get();
		if (upgrade == nullptr)
			continue;
		if (upgrade->type == upgrade_type)
			return upgrade;
	}
	return nullptr;
}

std::string LudumPlayer::GetPlayerUpgradeString() const
{
	std::string result;

	size_t count = upgrades.size();
	for (size_t i = 0 ; i < count ; ++i)
	{
		PlayerUpgrade const * upgrade = upgrades[i].get();


		float level     = (float)upgrade->level;
		float max_level = (float)upgrade->max_level;

		if (upgrade == nullptr || upgrade->level <= 0)
			continue;

		int ratio = (int)(8.0f * level / max_level);
		if (ratio < 1)
			ratio = 1;
		if (ratio > 8)
			ratio = 8;

		// 8 values for fonts

		result += chaos::StringTools::Printf("[%d [%s*]]", ratio, upgrade->bitmap_name.c_str());
	}
	return result;
}

size_t LudumPlayer::GetUpgradeLevel(chaos::TagType upgrade_type) const
{
	PlayerUpgrade const * upgrade = FindPlayerUpgrade(upgrade_type);
	if (upgrade != nullptr)
		return upgrade->level;
	return 0;
}

template<typename T>
T LudumPlayer::GetPlayerUpgradedValue(chaos::TagType upgrade_type, std::vector<T> const & values) const
{
	size_t count = values.size();
	if (count == 0)
		return T();

	T result;

	size_t level = GetUpgradeLevel(upgrade_type);
	if (level >= count)
		result = values[count - 1];
	else
		result = values[level];
	return result;
}





size_t LudumPlayer::GetSpeedLevel() const
{
	return GetUpgradeLevel(UpgradeKeys::SPEED);
}

size_t LudumPlayer::GetDamageLevel() const
{
	return GetUpgradeLevel(UpgradeKeys::DAMAGE);
}

size_t LudumPlayer::GetDashLevel() const
{
	return GetUpgradeLevel(UpgradeKeys::DASH);
}
size_t LudumPlayer::GetGhostLevel() const
{
	return GetUpgradeLevel(UpgradeKeys::GHOST);
}

size_t LudumPlayer::GetPowerRateLevel() const
{
	return GetUpgradeLevel(UpgradeKeys::POWERRATE);
}

size_t LudumPlayer::GetPowerSpreadLevel() const
{
	return GetUpgradeLevel(UpgradeKeys::POWERSPREAD);
}

size_t LudumPlayer::GetSpecialPowerLevel() const
{
	return GetUpgradeLevel(UpgradeKeys::SPECIALPOWER);
}








float LudumPlayer::GetCurrentSpeedValue() const
{
	LudumGame const * ludum_game = GetGame();
	return GetPlayerUpgradedValue(UpgradeKeys::SPEED, ludum_game->player_speeds);
}

float LudumPlayer::GetCurrentDamageValue() const
{
	LudumGame const* ludum_game = GetGame();
	return GetPlayerUpgradedValue(UpgradeKeys::DAMAGE, ludum_game->player_damages);
}

float LudumPlayer::GetCurrentDashValue() const
{
	LudumGame const* ludum_game = GetGame();
	return GetPlayerUpgradedValue(UpgradeKeys::DASH, ludum_game->player_dash_cooldowns);
}

float LudumPlayer::GetCurrentPowerRateValue() const
{
	LudumGame const* ludum_game = GetGame();
	return GetPlayerUpgradedValue(UpgradeKeys::POWERRATE, ludum_game->player_power_rates);
}

int LudumPlayer::GetCurrentPowerSpreadValue() const
{
	LudumGame const* ludum_game = GetGame();
	return GetPlayerUpgradedValue(UpgradeKeys::POWERSPREAD, ludum_game->player_power_spreads);
}

float LudumPlayer::GetCurrentSpecialPowerValue() const
{
	LudumGame const* ludum_game = GetGame();
	return GetPlayerUpgradedValue(UpgradeKeys::SPECIALPOWER, ludum_game->player_specialpowers);
}

bool LudumPlayer::GetCurrentGhostValue() const
{
	return GetGhostLevel() > 0;
}
