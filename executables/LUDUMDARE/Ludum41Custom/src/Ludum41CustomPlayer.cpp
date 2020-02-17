#include "Ludum41CustomPlayer.h"
#include "Ludum41CustomGame.h"
#include "Ludum41CustomGameInstance.h"
#include "Ludum41CustomLevel.h"
#include "Ludum41CustomLevelInstance.h"

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
	DisplacePlayerRacket(value * GetGame()->GetGamepadSensitivity() * (float)delta_time); // even if 0 because this will ensure player Y is well placed even if no input is pressed
}

void LudumPlayer::DisplacePlayerRacket(float delta_x)
{
	glm::vec2 position = GetPlayerPosition();
	SetPlayerPosition(glm::vec2(position.x + delta_x, PLAYER_Y));

	LudumLevelInstance * ludum_level_instance = GetLudumLevelInstance();
	if (ludum_level_instance != nullptr)
		ludum_level_instance->RestrictPlayerToWorld(this);
}


bool LudumPlayer::OnMouseMoveImpl(double x, double y)
{
	death::Game const * game = GetGame();
	if (game == nullptr)
		return true;
	if (game->IsPaused() || game->IsFreeCameraMode())
		return true;
	DisplacePlayerRacket(game->GetMouseSensitivity() * (float)x);
	return true;
}

bool LudumPlayer::OnCharEventImpl(unsigned int c)
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

	return death::Player::OnCharEventImpl(c);
}

void LudumPlayer::InternalHandleGamepadInputs(double delta_time, chaos::MyGLFW::GamepadData const * gpd)
{
	death::Player::InternalHandleGamepadInputs(delta_time, gpd);

	LudumGameInstance * ludum_game_instance = GetLudumGameInstance();
	ludum_game_instance->SendGamepadButtonToChallenge(gpd);
}

void LudumPlayer::SetPlayerLength(float in_length, bool increment)
{
	LudumGame const * ludum_game = GetLudumGame();

	if (increment)
		player_length += in_length;
	else
		player_length = in_length;

	player_length = std::clamp(player_length, ludum_game->player_min_length, ludum_game->player_max_length);

	chaos::box2 box = GetPlayerBox();
	box.half_size = glm::vec2(player_length * 0.5f, PLAYER_HEIGHT * 0.5f);
	SetPlayerBox(box);

	LudumLevelInstance * ludum_level_instance = GetLudumLevelInstance();
	if (ludum_level_instance != nullptr)
		ludum_level_instance->RestrictPlayerToWorld(this);
}

bool LudumPlayer::InitializeGameValues(nlohmann::json const& config, boost::filesystem::path const& config_path, bool hot_reload)
{
	if (!death::Player::InitializeGameValues(config, config_path, hot_reload))
		return false;
	DEATHGAME_JSON_ATTRIBUTE(max_life_count);
	return true;
}

void LudumPlayer::OnInputModeChanged(chaos::InputMode new_mode, chaos::InputMode old_mode)
{
	death::Player::OnInputModeChanged(new_mode, old_mode);

	LudumGameInstance* game_instance = GetLudumGameInstance();
	if (game_instance == nullptr)
		return;

	if (game_instance->sequence_challenge != nullptr)
	{
		if (chaos::IsPlatformChanged(new_mode, old_mode))
		{
			game_instance->sequence_challenge->particle_range = game_instance->CreateChallengeParticles(game_instance->sequence_challenge.get());
			game_instance->sequence_challenge->Show(game_instance->game->IsPlaying());
		}
	}
}