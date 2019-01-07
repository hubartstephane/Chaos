#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/MyGLFWwindow.h>
#include <chaos/MyGLFWGamepadManager.h>
#include <chaos/Application.h>
#include <chaos/MyGLFWSingleWindowApplication.h>
#include <chaos/SoundManager.h>
#include <chaos/GeometryFramework.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/ParticleManager.h>
#include <chaos/ParticleTextGenerator.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/StateMachine.h>
#include <chaos/TiledMap.h>
#include <chaos/RenderableLayerSystem.h>
#include <chaos/TimedAccumulator.h>

#include <death/GameHUD.h>
#include <death/GameLevel.h>
#include <death/TiledMapLevel.h>
#include <death/GameParticleCreator.h>

namespace death
{

#define DEATHGAME_JSON_ATTRIBUTE(x) chaos::JSONTools::GetAttribute(config, #x, x)

	class Game : public chaos::ReferencedObject
	{
		friend class GamepadManager;
		friend class GameWindow;

		friend class GameStateMachine;
		friend class GameState;
		friend class GameTransition;

		friend class MainMenuState;
		friend class PlayingState;
		friend class PauseState;

		friend class PlayingToPauseTransition;
		friend class PauseToPlayingTransition;
		friend class MainMenuToPlayingTransition;
		friend class PlayingToMainMenuTransition;
		friend class PlayingState;
		friend class PlayingToGameOverTransition;

		friend class PlayingHUD;

	public:

		/** initialization of the game */
		virtual bool InitializeGame(GLFWwindow * in_glfw_window);

		/** returns the game name */
		char const * GetGameName() const { return game_name; }
		/** returns the game instructions */
		char const * GetGameInstructions() const { return game_instructions; }

		/** returns the best score */
		int GetBestScore() const { return best_score; }
		/** returns the current score */
		int GetCurrentScore() const { return current_score; }

		/** returns the current life */
		void SetCurrentLife(int new_life);

		int GetCurrentLife() const { return current_life; }

		/** getter on the particle manager */
		chaos::ParticleManager * GetParticleManager(){ return particle_manager.get(); }
		chaos::ParticleManager const * GetParticleManager() const { return particle_manager.get(); }

		/** getter on the texture atlas */
		chaos::BitmapAtlas::TextureArrayAtlas * GetTextureAtlas(){ return texture_atlas.get(); }
		chaos::BitmapAtlas::TextureArrayAtlas const * GetTextureAtlas() const { return texture_atlas.get(); }

		/** getter on the text generator */
		chaos::ParticleTextGenerator::Generator * GetTextGenerator() { return particle_text_generator.get(); }
		chaos::ParticleTextGenerator::Generator const * GetTextGenerator() const { return particle_text_generator.get(); }

		// The clocks: 
		//   - root  clock : the top level clock. never reseted, never paused
		//   - main  clock : reseted whenever a new game starts/ends. never paused
		//   - game  clock : reseted whenever a new game starts/ends. paused in MainMenu and Pause
		//   - level clock : reseted whenever a new level starts/ends. paused in MainMenu and Pause
		//   - pause clock : reseted whenever we enter/leave pause. only running during pause

		/** returns the root time */
		double GetRootClockTime() const;
		/** returns the main time */
		double GetMainClockTime() const;
		/** returns the game time */
		double GetGameClockTime() const;
		/** returns the level time */
		double GetLevelClockTime() const;
		/** returns the pause time */
		double GetPauseClockTime() const;

		// Renderable layers
#define DEATH_FIND_RENDERABLE_CHILD(result, funcname)\
		result * funcname(char const * name, chaos::RenderableLayerSystem * root = nullptr);\
		result const * funcname(char const * name, chaos::RenderableLayerSystem const * root = nullptr) const;\
		result * funcname(chaos::TagType tag, chaos::RenderableLayerSystem * root = nullptr);\
		result const * funcname(chaos::TagType tag, chaos::RenderableLayerSystem const * root = nullptr) const;
		DEATH_FIND_RENDERABLE_CHILD(chaos::RenderableLayerSystem, FindRenderableLayer);
		DEATH_FIND_RENDERABLE_CHILD(chaos::ParticleLayer, FindParticleLayer);
#undef DEATH_FIND_RENDERABLE_CHILD

		/** utility function to get the application */
		chaos::MyGLFW::SingleWindowApplication * GetApplication();
		/** utility function to get the application */
		chaos::MyGLFW::SingleWindowApplication const * GetApplication() const;
		/** utility function to get the sound manager */
		chaos::SoundManager * GetSoundManager();

		/** returns application clock */
		chaos::Clock * GetApplicationClock();
		/** returns application clock */
		chaos::Clock const * GetApplicationClock() const;

		/** returns root clock */
		chaos::Clock * GetRootClock();
		/** returns root clock */
		chaos::Clock const * GetRootClock() const;

		/** returns main clock */
		chaos::Clock * GetMainClock();
		/** returns main clock */
		chaos::Clock const * GetMainClock() const;

		/** returns game clock */
		chaos::Clock * GetGameClock();
		/** returns game clock */
		chaos::Clock const * GetGameClock() const;

		/** returns level clock */
		chaos::Clock * GetLevelClock();
		/** returns level clock */
		chaos::Clock const * GetLevelClock() const;

		/** returns pause clock */
		chaos::Clock * GetPauseClock();
		/** returns pause clock */
		chaos::Clock const * GetPauseClock() const;

		/** returns the HUD */
		GameHUD * GetCurrentHUD(){ return hud.get();}
		/** returns the HUD */
		GameHUD const * GetCurrentHUD() const { return hud.get();}

		/** get particle tools */
		GameParticleCreator & GetGameParticleCreator(){ return particle_creator; }
		/** get particle tools */
		GameParticleCreator const & GetGameParticleCreator() const { return particle_creator; }

		/** get the size of the world */
		virtual glm::vec2 GetViewSize() const;
		/** get the view */
		chaos::box2 GetViewBox() const;

		/** getting the camera box */
		virtual chaos::box2 GetCameraBox() const;
		/** update the camera box */
		virtual void SetCameraBox(chaos::box2 const & in_camera_box);


		/** getting the initial camera box */
		virtual chaos::box2 GetInitialCameraBox() const;
		/** update the initial camera box */
		virtual void SetInitialCameraBox(chaos::box2 const & in_camera_box);

		/** getting the world boxes */
		virtual chaos::box2 GetWorldBox() const;

		/** play some sound */
		chaos::Sound * PlaySound(char const * name, bool paused, bool looping);

#if _DEBUG	
		/** declare we want to skip level */
		void SetCheatSkipLevelRequired(bool value);
		/** test whether we want to skip level */
		bool GetCheatSkipLevelRequired() const;

		/** declare we want to enter cheat mode */
		void SetCheatMode(bool value);
		/** test whether we can to loose life */
		bool GetCheatMode() const;

#endif

		// XXX : player allocation is not necessarly in one of the game particle_manager's layer
		//       it can be set from a level instance's particle_manager

		/** get the player allocation */
		chaos::ParticleAllocation * GetPlayerAllocation() { return player_allocations.get(); }
		chaos::ParticleAllocation const * GetPlayerAllocation() const { return player_allocations.get(); }
		/** set player allocation */
		virtual void SetPlayerAllocation(chaos::ParticleAllocation * in_allocation);

		/** get object particle */
		chaos::ParticleDefault::Particle * GetObjectParticle(chaos::ParticleAllocation * allocation, size_t index);
		chaos::ParticleDefault::Particle const * GetObjectParticle(chaos::ParticleAllocation const * allocation, size_t index) const;

		/** get object position */
		glm::vec2 GetObjectPosition(chaos::ParticleAllocation const * allocation, size_t index) const;
		/** get object box */
		chaos::box2 GetObjectBox(chaos::ParticleAllocation const * allocation, size_t index) const;
		/** set the object position */
		bool SetObjectPosition(chaos::ParticleAllocation * allocation, size_t index, glm::vec2 const & position);
		/** set the object box */
		bool SetObjectBox(chaos::ParticleAllocation * allocation, size_t index, chaos::box2 const & box);

		/** get player particle */
		chaos::ParticleDefault::Particle * GetPlayerParticle();
		chaos::ParticleDefault::Particle const * GetPlayerParticle() const;

		/** get player position */
		glm::vec2 GetPlayerPosition() const;
		/** get player box */
		chaos::box2 GetPlayerBox() const;
		/** set the player position */
		bool SetPlayerPosition(glm::vec2 const & position);
		/** set the player box */
		bool SetPlayerBox(chaos::box2 const & box);

		/** get currently played level */
		GameLevel * GetCurrentLevel();
		/** get currently played level */
		GameLevel const * GetCurrentLevel() const;

		/** get currently played level */
		GameLevelInstance * GetCurrentLevelInstance();
		/** get currently played level */
		GameLevelInstance const * GetCurrentLevelInstance() const;

		/** get currently played level */
		GameLevel * GetLevel(int level_index);
		/** get currently played level */
		GameLevel const * GetLevel(int level_index) const;

		/** get the current frame rate */
		float GetFrameRate() const { return fps_counter.GetCurrentValue(); }

		/** reloading the configuration file */
		bool ReloadConfigurationFile();

	protected:

		/** the tick method */
		virtual void Tick(double delta_time);
		/** tick the game inputs */
		virtual void TickGameInputs(double delta_time);

		/** whenever a key event is received */
		virtual bool OnKeyEvent(int key, int action);
		/** whenever a char event is received */
		virtual bool OnCharEvent(unsigned int c);
		/** whenever a mouse event is received */
		virtual void OnMouseButton(int button, int action, int modifier);
		/** whenever mouse is displaced */
		virtual void OnMouseMove(double x, double y);
		/** the rendering method */
		virtual void Display(chaos::Renderer * renderer, glm::ivec2 const & size);
		/** the user defined rendering function */
		virtual void DoDisplay(chaos::Renderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::RenderParams const & render_params);

		/** initialization from the config file */
		virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path);

		/** update the player and the camera position so that they remains inside the world */
		void RestrictCameraToPlayerAndWorld();

		/** blend out a music */
		void BlendMusic(chaos::Sound * music, bool blend_in);
		/** start music[0], stop all others */
		void ChangeMusic(chaos::Sound ** musics, size_t count, bool restart_first);

		/** change the game music */
		void StartMainMenuMusic(bool restart_first);
		void StartGameMusic(bool restart_first);
		void StartPauseMusic(bool restart_first);

		/** get current state ID */
		int GetCurrentStateTag() const;
		/** returns whether were are in playing state */
		bool IsPlaying() const;
		/** returns whether were are in pause state */
		bool IsPaused() const;

		/** test whether the current score is higher than best score and save it */
		void ConditionnalSaveBestScore();
		/** save the best score */
		bool SerializeBestScore(bool save);
		/** data internal method serialization */
		virtual bool LoadBestScore(std::ifstream & file);
		/** data internal method serialization */
		virtual bool SaveBestScore(std::ofstream & file);

		/** create all the music of the game */
		virtual bool CreateAllMusics();
		/** declare all particle types (for safety) */
		virtual bool DeclareParticleClasses();
		/** create the game state_machine */
		virtual bool CreateGameStateMachine();
		/** allocate the state machine */
		virtual chaos::SM::StateMachine * DoCreateGameStateMachine();
		/** create the game state_machine instance */
		virtual chaos::SM::StateMachineInstance * DoCreateGameStateMachineInstance(chaos::SM::StateMachine * state_machine);
		/** handle the gamepad input */
		virtual void HandleGamepadInput(chaos::MyGLFW::GamepadData & in_gamepad_data);
		/** handle keyboards input */
		virtual void HandleKeyboardInputs();
		/** internal method called to reset cached inputs */
		virtual void ResetPlayerCachedInputs();

		/** special action on gamepad input reception */
		virtual bool OnGamepadInput(chaos::MyGLFW::GamepadData & in_gamepad_data);

		/** called whenever a gamepad input is comming */
		virtual bool OnPhysicalGamepadInput(chaos::MyGLFW::PhysicalGamepad * physical_gamepad);

		/** called whenever the input mode changes */
		virtual void OnInputModeChanged(int new_mode, int old_mode);

		/** create some clocks */
		virtual bool InitializeClocks();
		/** create clocks used in game */
		virtual bool CreateInGameClocks();
		/** destroy all clocks for the game */
		virtual bool DestroyInGameClocks();

		/** initialize the game data from configuration file */
		virtual bool InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path, bool hot_reload);

		/** initialize a mapping with button names / text generator joker */
		virtual bool InitializeGamepadButtonInfo();
		/** initialize the particle text generator */
		virtual bool InitializeParticleTextGenerator();
		/** initialize the particle manager */
		virtual bool InitializeParticleManager();
		/** create the layers in the particle manager (returns the number of layer inserted => -1 for error) */
		virtual int AddParticleLayers();
		/** initialize the GameParticleCreator */
		virtual bool InitializeGameParticleCreator();
		/** create the background image */
		virtual bool CreateBackgroundImage(char const * material_name, char const * texture_name);

		/** initialize the render layer */
		virtual bool InitializeRootRenderLayer();

		/** insert a rendering layering */
		chaos::RenderableLayerSystem * AddChildRenderLayer(char const * layer_name, chaos::TagType layer_tag, int render_order);

		/** generate the atlas for the whole game */
		virtual bool GenerateAtlas(nlohmann::json const & config, boost::filesystem::path const & config_path);
		/** fill atlas generation input */
		virtual bool FillAtlasGenerationInput(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path);
		/** fill atlas generation input (sprite directory) */
		virtual bool FillAtlasGenerationInputSprites(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path);
		/** fill atlas generation input (fonts) */
		virtual bool FillAtlasGenerationInputFonts(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path);
		/** fill atlas generation input (one font from configuration file) */
		virtual bool FillAtlasGenerationInputOneFont(char const * font_config_name, char const * font_name, chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path);
		/** fill atlas generation input from the tiled map manager */
		virtual bool FillAtlasGenerationInputTiledMapManager(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path);

		/** load all the levels from the game (can be simple data) */
		virtual bool LoadLevels();
		/* load one level */
		virtual class death::GameLevel * DoLoadLevel(int level_index, chaos::FilePathParam const & path);
		/** create one tiled map level */
		virtual death::TiledMap::Level * CreateTiledMapLevel();

		/** the game main loop */
		virtual bool TickGameLoop(double delta_time);
		/** test whether there is a game over */
		virtual bool CheckGameOverCondition();
		/** check whether we must go to next level */
		virtual bool CheckLevelCompleted();
		/** check whether we can go to next level */
		virtual bool CanCompleteLevel();

		virtual void ResetGameVariables();

		/** called on the very first time the game is started */
		virtual void OnEnterMainMenu(bool very_first);
		/** called whenever the game is lost */
		virtual void OnGameOver();
		/** called whenever we enter in pause mode */
		virtual bool OnEnterPause();
		/** called whenever we leave pause mode */
		virtual bool OnLeavePause();

		/** called whenever we enter in game mode */
		virtual bool OnEnterGame();
		/** called whenever we leave game mode */
		virtual bool OnLeaveGame(bool gameover);
		/** called whenever we are returning from game to main menu without gameover */
		virtual bool OnAbordGame();

		/** returns true if the pause if fully set */
		virtual bool IsPauseEnterComplete();
		/** returns true if the game if fully restored from pause */
		virtual bool IsPauseLeaveComplete();

		/** returns true if the game enter if fully set */
		virtual bool IsGameEnterComplete();
		/** returns true if the game leave is fully completed */
		virtual bool IsGameLeaveComplete();

		/** require a pause or resume */
		virtual bool RequireTogglePause();
		/** require a game Start */
		virtual bool RequireStartGame();
		/** require a game exit */
		virtual bool RequireExitGame();
		/** require a game over */
		virtual bool RequireGameOver();

		/** pause/resume pause/game clocks */
		void OnPauseStateUpdateClocks(bool enter_pause);

		/** create the pause HUD */
		bool CreatePauseMenuHUD();
		/** create the main menu HUD */
		bool CreateMainMenuHUD();
		/** create the game HUD */
		bool CreatePlayingHUD();
		/** create the gameover HUD */
		bool CreateGameOverHUD();

		/** destroy the HUD */
		void DestroyHUD();

		/** user defined method to create the pause HUD */
		virtual GameHUD * DoCreatePauseMenuHUD();
		/** user defined method to create the main menu HUD */
		virtual GameHUD * DoCreateMainMenuHUD();
		/** user defined method to create the game HUD */
		virtual GameHUD * DoCreatePlayingHUD();
		/** user defined method to create the gameover HUD */
		virtual GameHUD * DoCreateGameOverHUD();

		/** change the current level */
		bool SetCurrentLevel(GameLevel * new_level);
		/** change the current level from its index */
		bool SetCurrentLevel(int level_index);
		/** change the current level for the next */
		bool SetNextLevel(bool looping_levels);
		/** reflex method whenever the level is changed */
		virtual void OnLevelChanged(GameLevel * new_level, GameLevel * old_level, GameLevelInstance * new_level_instance, GameLevelInstance * old_level_instance);

	protected:

		/** the window in GLFW library */
		GLFWwindow * glfw_window = nullptr;

		/** the camera (expressed in world system ?) */
		mutable chaos::box2 camera_box;
		/** the camera and last started level (expressed in world system ?)*/
		chaos::box2 initial_camera_box;
		/** the safe zone of the camera */
		glm::vec2 camera_safe_zone = glm::vec2(0.8f, 0.8f);


		/** the current gamepad manager */
		chaos::shared_ptr<chaos::MyGLFW::GamepadManager> gamepad_manager;
		/** the gamepad data */
		chaos::MyGLFW::GamepadData gamepad_data;
		/** the current stick position */
		glm::vec2 left_stick_position = glm::vec2(0.0f, 0.0f);
		glm::vec2 right_stick_position = glm::vec2(0.0f, 0.0f);
		/** the stick position, last time it was non null */
		glm::vec2 last_left_stick_position = glm::vec2(0.0f, 0.0f);
		glm::vec2 last_right_stick_position = glm::vec2(0.0f, 0.0f);

		/** the texture atlas */
		chaos::shared_ptr<chaos::BitmapAtlas::TextureArrayAtlas> texture_atlas;
		/** the particle manager */
		chaos::shared_ptr<chaos::ParticleManager> particle_manager;

		/** the rendering layer system */
		chaos::shared_ptr<chaos::RenderableLayerSystem> root_render_layer;


		/** the text generator */
		chaos::shared_ptr<chaos::ParticleTextGenerator::Generator> particle_text_generator;

		/** the sounds being played */
		chaos::shared_ptr<chaos::Sound> menu_music;
		chaos::shared_ptr<chaos::Sound> game_music;
		chaos::shared_ptr<chaos::Sound> pause_music;

		/** the HUDs */
		chaos::shared_ptr<GameHUD> hud;

		/** pointer on the state_machine */
		chaos::shared_ptr<chaos::SM::StateMachine> game_state_machine;
		/** pointer on the state_machine instance */
		chaos::shared_ptr<chaos::SM::StateMachineInstance> game_state_machine_instance;

		/** a mapping between the button index and its resource name + text generator alias */
		std::map<int, std::pair<std::string, std::string>> gamepad_button_map;

		/** for counting frame per seconds */
		chaos::TimedAccumulator<float> fps_counter;

		/** score values */
		int best_score = 0;
		int current_score = 0;

		/** life counters */
		int current_life = 3;
		int initial_life = 3;
		int max_life = 3;

		/** cheating */
#if _DEBUG
		mutable bool cheat_skip_level_required = false;
		bool cheat_mode = false;
#endif

		/** game settings */
		float mouse_sensitivity = 1.0f;
		float gamepad_sensitivity = 1.0f;

		/** the wanted viewport aspect */
		float viewport_wanted_aspect = (16.0f / 9.0f);

		/** name of the game */
		char const * game_name = nullptr;
		/** instructions displayed on may screen */
		char const * game_instructions = nullptr;

		/** the clocks */
		chaos::shared_ptr<chaos::Clock> root_clock;
		chaos::shared_ptr<chaos::Clock> main_clock;
		chaos::shared_ptr<chaos::Clock> game_clock;
		chaos::shared_ptr<chaos::Clock> level_clock;
		chaos::shared_ptr<chaos::Clock> pause_clock;

		/** the particle tools */
		GameParticleCreator particle_creator;

		/** a tiled map manager */
		chaos::shared_ptr<chaos::TiledMap::Manager> tiled_map_manager;

		/** level data */
		std::vector<chaos::shared_ptr<class GameLevel>> levels;
		/** the current level instance */
		chaos::shared_ptr<GameLevelInstance> current_level_instance;

		/** some allocations */
		chaos::shared_ptr<chaos::ParticleAllocation> player_allocations;
		chaos::shared_ptr<chaos::ParticleAllocation> background_allocations;

		/** a time stamp for rendering */
		uint64_t rendering_timestamp = 0;
	};

}; // namespace death
