#include "Ludum41Player.h"
#include "Ludum41Game.h"

LudumPlayer::LudumPlayer(death::GameInstance * in_game_instance) : 
	death::Player(in_game_instance)
{
}

void LudumPlayer::TickPlayerDisplacement(double delta_time)
{
	float value = left_stick_position.x;
	if (abs(right_stick_position.x) > abs(left_stick_position.x))
		value = right_stick_position.x;

	glm::vec2 position = GetPlayerPosition(0);
	SetPlayerPosition(0, glm::vec2(position.x + value, PLAYER_Y));
	RestrictPlayerToWorld(0);
}

bool LudumPlayer::OnMouseMove(double x, double y)
{
	return false;
}