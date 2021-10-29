#pragma once

#include <chaos/Chaos.h>

#include "Ludum44Particles.h"
#include "Ludum44Level.h"
#include "Ludum44PowerUp.h"

// =================================================
// LudumGame
// =================================================

class LudumGame : public chaos::Game
{
	friend class LudumWindow;
	friend class MainMenuState;
	friend class PlayingToPauseTransition;
	friend class PauseToPlayingTransition;
	friend class MainMenuToPlayingTransition;
	friend class PlayingToMainMenuTransition;
	friend class PlayingState;
	friend class PlayingToGameOverTransition;

	friend class LudumLevelInstance;

	friend class LudumPowerUp;
	friend class LudumSpeedUp;
	friend class LudumDamageUp;
	friend class LudumFireRateUp;

	friend class ParticleEnemyLayerTrait;
	friend class SpawnerTrigger;

public:

	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS(LudumGame, chaos::Game);

	/** constructor */
	LudumGame();

	/** override */
	virtual bool InitializeFromConfiguration(nlohmann::json const & config) override;

	float GetBuyUpgradeTime() const { return buy_upgrade_time; }

protected:

	/** override */
	virtual bool OnEnterGame(chaos::PhysicalGamepad * in_physical_gamepad) override;

	/** initialize the game variables */
	virtual bool InitializeGameValues(nlohmann::json const & config, bool hot_reload) override;

	/** override level creation */
	chaos::TMLevel * CreateTMLevel() override;

	/** move the player */
	virtual chaos::GameHUD * DoCreatePlayingHUD() override;

	bool PopulatePowerUps(nlohmann::json const & config);

	bool PopulatePowerOneUp(LudumPowerUp * power_up, char const * json_name, nlohmann::json const & config);

protected:

	float min_player_max_health = 0.1f;

	std::vector<float> player_speeds;
	std::vector<float> player_damages;
	std::vector<float> player_charged_damages;
	std::vector<int>   player_fire_rates;

	


	/** a multiplier for speed */
	float player_speed_factor = 500.0f;

	/** the buy upgrade time */
	float buy_upgrade_time = 3.0f;
	/** the super fire time */
	float charged_fire_time = 3.0f;
	/** the normal fire time */
	float normal_fire_time = 0.1f;

	/** bullet velocity */
	float fire_velocity = 800.0f;
	/** enemy bullet velocity */
	float enemy_fire_velocity = 800.0f;

	/** damaged cause by enemy fire */
	float enemy_fire_damage = 0.1f;
	/** enemy fire rate */
	float enemy_fire_rate = 1.0f;

	/** a scroll multiplier */
	float scroll_factor = 1.0f;

	float meteor_health = 3.0f;
	float alien_health = 3.0f;
	float turret_health = 3.0f;
		
	float meteor_damage_for_player = 0.5f;
	float alien_damage_for_player = 0.5f;
	float turret_damage_for_player = 0.5f;


	/** the power ups */
	std::vector<chaos::shared_ptr<LudumPowerUp>> power_ups;




};

