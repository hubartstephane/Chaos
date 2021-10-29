#pragma once

#include <chaos/Chaos.h>

#include "Ludum41Challenge.h"
#include "Ludum41Particles.h"

// =========================================================
// LudumGameInstance
// =========================================================

class LudumGameInstance : public chaos::GameInstance
{

	friend class LudumChallengeRewardPunishment_ExtraLife;
	friend class LudumChallengeRewardPunishment_BarSize;
	friend class LudumChallengeRewardPunishment_BrickLife;
	friend class LudumChallengeRewardPunishment_SpeedDownBall;
	friend class LudumChallengeRewardPunishment_SplitBall;
	friend class LudumChallengeRewardPunishment_BallPower;
	friend class LudumChallengeRewardPunishment_BrickOffset;
	
	friend class LudumChallenge;
	friend class ParticleMovableObjectLayerTrait;
	friend class ParticleLifeLayerTrait;
	friend class ParticleBrickLayerTrait;	

protected:

	/** some aspect constant */
	float CHALLENGE_SIZE = 100.0f;
	float CHALLENGE_PLACEMENT_Y = 0;

public:

	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS(LudumGameInstance, chaos::GameInstance);

	LudumGameInstance();

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

	/** returns index of any ball matching some condition (ball higher than a limit and going up or ball lower than a limit a going down) */
	size_t CanStartChallengeBallIndex(bool going_down) const;

	/** returns the current combo multiplier */
	int GetCurrentComboMultiplier() const { return combo_multiplier; }

	void TickBrickOffset(float delta_time);

	void TickChallenge(float delta_time);

	void TickBallSplit(float delta_time);

	/** test whether a button is being pressed and whether it correspond to the current challenge */
	void SendGamepadButtonToChallenge(chaos::GamepadState const * in_gamepad_state);
	/** test whether a key is being pressed and whether it correspond to the current challenge */
	void SendKeyboardButtonToChallenge(unsigned int C);
	/** called whenever a challenge is completed */
	void OnChallengeCompleted(LudumChallenge * challenge, bool success, size_t challenge_size);

	/** create a challenge  */
	LudumChallenge * CreateSequenceChallenge();
	/** create a challenge  */
	LudumChallenge * CreateSequenceChallenge(std::string keyboard_challenge);

protected:

	virtual bool DoTick(float delta_time) override;

	virtual void OnLevelChanged(chaos::Level * new_level, chaos::Level * old_level, chaos::LevelInstance * new_level_instance) override;

	virtual void OnLeavePause() override;

	virtual void OnEnterPause() override;

	/** generate a direction updward random for the ball */
	glm::vec2 GenerateBallRandomDirection() const;
	/** create the ball */
	chaos::ParticleAllocationBase * CreateBalls(size_t count, bool full_init);


	/** get the balls */
	ParticleMovableObject * GetBallParticles();
	/** get the balls */
	ParticleMovableObject const * GetBallParticles() const;
	/** get the number of balls */	
	size_t GetBallCount() const;
	
	
	/** get a random button in existing list */
	int GetRandomButtonID() const;	
	/** create a text for the challenge */
	chaos::ParticleAllocationBase * CreateChallengeParticles(LudumChallenge * challenge);
	/** create a string for a gamepad challenge */
	std::string GenerateGamepadChallengeString(std::vector<int> const & gamepad_challenge);	
	
	virtual bool Initialize(chaos::Game * in_game) override;

	/** called whenever a ball collide */
	void OnBallCollide(bool collide_brick);	
	
	/** destroying game objects*/
	void DestroyGameObjects();	

protected:

	/** current game values */
	float brick_offset = 0.0f;
	float target_brick_offset = 0.0f;
	int   combo_multiplier = 0;	
	float ball_time_dilation = 1.0f;
	float challenge_timer = 0.0f;
	float ball_collision_speed    = 0.5f;
	float ball_power         = 1.0f;
	float ball_speed         = 5.0f;	

	int pending_split_count = 0;
	
	/** some sprites */
	chaos::shared_ptr<chaos::ParticleAllocationBase> balls_allocations;
	/** current the challenge */
	chaos::shared_ptr<LudumChallenge> sequence_challenge;
};
