#include <chaos/Chaos.h>

#include "Ludum41IsolationPlayer.h"
#include "Ludum41IsolationGame.h"
#include "Ludum41IsolationGameInstance.h"
#include "Ludum41IsolationLevel.h"
#include "Ludum41IsolationLevelInstance.h"

bool LudumPlayer::Initialize(chaos::GameInstance * in_game_instance)
{
	if (!chaos::Player::Initialize(in_game_instance))
		return false;

	LudumGame const * ludum_game = GetGame();
	if (ludum_game != nullptr)
		player_length = ludum_game->player_initial_length;

	return true;
}

void LudumPlayer::TickPlayerDisplacement(float delta_time)
{
	float value = left_stick_position.x;
	if (abs(right_stick_position.x) > abs(left_stick_position.x))
		value = right_stick_position.x;
	DisplacePlayerRacket(value * GetGame()->GetGamepadSensitivity() * delta_time); // even if 0 because this will ensure player Y is well placed even if no input is pressed
}

void LudumPlayer::DisplacePlayerRacket(float delta_x)
{
	LudumLevelInstance* ludum_level_instance = GetLevelInstance();
	if (ludum_level_instance == nullptr)
		return;

	chaos::box2 level_box = GetLevelInstance()->GetBoundingBox();
	if (pawn != nullptr)
	{
		glm::vec2 position = pawn->GetPosition();
		pawn->SetPosition(glm::vec2(position.x + delta_x, level_box.position.y - level_box.half_size.y + PLAYER_Y));


		LudumLevelInstance* ludum_level_instance = GetLevelInstance();
		if (ludum_level_instance != nullptr)
			ludum_level_instance->RestrictPawnToWorld(pawn.get());
	}
}


bool LudumPlayer::OnMouseMoveImpl(double x, double y)
{
	chaos::Game const * game = GetGame();
	if (game == nullptr)
		return true;
	if (game->IsPaused() || game->IsFreeCameraMode())
		return true;
	DisplacePlayerRacket(game->GetMouseSensitivity() * (float)x);
	return true;
}

bool LudumPlayer::OnCharEventImpl(unsigned int c)
{
	LudumGameInstance * ludum_game_instance = GetGameInstance();

	// CHALLENGE
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
	{
		ludum_game_instance->SendKeyboardButtonToChallenge((char)c);
		return true;
	}
	return chaos::Player::OnCharEventImpl(c);
}

void LudumPlayer::InternalHandleGamepadInputs(float delta_time, chaos::MyGLFW::GamepadData const * gpd)
{
	chaos::Player::InternalHandleGamepadInputs(delta_time, gpd);

	LudumGameInstance * ludum_game_instance = GetGameInstance();
	ludum_game_instance->SendGamepadButtonToChallenge(gpd);
}

void LudumPlayer::SetPlayerLength(float in_length, bool increment)
{
	LudumGame const* ludum_game = GetGame();

	if (increment)
		player_length += in_length;
	else
		player_length = in_length;

	player_length = std::clamp(player_length, ludum_game->player_min_length, ludum_game->player_max_length);

	if (pawn != nullptr)
	{
		chaos::box2 box = pawn->GetBoundingBox();
		box.half_size = glm::vec2(player_length * 0.5f, PLAYER_HEIGHT * 0.5f);
		pawn->SetBoundingBox(box);

		LudumLevelInstance* ludum_level_instance = GetLevelInstance();
		if (ludum_level_instance != nullptr)
			ludum_level_instance->RestrictPawnToWorld(pawn.get());
	}
}

bool LudumPlayer::InitializeGameValues(nlohmann::json const& config, boost::filesystem::path const& config_path, bool hot_reload)
{
	if (!chaos::Player::InitializeGameValues(config, config_path, hot_reload))
		return false;
	CHAOS_JSON_ATTRIBUTE(config, max_life_count);
	return true;
}

void LudumPlayer::OnInputModeChanged(chaos::InputMode new_mode, chaos::InputMode old_mode)
{
	chaos::Player::OnInputModeChanged(new_mode, old_mode);

	LudumGameInstance* game_instance = GetGameInstance();
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

bool LudumPlayer::IsDead() const
{
	if (chaos::Player::IsDead())
		return true;

	LudumGameInstance const* ludum_game_instance = GetGameInstance();
	if (ludum_game_instance != nullptr)
	{
		if (ludum_game_instance->GetBallCount() == 0)
			return true;
	}
	return false;
}

void LudumPlayer::OnLifeLost()
{
	chaos::Player::OnLifeLost();

	LudumGame* ludum_game = GetGame();
	LudumGameInstance* ludum_game_instance = GetGameInstance();

	ludum_game->PlaySound("balllost", false, false, 0.0f, chaos::SoundContext::GAME);
	ludum_game_instance->combo_multiplier = 1;
	ludum_game_instance->ball_collision_speed = 0.0f;
	ludum_game_instance->ball_power = 1.0f;
	ludum_game_instance->ball_speed = ludum_game->ball_initial_speed;
	ludum_game_instance->pending_split_count = 0;
	ludum_game_instance->target_brick_offset = 0.0f;
	ludum_game_instance->ball_time_dilation = 1.0f;
	ludum_game_instance->challenge_timer = ludum_game->challenge_frequency;

	SetPlayerLength(ludum_game->player_initial_length, false);

	ludum_game_instance->balls_allocations = ludum_game_instance->CreateBalls(1, true);
}
