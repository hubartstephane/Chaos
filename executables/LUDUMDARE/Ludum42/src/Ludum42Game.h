#pragma once

#include "Ludum42StateMachine.h"
#include "Ludum42Particles.h"
#include "Ludum42Level.h"

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

class LudumGame : public death::Game
{
	friend class LudumWindow;
	friend class LudumAutomata;
	friend class MainMenuState;
	friend class PlayingToPauseTransition;
	friend class PauseToPlayingTransition;
	friend class MainMenuToPlayingTransition;
	friend class PlayingToMainMenuTransition;
	friend class PlayingState;
	friend class PlayingToGameOverTransition;

	friend class LudumGameplayLevelInstance;
	friend class LudumNarrativeLevelInstance;

public:

	static int const BACKGROUND_LAYER_ID = death::Game::LAST_LAYER_ID + 1;		
	static int const GROUND_LAYER_ID     = death::Game::LAST_LAYER_ID + 2;
	static int const WALLS_LAYER_ID      = death::Game::LAST_LAYER_ID + 3;
	static int const GAMEOBJECT_LAYER_ID = death::Game::LAST_LAYER_ID + 4;
	static int const PLAYER_LAYER_ID     = death::Game::LAST_LAYER_ID + 5;

	static int const OBJECT_TYPE_PLAYER = 0;

	/** constructor */
	LudumGame();

	/** override */
	virtual bool OnKeyEvent(int key, int action) override;
	/** override */
	virtual bool OnCharEvent(unsigned int c) override;
	/** override */
	virtual void OnMouseMove(double x, double y) override;
	/** override */
	virtual void DoDisplay(chaos::box2 const & viewport, chaos::GPUProgramProvider & uniform_provider) override;
	/** override */
	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override;

	/** called whenever a gamepad input is comming */
	bool OnPhysicalGamepadInput(chaos::MyGLFW::PhysicalGamepad * physical_gamepad);

protected:

	/** internal methods to generate the atlas for sprites */
	virtual bool FillAtlasGenerationInput(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path) override;
	virtual bool FillAtlasGenerationInputWithTileSets(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path);

	/** override */
	virtual bool CreateGameAutomata() override;
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
	
	void TickLevelCompleted(double delta_time);

	virtual bool CheckGameOverCondition(double delta_time) override;

	/** initialize the particle manager */
	virtual bool InitializeParticleManager() override;
	/** initialize the game variables */
	virtual bool InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path) override;
	
	/** loading the levels */
	virtual bool LoadLevels() override;
	/** load one level */
	virtual death::GameLevel * DoLoadLevel(int level_number, chaos::FilePathParam const & path) override;
	/** additionnal initialization when loading a level */
	bool DoLoadLevelInitialize(class LudumGameplayLevel * level, chaos::TiledMap::Map * tiled_map);
	bool DoLoadLevelInitialize(class LudumNarrativeLevel * level, nlohmann::json const & json_level);


	/** called whenever the input mode changes */
	virtual void OnInputModeChanged(int new_mode, int old_mode) override;

	



	/** create one particle for the background */
	void FillBackgroundLayer();

	/** create a number of game object */
	chaos::ParticleAllocation * CreateGameObjects(char const * name, size_t count, int layer_id = GAMEOBJECT_LAYER_ID);
	/** create the player */
	chaos::ParticleAllocation * CreatePlayer();



	/** override */
	virtual chaos::box2 GetPlayerBox() const override;
	/** override */
	virtual bool SetPlayerBox(chaos::box2 const & in_player_box) override;



	/** utility method to move an object */
	chaos::box2 GetObjectBox(chaos::ParticleAllocation const * allocation, size_t index) const;
	/** utility method to move an object */
	bool SetObjectBox(chaos::ParticleAllocation * allocation, size_t index, chaos::box2 const & b);

	


	/** reset the game variables */
	virtual void ResetGameVariables() override;


	/** spawning player */
	bool SpawnPlayer(ParticleObject const & particle_object);
	/** destroying player */
	void UnSpawnPlayer();

	/** get the position of the player */
	bool SetPlayerPosition(glm::vec2 const & position);
	/** get the player position */
	glm::vec2 GetPlayerPosition() const;



	/** change an object position */
	void SetObjectPosition(chaos::ParticleAllocation * allocation, size_t index, glm::vec2 const & position);

	/** ensure object is inside the world */
	void RestrictedObjectToScreen(chaos::ParticleAllocation * allocation, size_t index);
	/** ensure player is inside the world */
	void RestrictedPlayerToScreen();



	/** move the player */
	void DisplacePlayer(double delta_time);

	void ChangeLife(int delta_life);

	/** override */
	virtual void OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance, death::GameLevelInstance * old_level_instance) override;

	/** returns whether current level is completed */
	bool IsLevelCompleted();

protected:

	/** the tiled map manager */
	boost::intrusive_ptr<chaos::TiledMap::Manager> tiledmap_manager;

	/** game settings */
	int initial_life = 3;

	/** current game values */
	int current_life  = 3;
	int current_level = 0;

#if _DEBUG
	bool cheat_next_level = false;
#endif

	/** some sprites */
	boost::intrusive_ptr<chaos::ParticleAllocation> player_allocations;
	boost::intrusive_ptr<chaos::ParticleAllocation> life_allocations;
	boost::intrusive_ptr<chaos::ParticleAllocation> background_allocations;
};
