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
	
		game->OnChallengeCompleted(this);
	}
}

void LudumSequenceChallenge::OnChallengeError()
{

}



#if 0
void LudumGameSendGamepadButtonAction

SendGamepadButtonAction(physical_gamepad);
void SendGamepadButtonAction(chaos::MyGLFW::PhysicalGamepad * physical_gamepad);
float Trigger = physical_gamepad->GetAxisValue(chaos::MyGLFW::XBOX_TRIGGER);

if (Trigger > 0)
Trigger = Trigger;
else if (Trigger < 0)
	Trigger = Trigger;

int b1 = (physical_gamepad->GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_A) == chaos::MyGLFW::BUTTON_BECOME_PRESSED);
int b2 = (physical_gamepad->GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_B) == chaos::MyGLFW::BUTTON_BECOME_PRESSED);
int b3 = (physical_gamepad->GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_X) == chaos::MyGLFW::BUTTON_BECOME_PRESSED);
int b4 = (physical_gamepad->GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_Y) == chaos::MyGLFW::BUTTON_BECOME_PRESSED);
int b5 = (physical_gamepad->GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_LEFTBUT) == chaos::MyGLFW::BUTTON_BECOME_PRESSED);
int b6 = (physical_gamepad->GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_RIGHTBUT) == chaos::MyGLFW::BUTTON_BECOME_PRESSED);

int b7 = (physical_gamepad->GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_LEFTTRIGGER) == chaos::MyGLFW::BUTTON_BECOME_PRESSED);
int b8 = (physical_gamepad->GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_RIGHTTRIGGER) == chaos::MyGLFW::BUTTON_BECOME_PRESSED);

if (b1 || b2 || b3 || b4 || b5 || b6 || b7 | b8)
b1 = b1;
#endif