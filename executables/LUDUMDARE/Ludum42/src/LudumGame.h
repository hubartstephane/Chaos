#pragma once

#include "LudumStateMachine.h"
#include "LudumChallenge.h"
#include "LudumParticles.h"
#include "LudumChallengeRewardPunishment.h"

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>
#include <chaos/GeometryFramework.h>
#include <chaos/MyGLFWGamepadManager.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/SoundManager.h>
#include <chaos/MyGLFWSingleWindowApplication.h>
#include <chaos/ParticleManager.h>
#include <chaos/ParticleTextGenerator.h>


class LudumLevel
{
public:

	int level_number = 0;

};

// =================================================
// LudumGame
// =================================================

class LudumGame : public chaos::ReferencedObject
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

	friend class ParticleMovableObjectTrait;
	friend class ParticleLifeObjectTrait;
	friend class ParticleBrickTrait;

protected:

	float TITLE_SIZE = 150.0f;
	float TITLE_PLACEMENT_Y = 0;


public:

	/** destructor */
	~LudumGame();

	/** the tick method */
	void Tick(double delta_time);
	/** whenever a key event is received */
	bool OnKeyEvent(int key, int action);
	/** whenever a char event is received */
	bool OnCharEvent(unsigned int c);
	/** whenever a mouse event is received */
	void OnMouseButton(int button, int action, int modifier);
	/** whenever mouse is displaced */
	void OnMouseMove(double x, double y);
	/** the rendering method */
	void Display(chaos::box2 const & viewport);
	/** called whenever a gamepad input is comming */
	bool OnPhysicalGamepadInput(chaos::MyGLFW::PhysicalGamepad * physical_gamepad);

	/** basic initialization */
	bool InitializeGame(GLFWwindow * in_glfw_window);
	/** initialization from the config file */
	bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path);

protected:

	/** getter on the application */
	chaos::MyGLFW::SingleWindowApplication * GetApplication();
	/** getter on the sound manager */
	chaos::SoundManager * GetSoundManager();

	/** internal methods to generate the atlas for sprites */
	bool GenerateAtlas(nlohmann::json const & config, boost::filesystem::path const & config_path);

	/** internal method called to reset cached inputs */
	void ResetPlayerCachedInputs();
	/** internal method to test for keys and update stick position */
	void HandleKeyboardInputs();

	/** require a pause or resume */
	bool RequireTogglePause();
	/** require a game Start */
	bool RequireStartGame();
	/** require a game exit */
	bool RequireExitGame();
	/** require a game over */
	bool RequireGameOver();

	/** creating all object in the game */
	void CreateAllGameObjects(int level);
	/** destroying game objects*/
	void DestroyGameObjects();

	/** create the music used in the game */
	void CreateAllMusics();
	/** create one sound */
	chaos::Sound * PlaySound(char const * name, bool paused, bool looping);
	/** blend out a music */
	void BlendMusic(chaos::Sound * music, bool blend_in);
	/** start music[0], stop all others */
	void ChangeMusic(chaos::Sound ** musics, size_t count, bool restart_first);


	/** called on the very first time the game is started */
	void OnStartGame(bool very_first);
	/** called whenever the game is lost */
	void OnGameOver();
	/** called whenever we enter in pause mode */
	bool OnEnterPause();
	/** called whenever we leave pause mode */
	bool OnLeavePause();

	/** called whenever we enter in game mode */
	bool OnEnterGame();
	/** called whenever we leave game mode */
	bool OnLeaveGame();

	/** returns true if the pause if fully set */
	bool IsPauseEnterComplete();
	/** returns true if the game if fully restored from pause */
	bool IsPauseLeaveComplete();

	/** returns true if the game enter if fully set */
	bool IsGameEnterComplete();
	/** returns true if the game leave is fully completed */
	bool IsGameLeaveComplete();

	/** the game main loop */
	void TickGameLoop(double delta_time);
	
	void TickLevelCompleted(double delta_time);

	void TickHeartWarning(double delta_time);

	bool TickGameOverDetection(double delta_time);

	/** change the game music */
	void StartMainMenuMusic(bool restart_first);
	void StartGameMusic(bool restart_first);
	void StartPauseMusic(bool restart_first);

	/** get current state ID */
	int GetCurrentStateID() const;

	/** initialize the mapping between button index and resource name */
	bool InitializeGamepadButtonInfo();
	/** initialize the particle manager */
	bool InitializeParticleManager();
	/** initialize the particle text generator */
	bool InitializeParticleTextGenerator();
	/** initialize the game variables */
	bool InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path);
	/** fullfill the lists of rewards an punishments */
	bool InitializeRewardsAndPunishments();
	
	/** loading the levels */
	bool LoadLevels();
	/** load one level */
	bool DoLoadLevel(int level_number, std::vector<std::string> & level_content);


	
	/** get a random button in existing list */
	int GetRandomButtonID() const;


	/** get the size of the world */
	glm::vec2 GetWorldSize() const;
	/** get the box world */
	chaos::box2 GetWorldBox() const;

	/** called whenever the input mode changes */
	void OnInputModeChanged(int new_mode, int old_mode);

	



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





	/** get an object particle */
	ParticleObject * GetObjectParticle(chaos::ParticleAllocation * allocation, size_t index);
	/** get an object particle */
	ParticleObject const * GetObjectParticle(chaos::ParticleAllocation * allocation, size_t index) const;

	/** get the player particle */
	ParticleObject * GetPlayerParticle();
	/** get the player particle */
	ParticleObject const * GetPlayerParticle() const;

	/** get the balls */
	ParticleMovableObject * GetBallParticles();
	/** get the balls */
	ParticleMovableObject const * GetBallParticles() const;
	/** get the number of balls */	
	size_t GetBallCount() const;

	/** get the bricks */
	ParticleBrick * GetBricks();
	/** get the bricks */
	ParticleBrick const * GetBricks() const;



	/** get the box for a given object */
	chaos::box2 GetObjectBox(chaos::ParticleAllocation * allocation, size_t index) const;
	/** get the box for player */
	chaos::box2 GetPlayerBox() const;

	/** change one object box */
	void SetObjectBox(chaos::ParticleAllocation * allocation, size_t index, chaos::box2 const & box);
	/** change player box */
	void SetPlayerBox(chaos::box2 const & box);

	/** get the position for given object */
	glm::vec2 GetObjectPosition(chaos::ParticleAllocation * allocation, size_t index) const;
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




	/** returns true whether we are playing */
	bool IsPlaying() const;

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

	/** Save the best score */
	void SerializeBestScore(bool save);

protected:

	/** the window in GLFW library */
	GLFWwindow * glfw_window = nullptr;

	/** the automata corresponding to the game */
	boost::intrusive_ptr<LudumAutomata> game_automata;

	/** the current stick position */
	glm::vec2 left_stick_position  = glm::vec2(0.0f, 0.0f);
	glm::vec2 right_stick_position = glm::vec2(0.0f, 0.0f);

	/** the sounds being played */
	boost::intrusive_ptr<chaos::Sound> menu_music;
	boost::intrusive_ptr<chaos::Sound> game_music;
	boost::intrusive_ptr<chaos::Sound> pause_music;

	/** the current gamepad manager */
	boost::intrusive_ptr<chaos::MyGLFW::GamepadManager> gamepad_manager;

	/** the texture atlas */
	boost::intrusive_ptr<chaos::BitmapAtlas::TextureArrayAtlas> texture_atlas;

	/** the particle manager */
	boost::intrusive_ptr<chaos::ParticleManager> particle_manager;

	/** the text generator */
	boost::intrusive_ptr<chaos::ParticleTextGenerator::Generator> particle_text_generator;


	/** game values */
	int initial_life = 3;
	float mouse_sensitivity = 1.0f;
	float gamepad_sensitivity = 1.0f;
	float heart_beat_speed = 2.0f;


	/** current game values */
	int   current_life  = 3;

	int current_score = 0;
	int combo_multiplier = 1;
	int best_score = 0;
	int current_level = 0;

	bool should_update_score = false;
	bool should_update_combo = false;
	float heart_warning = 0.0f;

#if _DEBUG
	bool cheat_next_level = false;
#endif

	/** some sprites */
	boost::intrusive_ptr<chaos::ParticleAllocation> player_allocations;
	boost::intrusive_ptr<chaos::ParticleAllocation> bricks_allocations;
	boost::intrusive_ptr<chaos::ParticleAllocation> life_allocations;
	boost::intrusive_ptr<chaos::ParticleAllocation> balls_allocations;
	boost::intrusive_ptr<chaos::ParticleAllocation> text_allocations;
	boost::intrusive_ptr<chaos::ParticleAllocation> score_allocations;
	boost::intrusive_ptr<chaos::ParticleAllocation> combo_allocations;
	boost::intrusive_ptr<chaos::ParticleAllocation> best_score_allocations;
	boost::intrusive_ptr<chaos::ParticleAllocation> background_allocations;


	/** the levels */
	std::vector<boost::intrusive_ptr<LudumLevel>> levels;

};

// =================================================
// LudumGamepadManager
// =================================================

class LudumGamepadManager : public chaos::MyGLFW::GamepadManager
{
public:

	/** the constructor */
	LudumGamepadManager(LudumGame * in_game) : game(in_game) {}

protected:

	/** the gamepad manager */
	virtual bool DoPoolGamepad(chaos::MyGLFW::PhysicalGamepad * physical_gamepad) override;

protected:

	/** pointer on the game */
	LudumGame * game = nullptr;
};