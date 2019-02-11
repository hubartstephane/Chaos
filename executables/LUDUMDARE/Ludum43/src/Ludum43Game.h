#pragma once

#include "Ludum43StateMachine.h"
#include "Ludum43Particles.h"
#include "Ludum43Level.h"

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

namespace death
{
	namespace GameHUDKeys
	{
		CHAOS_DECLARE_TAG(PLANETS_LAYER_ID);
		CHAOS_DECLARE_TAG(GAMEOBJECT_LAYER_ID);
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

public:

	DEATH_GAMEFRAMEWORK_DECLARE_GAME(Ludum);

	/** constructor */
	LudumGame();

	/** override */
	virtual void DoDisplay(chaos::Renderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::RenderParams const & render_params) override;
	/** override */
	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override;

	/** called whenever a gamepad input is comming */
	void RegisterEnemiesInRange(glm::vec2 const & center, float radius, std::vector<ParticleEnemy> & enemy_particles, char const * layer_name, bool take_all);

	virtual chaos::box2 GetWorldBox() const override;

	float GetPlayerLife(int player_index) const;

	ParticlePlayer * GetPlayerParticle(int player_index);

	ParticlePlayer const * GetPlayerParticle(int player_index) const;

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
	
	/** override level creation */
	death::TiledMap::Level * CreateTiledMapLevel() override;

	/** called whenever the input mode changes */
	virtual void OnInputModeChanged(int new_mode, int old_mode) override;

	/** move the player */
	virtual death::GameHUD * DoCreatePlayingHUD() override;

	bool GenerateFramebuffer(glm::ivec2 const & size, chaos::shared_ptr<chaos::GPUFramebuffer> & in_framebuffer);

	virtual death::GameInstance * CreateGameInstance() override;

protected:

	/** the render buffers */
	chaos::shared_ptr<chaos::GPUFramebuffer> framebuffer_worldlimits;
	chaos::shared_ptr<chaos::GPUFramebuffer> framebuffer_other;

	/** the tiled map manager */
	chaos::shared_ptr<chaos::TiledMap::Manager> tiledmap_manager;

public:

	/** game settings */
	float cooldown = 0.1f;

	float dash_duration = 0.5f;
	float dash_cooldown = 1.0f;
	float dash_velocity = 200.0f;

	float player_attraction_minradius = 50.0f;
	float player_attraction_maxradius = 200.0f;
	float player_tangent_force        = 500000.0f;
	float player_attraction_force     = 20.0f;
	float player_repulsion_force      = 20.0f;
	
	float player_slowing_factor       = 0.5f;
	float player_max_velocity         = 20.0f;
	float player_acceleration         = 2000.0f;

	float worldlimits_attraction_maxradius_offset = 0.0f;
	float worldlimits_attraction_minradius_offset = 0.0f;
	float enemy_attraction_maxradius_offset = 200.0f;
	float enemy_attraction_minradius_offset = 50.0f;

	float worldlimits_attraction_maxradius_factor = 1.0f;
	float worldlimits_attraction_minradius_factor = 0.5f;
	float enemy_attraction_maxradius_factor = 1.0f;
	float enemy_attraction_minradius_factor = 1.0f;

	float enemy_tangent_force        = 500000.0f;
	float enemy_attraction_force     = 20.0f;
	float enemy_repulsion_force      = 20.0f;
	


	float particle_slowing_factor     = 0.5f;
	float particle_min_radius_factor = 1.0f;
	float particle_max_radius_factor = 3.0f;
	
	float particle_max_velocity = 20.0f;

	float world_clamp_radius = 3000.0f;

	/** current game values */
	float initial_player_life = 4.0f;
	float initial_particle_life = 1.0f;
};

