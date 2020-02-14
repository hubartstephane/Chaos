#pragma once

#include "Ludum41CustomChallenge.h"
#include "Ludum41CustomParticles.h"

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
	
	friend class LudumChallenge;
	friend class ParticleMovableObjectTrait;
	friend class ParticleLifeObjectTrait;
	friend class ParticleBrickTrait;	

protected:

	/** some aspect constant */
	float CHALLENGE_SIZE = 100.0f;
	float CHALLENGE_PLACEMENT_Y = 0;

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

	/** returns index of any ball matching some condition (ball higher than a limit and going up or ball lower than a limit a going down) */
	size_t CanStartChallengeBallIndex(bool going_down) const;

	/** returns the current combo multiplier */
	int GetCurrentComboMultiplier() const { return combo_multiplier; }

	void TickBrickOffset(double delta_time);

	void TickChallenge(double delta_time);

	void TickBallSplit(double delta_time);

	void TickBackgroundFillRatio(double delta_time);

	virtual void FillUniformProvider(chaos::GPUProgramProvider& main_uniform_provider) override;

	/** test whether a button is being pressed and whether it correspond to the current challenge */
	void SendGamepadButtonToChallenge(chaos::MyGLFW::GamepadData const * in_gamepad_data);
	/** test whether a key is being pressed and whether it correspond to the current challenge */
	void SendKeyboardButtonToChallenge(unsigned int C);
	/** called whenever a challenge is completed */
	void OnChallengeCompleted(LudumChallenge * challenge, bool success, size_t challenge_size);

	/** create a challenge  */
	LudumChallenge * CreateSequenceChallenge();
	/** create a challenge  */
	LudumChallenge * CreateSequenceChallenge(std::string keyboard_challenge);

protected:

	virtual death::Player * DoCreatePlayer() override;

	virtual bool DoTick(double delta_time) override;

	virtual void OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance) override;

	virtual void OnLeavePause() override;

	virtual void OnEnterPause() override;

	virtual void OnInputModeChanged(chaos::InputMode new_mode, chaos::InputMode old_mode) override;

	virtual bool DoCheckGameOverCondition() override;

	/** override */
	virtual void OnPlayerEntered(death::Player * player) override;
	/** override */
	virtual void OnPlayerLeaved(death::Player * player) override;

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
	
	virtual bool Initialize(death::Game * in_game) override;

	/** called whenever a ball collide */
	void OnBallCollide(bool collide_brick);	
	
	/** destroying game objects*/
	void DestroyGameObjects();	

	/** override */
	virtual bool CanCompleteLevel() const override;

protected:

	/** current game values */
	float brick_offset = 0.0f;
	float target_brick_offset = 0.0f;
	int   combo_multiplier = 1;	
	float ball_time_dilation = 1.0f;
	float challenge_timer = 0.0f;
	float ball_collision_speed    = 0.5f;
	float ball_power         = 1.0f;
	float ball_speed         = 5.0f;	

	float current_background_fillratio = 1.0f;
	float complete_level_timer = 0.0f;

	int pending_split_count = 0;
	
	/** some sprites */
	chaos::shared_ptr<chaos::ParticleAllocationBase> balls_allocations;
	/** current the challenge */
	chaos::shared_ptr<LudumChallenge> sequence_challenge;
};
