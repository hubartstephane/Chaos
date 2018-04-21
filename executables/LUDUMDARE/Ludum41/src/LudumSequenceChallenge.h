#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
#include <chaos/MyGLFWGamepadManager.h>

class LudumSequenceChallenge : public chaos::ReferencedObject
{
	friend class LudumGame;

public:

	/** try to catch button input */
	void OnGamepadButtonReceived(chaos::MyGLFW::PhysicalGamepad * physical_gamepad);
	/** try to catch keyboard input */
	void OnKeyboardButtonReceived(char c);

protected:

	/** the challenge progress */
	void AdvanceChallenge();
	/** the challenge error */
	void OnChallengeError();

protected:

	/** the challenge in gamepad mode */
	std::vector<int> gamepad_challenge;
	/** the challenge in keyboard mode */
	std::string keyboard_challenge;

	/** the position in the challenge */
	size_t challenge_position = 0;


	class LudumGame * game = nullptr;
};