#include "Ludum46PCH.h"
#include "Ludum46Player.h"
#include "Ludum46Level.h"
#include "Ludum46LevelInstance.h"
#include "Ludum46Game.h"
#include "Ludum46GameInstance.h"
#include "Ludum46Particles.h"

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
