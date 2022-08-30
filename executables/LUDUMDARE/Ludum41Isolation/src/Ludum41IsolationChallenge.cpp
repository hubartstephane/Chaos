#pragma once

#include "Ludum41IsolationPCH.h"
#include "Ludum41IsolationChallenge.h"
#include "Ludum41IsolationGame.h"
#include "Ludum41IsolationGameInstance.h"

void LudumChallenge::OnKeyboardButtonReceived(char c)
{
	LudumGame * game = game_instance->GetGame();
	if (game->IsFreeCameraMode())
		return;

	// test the challenge
	if (std::toupper(keyboard_challenge[keyboard_challenge_position]) == std::toupper(c))
		AdvanceChallenge();
	else
		OnChallengeError(false);
}

void LudumChallenge::Show(bool visible)
{
	if (particle_range != nullptr)
		particle_range->Show(visible);
}

void LudumChallenge::OnGamepadButtonReceived(chaos::GamepadState const * in_gamepad_state)
{
	LudumGame * game = game_instance->GetGame();
	if (game->IsFreeCameraMode())
		return;
	chaos::WindowApplication* window_application = chaos::Application::GetInstance();
	if (window_application == nullptr)
		return;

	int expected_key = gamepad_challenge[gamepad_challenge_position];

	if (in_gamepad_state->GetButtonStateChange((chaos::GamepadButton)expected_key) == chaos::ButtonStateChange::BECOME_PRESSED)
	{
		AdvanceChallenge();
	}
	else
	{
		// search if a bad button is pressed
		for (auto it = window_application->GetGamepadButtonMap().begin() ; it != window_application->GetGamepadButtonMap().end() ; ++it)
		{
			if (it->first == (chaos::GamepadButton)expected_key)
				continue;

			if (in_gamepad_state->GetButtonStateChange(it->first) == chaos::ButtonStateChange::BECOME_PRESSED)
			{
				OnChallengeError(false);
				return;
			}
		}
	}
}

void LudumChallenge::AdvanceChallenge()
{
	LudumGame * game = game_instance->GetGame();

	// update keyboard position
	++keyboard_challenge_position;
	while (keyboard_challenge_position < keyboard_challenge.size() && keyboard_challenge[keyboard_challenge_position] == ' ')
		++keyboard_challenge_position;
	// compute success for gamepad (much simpler)
	++gamepad_challenge_position;
	if (gamepad_challenge_position == gamepad_challenge.size())
	{
		game->PlaySound("challenge_success", false, false, 0.0f, chaos::SoundContext::GAME);

		game_instance->OnChallengeCompleted(this, true,  gamepad_challenge.size()); // remove the challenge from pending list
	}
}

void LudumChallenge::OnChallengeError(bool out_of_time)
{
	LudumGame * game = game_instance->GetGame();

	if (out_of_time)
		game->PlaySound("challenge_timeout", false, false, 0.0f, chaos::SoundContext::GAME);
	else
		game->PlaySound("challenge_error", false, false, 0.0f, chaos::SoundContext::GAME);

	game_instance->OnChallengeCompleted(this, false, gamepad_challenge.size()); // remove the challenge from pending list
}

void LudumChallenge::SetTimeout(float in_timeout)
{
	timeout = in_timeout;
}


int LudumChallenge::GetTimeSoundIndex(float t) const
{
	float times[] = {0.25f, 0.5f, 0.75f, 1.0f};

	int count = sizeof(times) / sizeof(times[0]);

	// greater values give 0, 1 ..
	if (t > times[count - 1])
		return (int)t;

	// lower values gives -1, -2 ...
	for (int i = 0 ; i < count; ++i)
		if (t < times[i])
			return -1 - i;

	return 0; // should never happen
}

void LudumChallenge::Tick(float delta_time)
{
	LudumGame * game = game_instance->GetGame();

	if (timeout > 0.0f)
	{
		int time_sound_index1 = GetTimeSoundIndex(timeout);

		timeout = timeout - delta_time;
		if (timeout <= 0.0f)
		{
			OnChallengeError(true);
		}
		else
		{
			int time_sound_index2 = GetTimeSoundIndex(timeout);
			if (time_sound_index1 != time_sound_index2)
				game->PlaySound("challenge_bip", false, false, 0.0f, chaos::SoundContext::GAME);
		}
	}
}

LudumChallenge::~LudumChallenge()
{
	game_instance->ball_time_dilation = 1.0;
}

std::string LudumChallenge::NoSpaceKeyboardChallenge(std::string const & keyboard_challenge)
{
	std::string result = keyboard_challenge;
	result.erase(std::remove(result.begin(), result.end(), ' '), result.end());
	return result;
}

size_t LudumChallenge::GetChallengePosition(bool gamepad) const
{
	return (gamepad)?
		gamepad_challenge_position:
		keyboard_challenge_position;
}
