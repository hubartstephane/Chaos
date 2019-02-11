#pragma once

#include "Ludum41Challenge.h"

#include <chaos/StandardHeaders.h> 
#include <death/Game.h>
#include <death/GameInstance.h>
#include <death/GameFramework.h>


// =========================================================
// LudumGameInstance
// =========================================================

class LudumGameInstance : public death::GameInstance
{

	friend class LudumChallengeRewardPunishment_ExtraLife;
	friend class LudumChallengeRewardPunishment_BarSize;
	friend class LudumChallengeRewardPunishment_BrickLife;
	friend class LudumChallengeRewardPunishment_SpeedDownBall;
	friend class LudumChallengeRewardPunishment_SplitBall;
	friend class LudumChallengeRewardPunishment_BallPower;
	friend class LudumChallengeRewardPunishment_BrickOffset;

public:

	DEATH_GAMEFRAMEWORK_DECLARE_GAMEINSTANCE(Ludum);

	/** constructor */
	LudumGameInstance(death::Game * in_game);

	/** some challenges */
	void OnBrickLifeChallenge(bool success);
	bool IsBrickLifeChallengeValid(bool success);
	/** some challenges */
	void OnBallSpeedChallenge(bool success);
	bool IsBallSpeedChallengeValid(bool success);
	/** some challenges */
	void OnBrickOffsetChallenge(bool success);
	bool IsBrickOffsetChallengeValid(bool success);
	/** some challenges */
	void OnBallPowerChallenge(bool success);
	bool IsBallPowerChallengeValid(bool success);
	/** some challenges */
	void OnSplitBallChallenge(bool success);
	bool IsSplitBallChallengeValid(bool success);
	/** some challenges */
	void OnExtraBallChallenge(bool success);
	bool IsExtraBallChallengeValid(bool success);
	/** some challenges */
	void OnLongBarChallenge(bool success);
	bool IsLongBarChallengeValid(bool success);

	/** update the score */
	void IncrementScore(int delta);

	/** returns the current combo multiplier */
	int GetCurrentComboMultiplier() const { return combo_multiplier; }

	void TickBrickOffset(double delta_time);

	void TickChallenge(double delta_time);

	void TickBallSplit(double delta_time);

	/** test whether a button is being pressed and whether it correspond to the current challenge */
	void SendGamepadButtonToChallenge(chaos::MyGLFW::GamepadData const * in_gamepad_data);
	/** test whether a key is being pressed and whether it correspond to the current challenge */
	void SendKeyboardButtonToChallenge(unsigned int C);
	/** called whenever a challenge is completed */
	void OnChallengeCompleted(LudumChallenge * challenge, bool success, size_t challenge_size);

	/** create a challenge for a given name */
	LudumChallenge * CreateSequenceChallenge(size_t len);

protected:

	virtual death::Player * DoCreatePlayer() override;

	virtual bool DoTick(double delta_time) override;

	virtual void OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance) override;

	virtual bool OnLeavePause() override;

	virtual bool OnEnterPause() override;

	virtual bool OnCharEvent(unsigned int c) override;

	virtual bool OnGamepadInput(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad);

protected:

	float brick_offset = 0.0f;
	float target_brick_offset = 0.0f;
	int   combo_multiplier = 1;


	/** current the challenge */
	chaos::shared_ptr<LudumChallenge> sequence_challenge;
};
