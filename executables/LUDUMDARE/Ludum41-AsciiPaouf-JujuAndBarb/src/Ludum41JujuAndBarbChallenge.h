#pragma once

#include "Ludum41JujuAndBarbPCH.h"

class LudumChallenge : public chaos::Object
{
	friend class LudumGameInstance;
	friend class LudumPlayer;

public:

	/** destructor */
	virtual ~LudumChallenge();

	/** try to catch button input */
	void OnGamepadButtonReceived(chaos::GamepadState const * in_gamepad_state);
	/** try to catch keyboard input */
	void OnKeyboardButtonReceived(char c);

	/** returns the position inside the challenge */
	size_t GetChallengePosition(bool gamepad) const;

	/** challenge can be ticked */
	void Tick(float delta_time);

	/** a timeout can be given */
	void SetTimeout(float in_timeout);

	/** show / hide */
	void Show(bool visible);

	/** convert the keyboard_challenge into a string with no space */
	static std::string NoSpaceKeyboardChallenge(std::string const & keyboard_challenge);

	class LudumGameInstance * GetGameInstance() { return game_instance; }

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
	size_t gamepad_challenge_position = 0;
	/** the position in the challenge */
	size_t keyboard_challenge_position = 0;

	/** time out (no time out by default) */
	float timeout = -1.0f;

	/** the game */
	class LudumGameInstance * game_instance = nullptr;

	/** the visible hud */
	chaos::shared_ptr<chaos::ParticleAllocationBase> particle_range;
};