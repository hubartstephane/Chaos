#pragma once

#include <chaos/Chaos.h>

#include "Ludum41IsolationChallenge.h"
#include "Ludum41IsolationParticles.h"
#include "Ludum41IsolationChallengeRewardPunishment.h"
#include "Ludum41IsolationLevel.h"
#include "Ludum41IsolationHUD.h"


// =================================================
// LudumGame
// =================================================

class LudumGame : public chaos::Game
{
	friend class LudumChallenge;
	friend class LudumWindow;

	friend class ParticleMovableObjectLayerTrait;
	friend class ParticleLifeLayerTrait;
	friend class ParticleBrickLayerTrait;

public:

	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS2(LudumGame, chaos::Game);

	/** constructor */
	LudumGame();

protected:

	/** override */
	chaos::GameHUD * DoCreatePlayingHUD() override;

	/** override */
	virtual void OnEnterMainMenu(bool very_first) override;

	/** initialize the dictionnary */
	bool InitializeDictionnary(nlohmann::json const & config, boost::filesystem::path const & config_path);
	/** replace some special */
	void ReplaceSpecialLetters(std::string & word) const;
	/** test whether a word only has the common letters (no accent) */
	bool IsWordValid(std::string const & word) const;

	/** override */
	virtual bool InitializeFromConfiguration(nlohmann::json const& config, boost::filesystem::path const& config_path) override;
	/** initialize the particle manager */
	virtual int AddParticleLayers() override;
	/** initialize the game variables */
	virtual bool InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path, bool hot_reload) override;
	/** fullfill the lists of rewards an punishments */
	bool InitializeRewardsAndPunishments();
	
	/** load one level */
	virtual chaos::Level * DoLoadLevel(chaos::FilePathParam const & path) override;

	/** override */
	virtual void OnInputModeChanged(chaos::InputMode new_mode, chaos::InputMode old_mode) override;

protected:

	/** the dictionnary */
	std::map<size_t, std::vector<std::string>> dictionnary;
	/** the min and max size */
	int min_word_size = 0;
	int max_word_size = 0;

	/** all the existing button */
	std::vector<int> gamepad_buttons;

	/** initial game values */

	int max_ball_count = 3;

	float max_brick_offset = 0.0f;
	float brick_offset_speed = 0.0f;
	float brick_offset_increment = 0.0f;

	float player_max_length       = 200.0f; 
	float player_min_length       = 70.0f; 
	float player_initial_length   = 100.0f; 
	float player_length_increment = 50.0f; 
	float player_length_decrement = 50.0f;

	float ball_size          = 25.0f;

	float ball_max_speed     = 5.0f;
	float ball_initial_speed = 1.0f;
	float ball_speed_increment = 0.5f;

	float ball_collision_speed_increment = 0.5f;
	float ball_collision_max_speed = 0.5f;

	float challenge_time_dilation = 0.5f;
	float challenge_frequency = 10.0f;
	float challenge_duration   = 5.0f;
	
	float delay_before_ball_move = 2.0f;

	float background_fillratio_changespeed = 0.01f;

	float delay_before_next_level = 3.0f;

	int min_brick_life = 1;
	float max_brick_life = 5.0f;

	int brick_per_line   = 5;

	int combo_for_reward = 3;

	int points_per_brick = 1;
	int points_per_challenge = 10;

	float split_angle = 20.0f;
	float ball_angle_min = 20.0f;
	float ball_angle_max = 20.0f;

	float rebound_angle_decrease = 10.0f;
	float rebound_angle_increase = 30.0f;

	/** the possible rewards */
	std::vector<chaos::shared_ptr<LudumChallengeRewardPunishment>> rewards;
	/** the possible punishment */
	std::vector<chaos::shared_ptr<LudumChallengeRewardPunishment>> punishments;
};

