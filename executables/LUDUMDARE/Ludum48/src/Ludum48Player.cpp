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
	burned_souls = 0;
}

void LudumPlayer::OnLevelChanged(chaos::Level * new_level, chaos::Level * old_level, chaos::LevelInstance * new_level_instance)
{
	chaos::Player::OnLevelChanged(new_level, old_level, new_level_instance);

	burned_souls = 0;
}
