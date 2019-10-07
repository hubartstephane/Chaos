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


void LudumPlayer::UpdateBrightSideOfLife(double delta_time)
{
#if 0
	if (brightsideoflife_timer > 0.0f)
	{
		brightsideoflife_timer -= (float)delta_time;
		if (brightsideoflife_timer > 0.0f)
			return;	
	}
#endif
	if (GetSpecialPowerLevel() > 0)
	{
		int const bright_key_buttons[] = {GLFW_KEY_LEFT_ALT, GLFW_KEY_RIGHT_ALT, -1};
		bool bright_key_pressed = CheckButtonPressed(bright_key_buttons, chaos::MyGLFW::XBOX_BUTTON_Y);
		if (bright_key_pressed)
		{
			DoUpdateBrightSideOfLife(true);
			return;
		}
	}
#if 0
	DoUpdateBrightSideOfLife(false);
#endif

}

void LudumPlayer::DoUpdateBrightSideOfLife(bool value)
{

	if (value && !brightsideoflife)
	{
		GetGame()->SetInGameMusic("brightsideoflife");

	}	
	brightsideoflife = value;

#if 0

	if (brightsideoflife == value)
		return;
	brightsideoflife = value;
	brightsideoflife_timer = 2.0f;

	if (value)
	{
		GetGame()->SetInGameMusic("brightsideoflife");
	}		
	else
	{
		death::TiledMap::Level * tiled_level = auto_cast(GetLevel());
		if (tiled_level != nullptr)
		{
			chaos::TiledMap::Map * tiled_map = tiled_level->GetTiledMap();
			if (tiled_map != nullptr)
			{
				std::string const * level_music = tiled_map->FindPropertyString("MUSIC");
				if (level_music != nullptr && !chaos::StringTools::IsEmpty(*level_music))
				{
					GetGame()->SetInGameMusic(level_music->c_str());				
					return;
				}			
			}					
		}
		GetGame()->SetInGameMusic("game_music");
	}
#endif
	
}

void LudumPlayer::TickPlayerDisplacement(double delta_time)
{
	// displace the player
	UpdatePlayerAcceleration(delta_time);
	// player fire particles
	UpdatePlayerFire(delta_time);
	// player fire particles
	UpdateBrightSideOfLife(delta_time);



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

	float left_length_2 = glm::length2(left_stick_position);	
	float right_length_2 = glm::length2(right_stick_position);

	int const dash_key_buttons[] = {GLFW_KEY_LEFT_CONTROL, GLFW_KEY_RIGHT_CONTROL, -1};
	bool dash_pressed = CheckButtonPressed(dash_key_buttons, chaos::MyGLFW::XBOX_BUTTON_B);

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
		if (left_length_2 > 0.0f || left_length_2 > 0.0f)
		{
			direction = (left_length_2 > right_length_2) ?
				left_stick_position / chaos::MathTools::Sqrt(left_length_2) :
				right_stick_position / chaos::MathTools::Sqrt(right_length_2);	

			// axis Y reversed
			direction *= glm::vec2(1.0f, -1.0f);
			// compute the orientation

		// shuldum	player_particle->orientation = atan2f(direction.y, direction.x) - (float)M_PI * 0.5f;		




		}
		else
		{
// shuludm			float angle = player_particle->orientation + (float)M_PI * 0.5f;

			float angle = player_particle->orientation;

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
		if (fire_pressed && GetCurrentPowerRateValue() > 0)
		{
			FireProjectile();					
			fire_timer = GetCurrentPowerRateValue();
		}								
	}			
}

ParticleFire * LudumPlayer::FireProjectile()
{
	LudumGame const * ludum_game = GetLudumGame();
	if (ludum_game == nullptr)
		return nullptr;

	float power_ratio = 1.0f;

	PlayerUpgrade * upgrade = FindPlayerUpgrade(UpgradeKeys::DAMAGE);
	if (upgrade != nullptr)
	{
		float power_level = (float)GetCurrentDamageValue();
		float power_max   = (float)upgrade->max_level;	
	
		power_ratio = power_level / power_max;
	}

	








	int count = GetCurrentPowerSpreadValue(); 
	ParticleFire * p = FireProjectile(fire_bitmap_layout, ludum_game->fire_size_ratio * power_ratio, count, "fire", 0.1f, ludum_game->fire_velocity);
	if (p != nullptr)
	{
		for (int i = 0 ; i < count ; ++i)
		{
			p[i].damage = GetCurrentDamageValue();
			p[i].player_ownership = true;
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

	return GetLudumGameInstance()->FireProjectile(fire_allocation.get(), GetPlayerBox(), layout, ratio_to_player, count, sound_name, delta_rotation, true, velocity, -(float)M_PI_2);

	//return GetLudumGameInstance()->FireProjectile(fire_allocation.get(), GetPlayerBox(), layout, ratio_to_player, count, sound_name, delta_rotation, true, velocity, player_particle->orientation);
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
	current_life = current_max_life;

}


void LudumPlayer::OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance)
{
	death::Player::OnLevelChanged(new_level, old_level, new_level_instance);
	current_life = current_max_life;
	DoUpdateBrightSideOfLife(false);
}


void LudumPlayer::SetLifeBarValue(float in_value, bool in_increment)
{

	// compute new life 
	float old_life = current_life;
	float new_life = current_life;

	if (in_increment)
		new_life += in_value;
	else
		new_life = in_value;

	if (new_life < 0.0f)
		new_life = 0.0f;
	else if (new_life > current_max_life)
		new_life = current_max_life;

	// commit life lost
	bool update_life = true;
#if _DEBUG
	if (old_life > new_life && GetGame()->GetCheatMode())
		update_life = false;
#endif
	if (update_life)
		current_life = new_life;

	// special FX
	if (old_life > new_life)
	{
		death::Camera * camera = GetLevelInstance()->GetCamera(0);
		if (camera != nullptr)
		{
			death::ShakeCameraComponent * shake_component = camera->FindComponentByClass<death::ShakeCameraComponent>();
			if (shake_component != nullptr)
				shake_component->RestartModifier();
		}
	}
}



void LudumPlayer::OnDamagedReceived(float damage)
{
	SetLifeBarValue(-damage, true);
	GetGame()->Play("player_touched", false, false, 0.0f, death::SoundContext::LEVEL);
}

void LudumPlayer::OnPlayerUpgrade(chaos::TagType upgrade_type)
{
	PlayerUpgrade * upgrade = FindPlayerUpgrade(upgrade_type);
	if (upgrade != nullptr)
		upgrade->Upgrade(GetGame());


}





void LudumPlayer::RegisterUpgrades()
{
	LudumGame * ludum_game = GetLudumGame();
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

		result += chaos::StringTools::Printf("[%d [%s]]", ratio, upgrade->bitmap_name.c_str());
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

	T result;

	size_t level = (size_t)GetUpgradeLevel(upgrade_type);
	if (level >= count)
		result = values[count - 1];
	else
		result = values[level];
	return result;
}





int LudumPlayer::GetSpeedLevel() const
{
	return GetUpgradeLevel(UpgradeKeys::SPEED);
}

int LudumPlayer::GetDamageLevel() const
{
	return GetUpgradeLevel(UpgradeKeys::DAMAGE);
}

int LudumPlayer::GetDashLevel() const
{
	return GetUpgradeLevel(UpgradeKeys::DASH);
}
int LudumPlayer::GetGhostLevel() const
{
	return GetUpgradeLevel(UpgradeKeys::GHOST);
}

int LudumPlayer::GetPowerRateLevel() const
{
	return GetUpgradeLevel(UpgradeKeys::POWERRATE);
}

int LudumPlayer::GetPowerSpreadLevel() const
{
	return GetUpgradeLevel(UpgradeKeys::POWERSPREAD);
}

int LudumPlayer::GetSpecialPowerLevel() const
{
	return GetUpgradeLevel(UpgradeKeys::SPECIALPOWER);
}








float LudumPlayer::GetCurrentSpeedValue() const
{
	return GetPlayerUpgradedValue(UpgradeKeys::SPEED, GetLudumGame()->player_speeds);
}

float LudumPlayer::GetCurrentDamageValue() const
{
	return GetPlayerUpgradedValue(UpgradeKeys::DAMAGE, GetLudumGame()->player_damages);
}

float LudumPlayer::GetCurrentDashValue() const
{
	return GetPlayerUpgradedValue(UpgradeKeys::DASH, GetLudumGame()->player_dash_cooldowns);
}

bool LudumPlayer::GetCurrentGhostValue() const
{
	return GetGhostLevel() > 0;
}

float LudumPlayer::GetCurrentPowerRateValue() const
{
	return GetPlayerUpgradedValue(UpgradeKeys::POWERRATE, GetLudumGame()->player_power_rates);
}

int LudumPlayer::GetCurrentPowerSpreadValue() const
{
	return GetPlayerUpgradedValue(UpgradeKeys::POWERSPREAD, GetLudumGame()->player_power_spreads);
}

float LudumPlayer::GetCurrentSpecialPowerValue() const
{
	return GetPlayerUpgradedValue(UpgradeKeys::SPECIALPOWER, GetLudumGame()->player_specialpowers);
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
