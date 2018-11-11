#pragma once

#include "Ludum42TwoStateMachine.h"
#include "Ludum42TwoParticles.h"
#include "Ludum42TwoLevel.h"

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

namespace death
{
	namespace GameHUDKeys
	{
		CHAOS_STATIC_TAG(BACKGROUND_LAYER_ID);
		CHAOS_STATIC_TAG(GROUND_LAYER_ID);
		CHAOS_STATIC_TAG(WALLS_LAYER_ID);
		CHAOS_STATIC_TAG(GAMEOBJECT_LAYER_ID);
		CHAOS_STATIC_TAG(FIRE_LAYER_ID);
		CHAOS_STATIC_TAG(WATER_LAYER_ID);
	};
};

// =================================================
// LudumGame
// =================================================

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

	static int const OBJECT_TYPE_PLAYER = 0;
	static int const OBJECT_TYPE_WALL   = 1;
	static int const OBJECT_TYPE_FIRE   = 2;

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
	virtual bool OnPhysicalGamepadInput(chaos::MyGLFW::PhysicalGamepad * physical_gamepad) override;

	virtual bool OnGamepadInput(chaos::MyGLFW::GamepadData & in_gamepad_data) override;

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

	/** cooldown the weapon */
	void TickCooldown(double delta_time);











	/** the game main loop */
	
	void TickLevelCompleted(double delta_time);

	virtual bool CheckGameOverCondition(double delta_time) override;

	/** initialize the particle manager */
	virtual bool InitializeParticleManager() override;
	/** initialize the game variables */
	virtual bool InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path) override;

	/** level creation override */
	virtual death::TiledMap::Level * CreateTiledMapLevel() override;


	/** called whenever the input mode changes */
	virtual void OnInputModeChanged(int new_mode, int old_mode) override;

	



	/** create one particle for the background */
	void FillBackgroundLayer();

	/** create a number of game object */
	chaos::ParticleAllocation * CreateGameObjects(char const * name, size_t count, chaos::TagType layer_id = death::GameHUDKeys::GAMEOBJECT_LAYER_ID);
	/** fill game object texture coordinates (from the last of the array) */
	bool InitializeGameObjects(chaos::ParticleAllocation * allocation, char const * name, size_t count);
	/** create the player */
	chaos::ParticleAllocation * CreatePlayer();

	ParticlePlayer * GetPlayerParticle();


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

	chaos::ParticleAllocation * SpawnObjects(chaos::TagType layer_id, size_t count);

	/** get the position of the player */
	bool SetPlayerPosition(glm::vec2 const & position);
	/** get the player position */
	glm::vec2 GetPlayerPosition() const;



	/** change an object position */
	void SetObjectPosition(chaos::ParticleAllocation * allocation, size_t index, glm::vec2 const & position);

	/** ensure object is inside the world */
	void RestrictObjectToWorld(chaos::ParticleAllocation * allocation, size_t index);
	/** ensure player is inside the world */
	void RestrictPlayerToWorld();

	void PlayerThrowWater();

	/** move the player */
	void DisplacePlayer(double delta_time);

	void ChangeLife(int delta_life);

	/** override */
	virtual void OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance, death::GameLevelInstance * old_level_instance) override;

	/** returns whether current level is completed */
	bool IsLevelCompleted();

protected:

	/** game settings */
	int initial_life = 3;
	float cooldown = 0.1f;
	float water_speed = 1.0f;
	float water_lifetime = 1.0f;
	

	/** current game values */
	int current_life     = 3;
	float current_cooldown = 0.1f;
	

#if _DEBUG
	bool cheat_next_level = false;
#endif

	/** some sprites */
	boost::intrusive_ptr<chaos::ParticleAllocation> player_allocations;
	boost::intrusive_ptr<chaos::ParticleAllocation> water_allocations;
	boost::intrusive_ptr<chaos::ParticleAllocation> life_allocations;
	boost::intrusive_ptr<chaos::ParticleAllocation> background_allocations;
};

