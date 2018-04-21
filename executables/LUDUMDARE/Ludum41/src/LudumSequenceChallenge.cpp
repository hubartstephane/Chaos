#pragma once

#include "LudumSequenceChallenge.h"
#include "LudumGame.h"


void LudumSequenceChallenge::OnKeyboardButtonReceived(char c)
{
	if (keyboard_challenge[challenge_position] == c)
		AdvanceChallenge();
	else
		OnChallengeError();
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
		for (auto const & it : game->gamepad_button_map)
		{
			if (it.first == expected_key) 
				continue;
		
			if (physical_gamepad->GetButtonChanges(it.first) == chaos::MyGLFW::BUTTON_BECOME_PRESSED)
			{
				OnChallengeError();
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
		if (callbacks != nullptr)
			callbacks->OnChallengeCompleted(this);

		game->OnChallengeCompleted(this); // remove the challenge from pending list
	}
}

void LudumSequenceChallenge::OnChallengeError()
{

}
