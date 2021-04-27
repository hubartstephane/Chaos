#include <chaos/Chaos.h>

#include "Ludum48Player.h"
#include "Ludum48Level.h"
#include "Ludum48LevelInstance.h"
#include "Ludum48Game.h"
#include "Ludum48GameInstance.h"
#include "Ludum48Particles.h"

bool LudumPlayer::Initialize(GameInstance * in_game_instance)
{
	if (!Player::Initialize(in_game_instance))
		return false;



	return true;
}

void LudumPlayer::OnLifeLost()
{
	Player::OnLifeLost();

	death_timer = -1.0f;
	suicidal_timer = -1.0f;


	GetGame()->ReloadCurrentLevel();


}

void LudumPlayer::OnLevelChanged(Level * new_level, Level * old_level, LevelInstance * new_level_instance)
{
	Player::OnLevelChanged(new_level, old_level, new_level_instance);

	death_timer = -1.0f;
	suicidal_timer = -1.0f;


}

bool LudumPlayer::IsDead() const
{
	if (Player::IsDead())
		return true;
	if (death_timer == 0.0f)
		return true;
	return false;
}

bool LudumPlayer::DoTick(float delta_time)
{
	Player::DoTick(delta_time);

	// count down
	if (pawn == nullptr || pawn->GetParticle<ParticleDefault>(0) == nullptr)
	{
		if (death_timer < 0.0f)
			death_timer = max_death_timer;
		death_timer = std::max(0.0f, death_timer - delta_time);
	}

	// suicidal
	if (death_timer < 0.0f)
	{
		Key const suicidal_buttons[] = { KeyboardButton::SPACE, GamepadButton::Y, Key() };
		if (CheckButtonPressed(suicidal_buttons))
		{
			suicidal_timer = std::min(std::max(suicidal_timer, 0.0f) + delta_time, max_suicidal_timer); 
			if (suicidal_timer == max_suicidal_timer)
			{
				death_timer = 0.0f; // kill the player
			}
		}
		else
		{
			suicidal_timer = -1.0f;

			

		}



	}


	return true;


}


bool LudumPlayer::InitializeGameValues(nlohmann::json const& config, boost::filesystem::path const& config_path, bool hot_reload)
{
	if (!Player::InitializeGameValues(config, config_path, hot_reload))
		return false;
	if (life_count != 8)
		cheater_farid = true;
	return true;
}
