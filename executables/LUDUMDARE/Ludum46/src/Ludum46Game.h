#pragma once

#include "Ludum46StateMachine.h"
#include "Ludum46Particles.h"
#include "Ludum46Level.h"

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

class LudumGame : public death::Game
{

	
	friend class ParticlePlayerTrait;


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

	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	/** constructor */
	LudumGame(GLFWwindow* in_glfw_window);

	/** override */
	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override;


protected:

	/** override */
	virtual chaos::SM::StateMachine * DoCreateGameStateMachine() override;
	/** override */
	virtual bool DeclareParticleClasses() override;

	/** override */
	virtual bool OnEnterGame(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad) override;

	/** initialize the game variables */
	virtual bool InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path, bool hot_reload) override;

	/** override level creation */
	death::TiledMapLevel * CreateTiledMapLevel() override;

	/** move the player */
	virtual death::GameHUD * DoCreatePlayingHUD() override;

	virtual death::GameInstance * DoCreateGameInstance() override;

	virtual void DoDisplayGame(chaos::GPURenderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::GPURenderParams const & render_params) override;

protected:

};

