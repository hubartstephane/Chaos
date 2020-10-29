#pragma once

#include <chaos/Chaos.h>

#include "Ludum46Particles.h"
#include "Ludum46Level.h"

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

	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	CHAOS_OBJECT_DECLARE_CLASS2(LudumGame, chaos::Game);

	/** constructor */
	LudumGame();

	/** override */
	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override;


protected:

	/** override */
	virtual bool OnEnterGame(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad) override;

	/** initialize the game variables */
	virtual bool InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path, bool hot_reload) override;

	/** override level creation */
	chaos::TMLevel * CreateTMLevel() override;

	/** move the player */
	virtual chaos::GameHUD * DoCreatePlayingHUD() override;

	virtual void DoDisplayGame(chaos::GPURenderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::GPURenderParams const & render_params) override;

protected:

};

