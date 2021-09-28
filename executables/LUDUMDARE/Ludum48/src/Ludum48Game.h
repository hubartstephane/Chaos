#pragma once

#include "Ludum48.h"

#include "Ludum48Particles.h"
#include "Ludum48Level.h"

// =================================================
// LudumGame
// =================================================

class LudumGame : public Game
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

	CHAOS_DECLARE_OBJECT_CLASS2(LudumGame, Game);

	/** constructor */
	LudumGame();

	/** override */
	virtual bool InitializeFromConfiguration(nlohmann::json const & config) override;


protected:


	/** override */
	virtual bool OnEnterGame(PhysicalGamepad * in_physical_gamepad) override;

	/** initialize the game variables */
	virtual bool InitializeGameValues(nlohmann::json const & config, bool hot_reload) override;

	/** override level creation */
	TMLevel * CreateTMLevel() override;

	/** move the player */
	virtual GameHUD * DoCreatePlayingHUD() override;

	virtual void DoDisplayGame(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params) override;

	void SetFadeEffect(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params, bool fade_to_black, float t);

protected:

};

