#pragma once

#include "LudumTemplateStateMachine.h"
#include "LudumTemplateParticles.h"
#include "LudumTemplateLevel.h"

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

#include <death/Game.h>

// =================================================
// LudumGame
// =================================================

namespace death
{
	namespace GameHUDKeys
	{
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

public:

	/** constructor */
	LudumGame();

	/** override */
	virtual bool OnKeyEvent(int key, int action) override;
	/** override */
	virtual bool OnCharEvent(unsigned int c) override;
	/** override */
	virtual bool OnMouseMove(double x, double y) override;
	/** override */
	virtual void DoDisplay(chaos::Renderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::RenderParams const & render_params) override;
	/** override */
	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override;

	/** called whenever a gamepad input is comming */
	bool OnPhysicalGamepadInput(chaos::MyGLFW::PhysicalGamepad * physical_gamepad);

protected:

	/** override */
	virtual chaos::SM::StateMachine * DoCreateGameStateMachine() override;
	/** override */
	virtual bool DeclareParticleClasses() override;

	/** creating all object in the game */
	void CreateAllGameObjects(int level);
	/** destroying game objects*/
	void DestroyGameObjects();

	/** override */
	virtual void OnEnterMainMenu(bool very_first) override;
	/** override */
	virtual void OnGameOver() override;
	/** override */
	virtual bool OnEnterPause() override;
	/** override */
	virtual bool OnLeavePause() override;

	/** override */
	virtual bool OnEnterGame() override;
	/** override */
	virtual bool OnLeaveGame(bool gameover) override;
	/** override */
	virtual bool OnAbordGame() override;

	/** override */
	virtual bool TickGameLoop(double delta_time) override;











	/** the game main loop */
	virtual bool CheckGameOverCondition() override;

	/** initialize the particle manager */
	virtual int AddParticleLayers() override;
	/** initialize the game variables */
	virtual bool InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path, bool hot_reload) override;
	
	/** override level creation method */
	virtual death::TiledMap::Level * CreateTiledMapLevel() override;

	/** called whenever the input mode changes */
	virtual void OnInputModeChanged(int new_mode, int old_mode) override;

	/** create the player */
	chaos::ParticleAllocation * CreatePlayer();

	/** create the ball */
	chaos::ParticleAllocation * CreateBalls(size_t count, bool full_init);


	/** reset the game variables */
	virtual void ResetGameVariables() override;

	/** ensure object is inside the world */
	void RestrictedObjectToScreen(chaos::ParticleAllocation * allocation, size_t index);
	/** ensure player is inside the world */
	void RestrictedPlayerToScreen();



	/** move the player */

	void ChangeLife(int delta_life);

	/** override */
	virtual void OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance, death::GameLevelInstance * old_level_instance) override;

protected:

	/** game settings */
	int initial_life = 3;

	/** current game values */
	int current_life  = 3;
	int current_level = 0;

	/** some sprites */
	chaos::shared_ptr<chaos::ParticleAllocation> life_allocations;
};
