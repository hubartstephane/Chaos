#pragma once

#include "Ludum44StateMachine.h"
#include "Ludum44Particles.h"
#include "Ludum44Level.h"
#include "Ludum44PowerUp.h"

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
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

class GameValue
{
public:

	float initial_value = 1.0f;
	float max_value = 5.0f;
	float increment_value = 0.5f;
};


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
	friend class LudumStateMachine;
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

public:

	DEATH_GAMEFRAMEWORK_DECLARE_GAME(Ludum);

	/** constructor */
	LudumGame();

	/** override */
	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override;

	virtual chaos::box2 GetWorldBox() const override;


	ParticlePlayer * GetPlayerParticle(int player_index);

	ParticlePlayer const * GetPlayerParticle(int player_index) const;

	float GetBuyUpgradeTime() const { return buy_upgrade_time; }

protected:

	/** override */
	virtual chaos::SM::StateMachine * DoCreateGameStateMachine() override;
	/** override */
	virtual bool DeclareParticleClasses() override;

	/** override */
	virtual bool OnEnterGame(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad) override;

	/** the game main loop */
	virtual void OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance);

	/** initialize the game variables */
	virtual bool InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path, bool hot_reload) override;

	virtual void DoDisplayGame(chaos::Renderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::RenderParams const & render_params) override;
	
	virtual bool InitializeParticleTextGenerator() override;

	/** override level creation */
	death::TiledMap::Level * CreateTiledMapLevel() override;

	/** called whenever the input mode changes */
	virtual void OnInputModeChanged(int new_mode, int old_mode) override;

	/** move the player */
	virtual death::GameHUD * DoCreatePlayingHUD() override;

	virtual death::GameInstance * CreateGameInstance() override;

	virtual int AddParticleLayers() override;


	bool PopulatePowerUps(nlohmann::json const & config, boost::filesystem::path const & config_path);

	bool PopulatePowerOneUp(LudumPowerUp * power_up, char const * json_name, nlohmann::json const & config, boost::filesystem::path const & config_path);

protected:

	GameValue player_life; // XXX : only use initial value for the moment

	GameValue player_speed;
	GameValue player_damage;
	GameValue player_charged_damage;
	GameValue player_fire_rate;

	float min_player_max_life = 0.1f;


	/** a multiplier for speed */
	float player_speed_factor = 500.0f;

	/** the buy upgrade time */
	float buy_upgrade_time = 3.0f;
	/** the super fire time */
	float charged_fire_time = 3.0f;

	/** bullet velocity */
	float fire_velocity = 800.0f;

	/** a scroll multiplier */
	float scroll_factor = 1.0f;

	/** the power ups */
	std::vector<chaos::shared_ptr<LudumPowerUp>> power_ups;




};

