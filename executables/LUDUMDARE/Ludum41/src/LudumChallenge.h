#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
#include <chaos/MyGLFWGamepadManager.h>
#include <chaos/ParticleManager.h>

class LudumChallenge : public chaos::ReferencedObject
{
	friend class LudumGame;

public:

	/** destructor */
	virtual ~LudumChallenge();

	/** try to catch button input */
	void OnGamepadButtonReceived(chaos::MyGLFW::PhysicalGamepad * physical_gamepad);
	/** try to catch keyboard input */
	void OnKeyboardButtonReceived(char c);

	/** returns the position inside the challenge */
	size_t GetChallengePosition() const { return challenge_position; }

	/** challenge can be ticked */
	void Tick(double delta_time);

	/** a timeout can be given */
	void SetTimeout(float in_timeout);

	/** show / hide */
	void Show(bool visible);

protected:

	/** the challenge progress */
	void AdvanceChallenge();
	/** the challenge error */
	void OnChallengeError(bool out_of_time);

	/** get an index of time to know if we must trigger some sound event */
	int GetTimeSoundIndex(float t) const;

protected:

	/** the challenge in gamepad mode */
	std::vector<int> gamepad_challenge;
	/** the challenge in keyboard mode */
	std::string keyboard_challenge;

	/** the position in the challenge */
	size_t challenge_position = 0;

	/** time out (no time out by default) */
	float timeout = -1.0f; 

	/** the game */
	class LudumGame * game = nullptr;

	/** the visible hud */
	boost::intrusive_ptr<chaos::ParticleAllocation> particle_range;
};