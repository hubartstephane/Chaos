#pragma once

#include "LudumSequenceChallenge.h"
#include "LudumGame.h"


void LudumSequenceChallenge::OnKeyboardButtonReceived(char c)
{
	if (keyboard_challenge[challenge_position] == c)
		AdvanceChallenge();
	else
		OnChallengeError(false);
}

void LudumSequenceChallenge::OnGamepadButtonReceived(chaos::MyGLFW::PhysicalGamepad * physical_gamepad)
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

void LudumSequenceChallenge::AdvanceChallenge()
{
	++challenge_position;
	if (challenge_position == gamepad_challenge.size())
	{			
		game->PlaySound("challenge_success", false, false);

		if (callbacks != nullptr)
			callbacks->OnChallengeCompleted(game, this);

		game->OnChallengeCompleted(this, true); // remove the challenge from pending list
	}
}

void LudumSequenceChallenge::OnChallengeError(bool out_of_time)
{
	if (out_of_time)
		game->PlaySound("challenge_timeout", false, false);
	else
		game->PlaySound("challenge_error", false, false);

	if (callbacks != nullptr)
		callbacks->OnChallengeFailed(game, this);

	game->OnChallengeCompleted(this, false); // remove the challenge from pending list
}

void LudumSequenceChallenge::SetTimeout(float in_timeout)
{
	timeout = in_timeout;
}


int LudumSequenceChallenge::GetTimeSoundIndex(float t) const
{
	float times[4] = {0.25f, 0.5f, 0.75f, 1.0f};

	for (int i = 0 ; i < 4 ; ++i)
		if (t < times[i])
			return i;

	return (int)(t + 1.0f);
}

void LudumSequenceChallenge::Tick(double delta_time)
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



LudumSequenceChallenge::~LudumSequenceChallenge()
{
	game->ball_time_dilation = 1.0;
}










void LudumSequenceChallenge_ExtraBallCallbacks::OnChallengeCompleted(class LudumGame * game, class LudumSequenceChallenge * challenge)
{
	if (game != nullptr)
		game->OnExtraBallChallenge(this, true);
}

void LudumSequenceChallenge_ExtraBallCallbacks::OnChallengeFailed(class LudumGame * game, class LudumSequenceChallenge * challenge)
{
	if (game != nullptr)
		game->OnExtraBallChallenge(this, false);
}


void LudumSequenceChallenge_LongBarBallCallbacks::OnChallengeCompleted(class LudumGame * game, class LudumSequenceChallenge * challenge)
{
	if (game != nullptr)
		game->OnLongBarChallenge(this, true);
}

void LudumSequenceChallenge_LongBarBallCallbacks::OnChallengeFailed(class LudumGame * game, class LudumSequenceChallenge * challenge)
{
	if (game != nullptr)
		game->OnLongBarChallenge(this, false);
}

void LudumSequenceChallenge_LifeBallCallbacks::OnChallengeCompleted(class LudumGame * game, class LudumSequenceChallenge * challenge)
{
	if (game != nullptr)
		game->OnLifeChallenge(this, true);
}

void LudumSequenceChallenge_LifeBallCallbacks::OnChallengeFailed(class LudumGame * game, class LudumSequenceChallenge * challenge)
{
	if (game != nullptr)
		game->OnLifeChallenge(this, false);
}

void LudumSequenceChallenge_SpeedDownBallCallbacks::OnChallengeCompleted(class LudumGame * game, class LudumSequenceChallenge * challenge)
{
	if (game != nullptr)
		game->OnBallSpeedChallenge(this, true);
}

void LudumSequenceChallenge_SpeedDownBallCallbacks::OnChallengeFailed(class LudumGame * game, class LudumSequenceChallenge * challenge)
{
	if (game != nullptr)
		game->OnBallSpeedChallenge(this, false);
}



void LudumSequenceChallenge_SplitBallCallbacks::OnChallengeCompleted(class LudumGame * game, class LudumSequenceChallenge * challenge)
{
	if (game != nullptr)
		game->OnSplitBallChallenge(this, true);
}

void LudumSequenceChallenge_SplitBallCallbacks::OnChallengeFailed(class LudumGame * game, class LudumSequenceChallenge * challenge)
{
	if (game != nullptr)
		game->OnSplitBallChallenge(this, false);
}





