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

bool LudumPlayer::Initialize(death::GameInstance * in_game_instance)
{
	if (!death::Player::Initialize(in_game_instance))
		return false;
	RegisterUpgrades();
	return true;
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

	// update dash timer
	dash_timer -= dt;
	if (dash_timer < 0.0f)
		dash_timer = 0.0f;
	dash_cooldown -= dt;
	if (dash_cooldown < 0.0f)
		dash_cooldown = 0.0f;

	// get the dash input 
	int const dash_key_buttons[] = {GLFW_KEY_LEFT_CONTROL, GLFW_KEY_RIGHT_CONTROL, -1};
	bool dash_pressed = CheckButtonPressed(dash_key_buttons, chaos::MyGLFW::XBOX_BUTTON_B);

	if (dash_pressed)
	{
		if (GetDashLevel() > 0)
		{
			if (dash_cooldown <= 0.0f && !dash_locked)
			{
				dash_timer = ludum_game->player_dash_duration;
				dash_cooldown = ludum_game->player_dash_cooldown;			
			}		
		}
		dash_locked = true; // dash is locked until the key is released
	}
	else
	{
		dash_locked = false;	
	}

	// compute max velocity and extra dash boost
	float input_max_velocity = GetPlayerSpeed() * ludum_game->player_speed_factor;
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
	float left_length_2 = glm::length2(left_stick_position);	
	float right_length_2 = glm::length2(right_stick_position);
	if (left_length_2 > 0.0f || right_length_2 > 0.0f || dashing)
	{

		float input_factor = 1.0f;

		glm::vec2 direction = glm::vec2(0.0f, 0.0f);
		// compute the normalized direction
		if (left_length_2 > 0.0f || left_length_2 > 0.0f)
		{
			direction = (left_length_2 > right_length_2) ?
				left_stick_position / chaos::MathTools::Sqrt(left_length_2) :
				right_stick_position / chaos::MathTools::Sqrt(right_length_2);	
		
			// axis Y reversed
			direction *= glm::vec2(1.0f, -1.0f);
			// compute the orientation
			player_particle->orientation = atan2f(direction.y, direction.x) - (float)M_PI * 0.5f;		
		}
		else
		{
			float angle = player_particle->orientation + (float)M_PI * 0.5f;

			direction = glm::vec2(cosf(angle), sinf(angle)) ; // direction comes from the one from previous frame
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
			tangeantial_velocity * powf(ludum_game->player_tan_speed_damping, dt);
	}
	else
		player_velocity *= powf(ludum_game->player_speed_damping, dt);

	// clamp the final velocity		
	float len = glm::length(player_velocity);
	if (len > max_velocity)
		player_velocity *= max_velocity / len;
	player_particle->velocity = player_velocity;

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
		if (fire_pressed && GetPowerLevel() > 0)
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

	int count = GetPlayerPower(); 
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

void LudumPlayer::OnPlayerUpgrade(chaos::TagType upgrade_type)
{
	PlayerUpgrade * upgrade = FindPlayerUpgrade(upgrade_type);
	if (upgrade != nullptr)
		upgrade->Upgrade();


}

void LudumPlayer::RegisterUpgrades()
{
	upgrades.push_back(new PlayerUpgrade(UpgradeKeys::SPEED, "SPEED"));
	upgrades.push_back(new PlayerUpgrade(UpgradeKeys::VIEW, "VIEW"));
	upgrades.push_back(new PlayerUpgrade(UpgradeKeys::POWER, "POWER"));
	upgrades.push_back(new PlayerUpgrade(UpgradeKeys::DASH, "DASH"));
	upgrades.push_back(new PlayerUpgrade(UpgradeKeys::SPECIALPOWER, "SPECIALPOWER"));
	upgrades.push_back(new PlayerUpgrade(UpgradeKeys::GHOST, "GHOST"));
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
		if (upgrade == nullptr || upgrade->level <= 0)
			continue;
		result += chaos::StringTools::Printf("[%d [%s]]", upgrade->level, upgrade->bitmap_name.c_str());
	}
	return result;
}

int LudumPlayer::GetUpgradeLevel(chaos::TagType upgrade_type) const
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
	size_t level = (size_t)GetUpgradeLevel(upgrade_type);
	if (level >= count)
		return values.back();
	return values[level];
}



float LudumPlayer::GetPlayerSpeed() const
{
	return GetPlayerUpgradedValue(UpgradeKeys::SPEED, GetLudumGame()->player_speeds);
}


int LudumPlayer::GetPlayerPower() const
{
	return GetPlayerUpgradedValue(UpgradeKeys::POWER, GetLudumGame()->player_fire_rates);
}





int LudumPlayer::GetSpeedLevel() const
{
	return GetUpgradeLevel(UpgradeKeys::SPEED);
}

int LudumPlayer::GetPowerLevel() const
{
	return GetUpgradeLevel(UpgradeKeys::POWER);
}
int LudumPlayer::GetSpecialPowerLevel() const
{
	return GetUpgradeLevel(UpgradeKeys::SPECIALPOWER);
}
int LudumPlayer::GetDashLevel() const
{
	return GetUpgradeLevel(UpgradeKeys::DASH);
}
int LudumPlayer::GetGhostLevel() const
{
	return GetUpgradeLevel(UpgradeKeys::GHOST);
}
int LudumPlayer::GetViewLevel() const
{
	return GetUpgradeLevel(UpgradeKeys::VIEW);
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
