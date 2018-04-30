#pragma once

#include "LudumChallenge.h"
#include "LudumGame.h"


void LudumChallenge::OnKeyboardButtonReceived(char c)
{
	if (keyboard_challenge[challenge_position] == c)
		AdvanceChallenge();
	else
		OnChallengeError(false);
}

void LudumChallenge::Show(bool visible)
{
	if (particle_range != nullptr)
		particle_range->Show(visible);
}

void LudumChallenge::OnGamepadButtonReceived(chaos::MyGLFW::PhysicalGamepad * physical_gamepad)
{
	int expected_key = gamepad_challenge[challenge_position];

	if (physical_gamepad->GetButtonChanges(expected_key) == chaos::MyGLFW::BUTTON_BECOME_PRESSED)
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
		
			if (physical_gamepad->GetButtonChanges(it->first) == chaos::MyGLFW::BUTTON_BECOME_PRESSED)
			{
				OnChallengeError(false);
				return;
			}
		}	
	}
}

void LudumChallenge::AdvanceChallenge()
{
	++challenge_position;
	if (challenge_position == gamepad_challenge.size())
	{			
		game->PlaySound("challenge_success", false, false);

		game->OnChallengeCompleted(this, true,  gamepad_challenge.size()); // remove the challenge from pending list
	}
}

void LudumChallenge::OnChallengeError(bool out_of_time)
{
	if (out_of_time)
		game->PlaySound("challenge_timeout", false, false);
	else
		game->PlaySound("challenge_error", false, false);

	game->OnChallengeCompleted(this, false, gamepad_challenge.size()); // remove the challenge from pending list
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
				game->PlaySound("challenge_bip", false, false);
		}			
	}
}

LudumChallenge::~LudumChallenge()
{
	game->ball_time_dilation = 1.0;
}
