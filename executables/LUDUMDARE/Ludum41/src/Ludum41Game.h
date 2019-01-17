#pragma once

#include "Ludum41StateMachine.h"
#include "Ludum41Challenge.h"
#include "Ludum41Particles.h"
#include "Ludum41ChallengeRewardPunishment.h"
#include "Ludum41Level.h"
#include "Ludum41HUD.h"

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
#include <chaos/GeometryFramework.h>
#include <chaos/MyGLFWGamepadManager.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/SoundManager.h>
#include <chaos/MyGLFWSingleWindowApplication.h>
#include <chaos/ParticleManager.h>
#include <chaos/ParticleTextGenerator.h>
#include <chaos/FilePath.h>

#include <death/Game.h>
#include <death/GameHUD.h>

// =================================================
// LudumGame
// =================================================

class LudumGame : public death::Game
{
	friend class LudumChallenge;
	friend class LudumWindow;

	friend class LudumChallengeRewardPunishment_ExtraLife;
	friend class LudumChallengeRewardPunishment_BarSize;
	friend class LudumChallengeRewardPunishment_BrickLife;
	friend class LudumChallengeRewardPunishment_SpeedDownBall;
	friend class LudumChallengeRewardPunishment_SplitBall;
	friend class LudumChallengeRewardPunishment_BallPower;
	friend class LudumChallengeRewardPunishment_BrickOffset;

	friend class ParticleMovableObjectTrait;
	friend class ParticleLifeObjectTrait;
	friend class ParticleBrickTrait;

	friend class LudumLevel;
	friend class LudumLevelInstance;

protected:

	/** some aspect constant */
	float CHALLENGE_SIZE = 100.0f;
	float CHALLENGE_PLACEMENT_Y = 0;

	float PLAYER_Y = -385.0f;
	float PLAYER_HEIGHT = 35.0f;

public:

	/** constructor */
	LudumGame();

	/** returns the current combo multiplier */
	int GetCurrentComboMultiplier() const { return combo_multiplier; }

	/** override */
	virtual bool OnKeyEvent(int key, int action) override;
	/** override */
	virtual bool OnCharEvent(unsigned int c) override;
	/** override */
	virtual bool OnMouseMove(double x, double y) override;
	/** override */
	virtual void DoDisplay(chaos::Renderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::RenderParams const & render_params) override;
	/** override */
	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override;

protected:

	/** override */
	death::GameHUD * DoCreatePlayingHUD() override;

	/** creating all object in the game */
	void CreateAllGameObjects(int level);
	/** destroying game objects*/
	void DestroyGameObjects();

	/** override */
	virtual chaos::SM::StateMachine * DoCreateGameStateMachine() override;
	/** override */
	virtual bool DeclareParticleClasses() override;
	/** override */
	virtual bool OnGamepadInput(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad) override;

	



	/** override */
	virtual void OnEnterMainMenu(bool very_first) override;
	/** override */
	virtual void OnGameOver() override;
	/** override */
	virtual bool OnEnterPause() override;
	/** override */
	virtual bool OnLeavePause() override;

	/** override */
	virtual bool OnEnterGame() override;
	/** override */
	virtual bool OnLeaveGame(bool gameover) override;
	/** override */
	virtual bool OnAbordGame() override;

	/** override */
	virtual bool TickGameLoop(double delta_time) override;

	
	
	
	void TickBrickOffset(double delta_time);

	void TickChallenge(double delta_time);

	void TickBallSplit(double delta_time);

	

	virtual bool CheckGameOverCondition() override;

	/** initialize the dictionnary */
	bool InitializeDictionnary(nlohmann::json const & config, boost::filesystem::path const & config_path);
	/** replace some special */
	void ReplaceSpecialLetters(std::string & word) const;
	/** test whether a word only has the common letters (no accent) */
	bool IsWordValid(std::string const & word) const;

	/** initialize the mapping between button index and resource name */
	virtual bool InitializeGamepadButtonInfo() override;
	/** initialize the particle manager */
	virtual int AddParticleLayers() override;
	/** initialize the game variables */
	bool InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path, bool hot_reload) override;
	/** fullfill the lists of rewards an punishments */
	bool InitializeRewardsAndPunishments();
	
	/** load one level */
	virtual death::GameLevel * DoLoadLevel(int level_index, chaos::FilePathParam const & path) override;


	
	/** get a random button in existing list */
	int GetRandomButtonID() const;

	/** override */
	virtual void OnInputModeChanged(int new_mode, int old_mode) override;


	/** test whether a button is being pressed and whether it correspond to the current challenge */
	void SendGamepadButtonToChallenge(chaos::MyGLFW::GamepadData * in_gamepad_data);
	/** test whether a key is being pressed and whether it correspond to the current challenge */
	void SendKeyboardButtonToChallenge(unsigned int C);

	/** create a challenge for a given name */
	LudumChallenge * CreateSequenceChallenge(size_t len);

	/** called whenever a challenge is completed */
	void OnChallengeCompleted(LudumChallenge * challenge, bool success, size_t challenge_size);

	/** create a text for the challenge */
	chaos::ParticleAllocation * CreateChallengeParticles(LudumChallenge * challenge);

	/** create a string for a gamepad challenge */
	std::string GenerateGamepadChallengeString(std::vector<int> const & gamepad_challenge);

	/** create the player */
	chaos::ParticleAllocation * CreatePlayer();

	/** create the ball */
	chaos::ParticleAllocation * CreateBalls(size_t count, bool full_init);
	/** create the bricks */
	chaos::ParticleAllocation * CreateBricks(LudumLevel const * level);

	/** update the score */
	void IncrementScore(int delta);

	/** reset the game variables */
	virtual void ResetGameVariables() override;

	/** called whenever a ball collide */
	void OnBallCollide(bool collide_brick);
	/** change the length */
	void SetPlayerLength(float length);



	/** get the balls */
	ParticleMovableObject * GetBallParticles();
	/** get the balls */
	ParticleMovableObject const * GetBallParticles() const;
	/** get the number of balls */	
	size_t GetBallCount() const;

	/** get the bricks */
	ParticleBrick * GetBricks();
	/** get the bricks */
	ParticleBrick const * GetBricks() const;

	/** get the number of bricks */
	size_t GetBrickCount() const;


	/** returns true whether we can start a challenge (returns index of a valid ball) */
	size_t CanStartChallengeBallIndex(bool reverse = false) const;


	/** ensure object is inside the world */
	void RestrictObjectToWorld(chaos::ParticleAllocation * allocation, size_t index);
	/** ensure player is inside the world */
	void RestrictPlayerToWorld();


	/** generate a direction updward random for the ball */
	glm::vec2 GenerateBallRandomDirection() const;

	/** move the player */
	void DisplacePlayer(double delta_time);


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
	
	virtual void OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance, death::GameLevelInstance * old_level_instance) override;

protected:

	/** the dictionnary */
	std::map<size_t, std::vector<std::string>> dictionnary;
	/** the min and max size */
	int min_word_size = 0;
	int max_word_size = 0;

	/** the challenge */
	chaos::shared_ptr<LudumChallenge> sequence_challenge;

	/** all the existing button */
	std::vector<int> gamepad_buttons;

	/** initial game values */

	int max_ball_count = 3;

	float brick_offset = 0.0f;
	float target_brick_offset = 0.0f;
	float max_brick_offset = 0.0f;
	float brick_offset_speed = 0.0f;
	float brick_offset_increment = 0.0f;

	float player_max_length       = 200.0f; 
	float player_min_length       = 70.0f; 
	float player_initial_length   = 100.0f; 
	float player_length_increment = 50.0f; 
	float player_length_decrement = 50.0f;

	float ball_size          = 25.0f;

	float ball_power         = 1.0f;
	float ball_speed         = 5.0f;
	float ball_max_speed     = 5.0f;
	float ball_initial_speed = 1.0f;
	float ball_speed_increment = 0.5f;

	float ball_collision_speed    = 0.5f;
	float ball_collision_speed_increment = 0.5f;
	float ball_collision_max_speed = 0.5f;

	float challenge_time_dilation = 0.5f;
	float challenge_frequency = 10.0f;
	float challenge_duration   = 5.0f;
	
	float delay_before_ball_move = 2.0f;

	int min_brick_life = 1;
	float max_brick_life = 5.0f;

	int pending_split_count = 0;

	int brick_per_line   = 5;
	int brick_line_count = 5;

	int points_per_brick = 1;
	int points_per_challenge = 10;

	float split_angle = 0.1f;
	float ball_angle_limit = 0.1f;

	/** current game values */
	float player_length = 70.0f;
	
	float ball_time_dilation = 1.0f;
	float challenge_timer = 0.0f;

	
	int combo_multiplier = 1;
	
	/** some sprites */	
	chaos::shared_ptr<chaos::ParticleAllocation> bricks_allocations;
	chaos::shared_ptr<chaos::ParticleAllocation> balls_allocations;

	/** the possible rewards */
	std::vector<chaos::shared_ptr<LudumChallengeRewardPunishment>> rewards;
	/** the possible punishment */
	std::vector<chaos::shared_ptr<LudumChallengeRewardPunishment>> punishments;
};

