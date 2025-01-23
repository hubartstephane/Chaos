#pragma once

#include "Ludum47PCH.h"
#include "Ludum47Particles.h"
#include "Ludum47Level.h"

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

	CHAOS_DECLARE_OBJECT_CLASS(LudumGame, chaos::Game);

	/** constructor */
	LudumGame();

	/** override */
	virtual bool InitializeFromConfiguration(nlohmann::json const * config) override;


protected:

	/** override */
	virtual bool OnEnterGame(chaos::PhysicalGamepad * in_physical_gamepad) override;

	/** initialize the game variables */
	virtual bool OnReadConfigurableProperties(chaos::JSONReadConfiguration config, chaos::ReadConfigurablePropertiesContext context) override;

	/** override level creation */
	chaos::TMLevel * CreateTMLevel() override;

	/** move the player */
	virtual chaos::GameHUD * DoCreatePlayingHUD() override;

	virtual void DoDisplayGame(chaos::GPURenderContext * render_context, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::GPURenderParams const & render_params) override;

protected:

};

