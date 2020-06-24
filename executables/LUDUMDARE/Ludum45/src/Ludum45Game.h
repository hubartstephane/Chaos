#pragma once

#include "Ludum45Particles.h"
#include "Ludum45Level.h"
#include "Ludum45Enemies.h"

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

class LudumGame : public death::Game
{

	
	friend class ParticlePlayerTrait;


	friend class LudumWindow;
	friend class MainMenuState;
	friend class PlayingToPauseTransition;
	friend class PauseToPlayingTransition;
	friend class MainMenuToPlayingTransition;
	friend class PlayingToMainMenuTransition;
	friend class PlayingState;
	friend class PlayingToGameOverTransition;

	friend class LudumLevelInstance;



public:

	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	CHAOS_OBJECT_DECLARE_CLASS2(LudumGame, death::Game);

	/** constructor */
	LudumGame();

	/** override */
	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override;


	EnemyType * FindEnemyType(char const * name);
	EnemyType const * FindEnemyType(char const * name) const;


protected:

	/** override */
	virtual bool OnEnterGame(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad) override;

	/** initialize the game variables */
	virtual bool InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path, bool hot_reload) override;

	/** override level creation */
	death::TiledMapLevel * CreateTiledMapLevel() override;

	/** move the player */
	virtual death::GameHUD * DoCreatePlayingHUD() override;

	virtual void DoDisplayGame(chaos::GPURenderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::GPURenderParams const & render_params) override;

	

	void RegisterEnemyTypes();

	void DoRegisterEnemyType(chaos::TiledMap::ObjectTypeDefinition const * def);

	template<typename FUNC>
	void RegisterObjectTypeDefinition(char const * prefix, FUNC func);

protected:

	std::vector<chaos::shared_ptr<EnemyType>> enemy_types;

	float player_speed_factor = 500.0f;
	float player_speed_damping = 0.95f;
	float player_tan_speed_damping = 0.95f;
	float player_dash_velocity_boost = 500.0f;
	float player_dash_duration       = 0.5f;

	float fire_size_ratio = 0.3f;
	float fire_velocity = 800.0f;

	float scroll_factor = 1.0f;
	
	std::vector<float> player_speeds;
	std::vector<float> player_damages;
	std::vector<float> player_dash_cooldowns;
	std::vector<float> player_power_rates;
	std::vector<int>   player_power_spreads;	
	std::vector<float> player_specialpowers;	
};

