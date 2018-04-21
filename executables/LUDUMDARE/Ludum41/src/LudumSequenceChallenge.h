#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>

class LudumSequenceChallenge : public chaos::ReferencedObject
{
public:



protected:

	/** the challenge in gamepad mode */
	std::vector<int> gamepad_challenge;
	/** the challenge in keyboard mode */
	std::string keyboard_challenge;
};