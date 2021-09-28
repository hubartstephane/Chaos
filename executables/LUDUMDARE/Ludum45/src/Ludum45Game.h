#pragma once

#include <chaos/Chaos.h>

#include "Ludum45Particles.h"
#include "Ludum45Level.h"
#include "Ludum45Enemies.h"

// =================================================
// LudumGame
// =================================================

class LudumGame : public chaos::Game
{

	
	friend class ParticlePlayerLayerTrait;


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

	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS2(LudumGame, chaos::Game);

	/** constructor */
	LudumGame();

	/** override */
	virtual bool InitializeFromConfiguration(nlohmann::json const & config) override;


	EnemyType * FindEnemyType(char const * name);
	EnemyType const * FindEnemyType(char const * name) const;


protected:

	/** override */
	virtual bool OnEnterGame(chaos::PhysicalGamepad * in_physical_gamepad) override;

	/** initialize the game variables */
	virtual bool InitializeGameValues(nlohmann::json const & config, bool hot_reload) override;

	/** override level creation */
	chaos::TMLevel * CreateTMLevel() override;

	/** move the player */
	virtual chaos::GameHUD * DoCreatePlayingHUD() override;

	virtual void DoDisplayGame(chaos::GPURenderer * renderer, chaos::GPUProgramProviderBase const* uniform_provider, chaos::GPURenderParams const & render_params) override;

	

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

