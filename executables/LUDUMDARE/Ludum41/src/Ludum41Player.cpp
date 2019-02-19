#include "Ludum41Player.h"
#include "Ludum41Game.h"
#include "Ludum41GameInstance.h"
#include "Ludum41Level.h"
#include "Ludum41LevelInstance.h"

DEATH_GAMEFRAMEWORK_IMPLEMENT_PLAYER(Ludum);

LudumPlayer::LudumPlayer(death::GameInstance * in_game_instance) : 
	death::Player(in_game_instance)
{
	LudumGame const * ludum_game = GetLudumGame();
	if (ludum_game != nullptr)
		player_length = ludum_game->player_initial_length;
}

void LudumPlayer::TickPlayerDisplacement(double delta_time)
{
	float value = left_stick_position.x;
	if (abs(right_stick_position.x) > abs(left_stick_position.x))
		value = right_stick_position.x;

	glm::vec2 position = GetPlayerPosition();
	SetPlayerPosition(glm::vec2(position.x + value, PLAYER_Y));	
}

bool LudumPlayer::OnMouseMove(double x, double y)
{
	death::Game const * game = GetGame();
	if (game == nullptr)
		return true;
	left_stick_position.x = game->GetMouseSensitivity() * (float)x;
	return false;
}

bool LudumPlayer::OnCharEvent(unsigned int c)
{
	LudumGameInstance * ludum_game_instance = GetLudumGameInstance();

	// CHALLENGE
	if (c >= 'a' && c <= 'z')
	{
		ludum_game_instance->SendKeyboardButtonToChallenge((char)c);
		return true;
	}
	else if (c >= 'A' && c <= 'Z')
	{
		ludum_game_instance->SendKeyboardButtonToChallenge((char)(c - 'A' + 'a'));
		return true;
	}

	return death::Player::OnCharEvent(c);
}

void LudumPlayer::InternalHandleGamepadInputs(double delta_time, chaos::MyGLFW::GamepadData const * gpd)
{
	death::Player::InternalHandleGamepadInputs(delta_time, gpd);

	LudumGameInstance * ludum_game_instance = GetLudumGameInstance();
	ludum_game_instance->SendGamepadButtonToChallenge(gpd);
}


void LudumPlayer::SetPlayerLength(float in_length, bool increment)
{
	if (increment)
		player_length += in_length;
	else
		player_length = in_length;
}



#if 0


if (player_allocations == nullptr)
{
	player_allocations = CreatePlayer();
	SetPlayerLength(player_length);
	SetPlayerPosition(0, glm::vec2(0.0f, PLAYER_Y));
	RestrictPlayerToWorld(0);
}



void LudumGame::SetPlayerLength(float length)
{
	length = chaos::MathTools::Clamp(length, player_min_length, player_max_length);

	chaos::box2 box = GetPlayerBox(0);
	box.half_size = glm::vec2(length * 0.5f, PLAYER_HEIGHT * 0.5f);
	SetPlayerBox(0, box);

	player_length = length;
	RestrictPlayerToWorld(0);
}
#endif