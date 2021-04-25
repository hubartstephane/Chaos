#include <chaos/Chaos.h>

#include "Ludum48Player.h"
#include "Ludum48Level.h"
#include "Ludum48LevelInstance.h"
#include "Ludum48Game.h"
#include "Ludum48GameInstance.h"
#include "Ludum48Particles.h"

bool LudumPlayer::Initialize(chaos::GameInstance * in_game_instance)
{
	if (!chaos::Player::Initialize(in_game_instance))
		return false;



	return true;
}

void LudumPlayer::OnLifeLost()
{
	chaos::Player::OnLifeLost();

	death_timer = -1.0f;
	suicidal_timer = 0.0f;


	GetGame()->ReloadCurrentLevel();


}

void LudumPlayer::OnLevelChanged(chaos::Level * new_level, chaos::Level * old_level, chaos::LevelInstance * new_level_instance)
{
	chaos::Player::OnLevelChanged(new_level, old_level, new_level_instance);

	death_timer = -1.0f;
	suicidal_timer = 0.0f;


}

bool LudumPlayer::IsDead() const
{
	if (chaos::Player::IsDead())
		return true;
	if (death_timer == 0.0f)
		return true;
	return false;
}

bool LudumPlayer::DoTick(float delta_time)
{
	chaos::Player::DoTick(delta_time);

	// count down
	if (pawn == nullptr || pawn->GetParticle<chaos::ParticleDefault>(0) == nullptr)
	{
		if (death_timer < 0.0f)
			death_timer = max_death_timer;
		death_timer = std::max(0.0f, death_timer - delta_time);
	}

	// suicidal
	if (death_timer < 0.0f)
	{
		chaos::Key const suicidal_buttons[] = { chaos::KeyboardButton::SPACE, chaos::GamepadButton::Y, chaos::Key() };
		if (CheckButtonPressed(suicidal_buttons))
		{
			suicidal_timer = std::min(suicidal_timer + delta_time, max_suicidal_timer); 
			if (suicidal_timer == max_suicidal_timer)
			{
				death_timer = 0.0f; // kill the player
			}
		}
		else
		{
			suicidal_timer = std::max(suicidal_timer - delta_time, 0.0f);
			

		}



	}


	return true;


}