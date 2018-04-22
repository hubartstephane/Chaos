#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
#include <chaos/MyGLFWGamepadManager.h>
#include <chaos/ParticleManager.h>

class LudumSequenceChallengeCallbacks : public chaos::ReferencedObject
{
public:

	/** called whenever the challenge is completed */
	virtual void OnChallengeCompleted(class LudumSequenceChallenge * challenge){}
	/** called whenever the challenge is failed */
	virtual void OnChallengeFailed(class LudumSequenceChallenge * challenge){}
};

class LudumSequenceChallenge_ExtraBallCallbacks : public LudumSequenceChallengeCallbacks
{
public:
	/** called whenever the challenge is completed */
	virtual void OnChallengeCompleted(class LudumSequenceChallenge * challenge);
};

class LudumSequenceChallenge_LongBarBallCallbacks : public LudumSequenceChallengeCallbacks
{
public:
	/** called whenever the challenge is completed */
	virtual void OnChallengeCompleted(class LudumSequenceChallenge * challenge);
};


class LudumSequenceChallenge_LifeBallCallbacks : public LudumSequenceChallengeCallbacks
{
public:
	/** called whenever the challenge is completed */
	virtual void OnChallengeCompleted(class LudumSequenceChallenge * challenge);
};

class LudumSequenceChallenge_SpeedDownBallCallbacks : public LudumSequenceChallengeCallbacks
{
public:
	/** called whenever the challenge is completed */
	virtual void OnChallengeCompleted(class LudumSequenceChallenge * challenge);
};


class LudumSequenceChallenge : public chaos::ReferencedObject
{
	friend class LudumGame;

public:

	/** try to catch button input */
	void OnGamepadButtonReceived(chaos::MyGLFW::PhysicalGamepad * physical_gamepad);
	/** try to catch keyboard input */
	void OnKeyboardButtonReceived(char c);

	/** returns the position inside the challenge */
	size_t GetChallengePosition() const { return challenge_position; }

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

	/** the game */
	class LudumGame * game = nullptr;

	/** some callbacks */
	boost::intrusive_ptr<LudumSequenceChallengeCallbacks> callbacks;

	/** the visible hud */
	boost::intrusive_ptr<chaos::ParticleRangeAllocation> particle_range;
};