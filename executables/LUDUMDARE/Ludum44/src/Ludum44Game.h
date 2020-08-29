#pragma once

#include "Ludum44Particles.h"
#include "Ludum44Level.h"
#include "Ludum44PowerUp.h"

#include <chaos/StandardHeaders.h> 
#include <chaos/Object.h>
#include <chaos/GeometryFramework.h>
#include <chaos/MyGLFWGamepadManager.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/SoundManager.h>
#include <chaos/MyGLFWSingleWindowApplication.h>
#include <chaos/ParticleManager.h>
#include <chaos/ParticleTextGenerator.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/TiledMap.h>
#include <chaos/TiledMapTools.h>
#include <chaos/GPUFramebuffer.h>

#include <death/Game.h>
#include <death/GameFramework.h>


// =================================================
// LudumGame
// =================================================

namespace death
{
	namespace GameHUDKeys
	{
		CHAOS_DECLARE_TAG(PLANETS_LAYER_ID);
		CHAOS_DECLARE_TAG(GAMEOBJECT_LAYER_ID);
		CHAOS_DECLARE_TAG(FIRE_LAYER_ID);
	};
};

class LudumGame : public death::Game
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

	friend class ParticleEnemyTrait;
	friend class SpawnerTrigger;

public:

	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	CHAOS_OBJECT_DECLARE_CLASS2(LudumGame, death::Game);

	/** constructor */
	LudumGame();

	/** override */
	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override;

	float GetBuyUpgradeTime() const { return buy_upgrade_time; }

protected:

	/** override */
	virtual bool OnEnterGame(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad) override;

	/** initialize the game variables */
	virtual bool InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path, bool hot_reload) override;

	/** override level creation */
	death::TMLevel * CreateTMLevel() override;

	/** move the player */
	virtual death::GameHUD * DoCreatePlayingHUD() override;

	bool PopulatePowerUps(nlohmann::json const & config, boost::filesystem::path const & config_path);

	bool PopulatePowerOneUp(LudumPowerUp * power_up, char const * json_name, nlohmann::json const & config, boost::filesystem::path const & config_path);

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

