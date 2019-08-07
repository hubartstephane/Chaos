#pragma once

#include "Ludum41Challenge.h"
#include "Ludum41Game.h"
#include "Ludum41GameInstance.h"

void LudumChallenge::OnKeyboardButtonReceived(char c)
{
	// test the challenge
	if (keyboard_challenge[keyboard_challenge_position] == c)
		AdvanceChallenge();
	else
		OnChallengeError(false);
}

void LudumChallenge::Show(bool visible)
{
	if (particle_range != nullptr)
		particle_range->Show(visible);
}

void LudumChallenge::OnGamepadButtonReceived(chaos::MyGLFW::GamepadData const * in_gamepad_data)
{
	LudumGame * game = game_instance->GetLudumGame();

	int expected_key = gamepad_challenge[gamepad_challenge_position];

	if (in_gamepad_data->GetButtonChanges(expected_key) == chaos::MyGLFW::BUTTON_BECOME_PRESSED)
	{
		AdvanceChallenge();
	}
	else
	{
		// search if a bad button is pressed
		for (auto it = game->gamepad_button_map.begin() ; it != game->gamepad_button_map.end() ; ++it)
		{
			if (it->first == expected_key) 
				continue;
		
			if (in_gamepad_data->GetButtonChanges(it->first) == chaos::MyGLFW::BUTTON_BECOME_PRESSED)
			{
				OnChallengeError(false);
				return;
			}
		}	
	}
}

void LudumChallenge::AdvanceChallenge()
{
	LudumGame * game = game_instance->GetLudumGame();

	// update keyboard position
	++keyboard_challenge_position;
	while (keyboard_challenge_position < keyboard_challenge.size() && keyboard_challenge[keyboard_challenge_position] == ' ')
		++keyboard_challenge_position;
	// compute success for gamepad (much simpler)
	++gamepad_challenge_position;
	if (gamepad_challenge_position == gamepad_challenge.size())
	{			
		game->Play("challenge_success", false, false);

		game_instance->OnChallengeCompleted(this, true,  gamepad_challenge.size()); // remove the challenge from pending list
	}
}

void LudumChallenge::OnChallengeError(bool out_of_time)
{
	LudumGame * game = game_instance->GetLudumGame();

	if (out_of_time)
		game->Play("challenge_timeout", false, false);
	else
		game->Play("challenge_error", false, false);

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

void LudumChallenge::Tick(double delta_time)
{
	LudumGame * game = game_instance->GetLudumGame();

	if (timeout > 0.0f)
	{
		int time_sound_index1 = GetTimeSoundIndex(timeout);
	
		timeout = timeout - (float)delta_time;
		if (timeout <= 0.0f)
		{
			OnChallengeError(true);		
		}
		else
		{
			int time_sound_index2 = GetTimeSoundIndex(timeout);
			if (time_sound_index1 != time_sound_index2)
				game->Play("challenge_bip", false, false);
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
