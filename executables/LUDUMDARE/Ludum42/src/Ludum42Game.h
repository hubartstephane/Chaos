#pragma once

#include "Ludum42StateMachine.h"
#include "Ludum42Particles.h"

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

class LudumLevel : public chaos::ReferencedObject
{
public:

	int level_number = 0;

	boost::intrusive_ptr<chaos::TiledMap::Map> tiled_map;
};

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



protected:

	float TITLE_SIZE = 150.0f;
	float TITLE_PLACEMENT_Y = 0;


public:

	/** destructor */
	~LudumGame();


	/** override */
	virtual bool OnKeyEvent(int key, int action) override;
	/** override */
	virtual bool OnCharEvent(unsigned int c) override;
	/** override */
	virtual void OnMouseMove(double x, double y) override;
	/** override */
	virtual void Display(glm::ivec2 const & size) override;
	/** override */
	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override;







	/** called whenever a gamepad input is comming */
	bool OnPhysicalGamepadInput(chaos::MyGLFW::PhysicalGamepad * physical_gamepad);

protected:

	/** internal methods to generate the atlas for sprites */
	virtual bool FillAtlasGenerationInput(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path) override;
	virtual bool FillAtlasGenerationInputWithTileSets(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path);

	/** override */
	virtual bool LoadBestScore(std::ifstream & file) override;
	/** override */
	virtual bool SaveBestScore(std::ofstream & file) override;
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
	virtual void TickGameLoop(double delta_time) override;











	/** the game main loop */
	
	void TickLevelCompleted(double delta_time);

	void TickHeartWarning(double delta_time);

	bool TickGameOverDetection(double delta_time);

	/** initialize the particle manager */
	virtual bool InitializeParticleManager() override;
	/** initialize the game variables */
	virtual bool InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path) override;
	
	/** loading the levels */
	virtual bool LoadLevels() override;
	/** load one level */
	bool DoLoadLevel(int level_number, chaos::TiledMap::Map * tiled_map);
	/** additionnal initialization when loading a level */
	bool DoLoadLevelInitialize(LudumLevel * level);

	/** get the size of the world */
	glm::vec2 GetWorldSize() const;
	/** get the box world */
	chaos::box2 GetWorldBox() const;

	/** called whenever the input mode changes */
	virtual void OnInputModeChanged(int new_mode, int old_mode) override;

	



	/** create one particle for the background */
	void FillBackgroundLayer();



	/** create particles in the text layer */
	chaos::ParticleAllocation * CreateTextParticles(char const * text, chaos::ParticleTextGenerator::GeneratorParams const & params);

	/** create a number of game object */
	chaos::ParticleAllocation * CreateGameObjects(char const * name, size_t count, int layer_id = GAMEOBJECT_LAYER_ID);
	/** create the player */
	chaos::ParticleAllocation * CreatePlayer();

	/** create the ball */
	chaos::ParticleAllocation * CreateBalls(size_t count, bool full_init);


	/** create the title of the game */
	void CreateGameTitle();
	/** create the title */
	void CreateTitle(char const * title, bool normal);
	/** create the title */
	void DestroyTitle();

	/** update the score */
	void IncrementScore(int delta);

	/** reset the game variables */
	void ResetGameVariables();


	/** get the position of the player */
	void SetPlayerPosition(float position);
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



	/** create the score allocation */
	void UpdateScoreParticles();
	/** create the combo allocation */
	void UpdateComboParticles();
	/** create the life allocation */
	void UpdateLifeParticles();

	void ChangeLife(int delta_life);

	/** internal method to create score/combo */
	chaos::ParticleAllocation * CreateScoringParticles(bool & update_flag, char const * format, int value, float Y);


	/** get currently played level */
	LudumLevel * GetCurrentLevel();
	/** get currently played level */
	LudumLevel const * GetCurrentLevel() const;

	/** get currently played level */
	LudumLevel * GetLevel(int level_number);
	/** get currently played level */
	LudumLevel const * GetLevel(int level_number) const;
	/** returns whether current level is completed */
	bool IsLevelCompleted();



protected:

	/** the tiled map manager */
	boost::intrusive_ptr<chaos::TiledMap::Manager> tiledmap_manager;

	/** game settings */
	int   initial_life = 3;
	float heart_beat_speed = 2.0f;

	/** current game values */
	int current_life  = 3;
	int current_level = 0;
	int current_score = 0;
	int current_combo_multiplier = 1;

	/** the best score value */
	int best_score = 0;

	/** some data */
	bool should_update_score = false;
	bool should_update_combo = false;
	float heart_warning = 0.0f;

#if _DEBUG
	bool cheat_next_level = false;
#endif

	/** some sprites */
	boost::intrusive_ptr<chaos::ParticleAllocation> player_allocations;
	boost::intrusive_ptr<chaos::ParticleAllocation> life_allocations;
	boost::intrusive_ptr<chaos::ParticleAllocation> text_allocations;
	boost::intrusive_ptr<chaos::ParticleAllocation> score_allocations;
	boost::intrusive_ptr<chaos::ParticleAllocation> combo_allocations;
	boost::intrusive_ptr<chaos::ParticleAllocation> best_score_allocations;
	boost::intrusive_ptr<chaos::ParticleAllocation> background_allocations;

	/** the levels */
	std::vector<boost::intrusive_ptr<LudumLevel>> levels;
};
