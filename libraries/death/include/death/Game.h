#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/InputEventReceiver.h>
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
#include <chaos/GPURenderableLayerSystem.h>
#include <chaos/TimedAccumulator.h>
#include <chaos/FileResource.h>

#include <death/GameFramework.h>
#include <death/GameHUD.h>
#include <death/GameLevel.h>
#include <death/TiledMapLevel.h>
#include <death/GameParticleCreator.h>
#include <death/GameInstanceEntity.h>
#include <death/Player.h>
#include <death/GameInstance.h>

namespace death
{

#define DEATHGAME_JSON_ATTRIBUTE(x) chaos::JSONTools::GetAttribute(config, #x, x)

	using PhysicalGamepadWrapper = chaos::ReferencedObjectDataWrapper<chaos::MyGLFW::PhysicalGamepad*>;

	class Game : public chaos::ReferencedObject, public chaos::InputEventReceiver, public chaos::ResourceFriend
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

		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

	public:

		/** get the player by its index */
		Player * GetPlayer(size_t player_index);
		/** get the player by its index */
		Player const * GetPlayer(size_t player_index) const;
		/** get the number of players */
		size_t GetPlayerCount() const;

		/** get currently played level */
		GameLevel * GetLevel();
		/** get currently played level */
		GameLevel const * GetLevel() const;

		/** get currently played level */
		GameLevelInstance * GetLevelInstance();
		/** get currently played level */
		GameLevelInstance const * GetLevelInstance() const;

		/** get the game instance */
		GameInstance * GetGameInstance() { return game_instance.get(); }
		/** get the game instance */
		GameInstance const * GetGameInstance() const { return game_instance.get(); }

		/** initialization of the game */
		virtual bool InitializeGame(GLFWwindow * in_glfw_window);

		/** returns the game name */
		char const * GetGameName() const { return game_name; }
		/** returns the game instructions */
		char const * GetGameInstructions() const { return game_instructions; }

		/** returns the best score */
		int GetBestScore() const { return best_score; }
		/** returns the best score among existing players */
		int GetBestPlayerScore() const;

		/** getter on the tiled map manager */
		chaos::TiledMap::Manager * GetTiledMapManager() { return tiled_map_manager.get(); }
		chaos::TiledMap::Manager const * GetTiledMapManager() const { return tiled_map_manager.get(); }

		/** getter on the particle manager */
		chaos::ParticleManager * GetParticleManager() { return particle_manager.get(); }
		chaos::ParticleManager const * GetParticleManager() const { return particle_manager.get(); }

		/** getter on the texture atlas */
		chaos::BitmapAtlas::TextureArrayAtlas * GetTextureAtlas() { return texture_atlas.get(); }
		chaos::BitmapAtlas::TextureArrayAtlas const * GetTextureAtlas() const { return texture_atlas.get(); }

		/** getter on the text generator */
		chaos::ParticleTextGenerator::Generator * GetTextGenerator() { return particle_text_generator.get(); }
		chaos::ParticleTextGenerator::Generator const * GetTextGenerator() const { return particle_text_generator.get(); }

		// The clocks: 
		//   - root  clock : the top level clock. never reseted, never paused

		/** returns the root time */
		double GetRootClockTime() const;
		/** returns root clock */
		chaos::Clock * GetRootClock() { return root_clock.get(); }
		/** returns root clock */
		chaos::Clock const * GetRootClock() const { return root_clock.get(); }

		// GPURenderable layers
#define DEATH_FIND_RENDERABLE_CHILD(result, funcname)\
		result * funcname(char const * name, chaos::GPURenderableLayerSystem * root = nullptr);\
		result const * funcname(char const * name, chaos::GPURenderableLayerSystem const * root = nullptr) const;\
		result * funcname(chaos::TagType tag, chaos::GPURenderableLayerSystem * root = nullptr);\
		result const * funcname(chaos::TagType tag, chaos::GPURenderableLayerSystem const * root = nullptr) const;
		DEATH_FIND_RENDERABLE_CHILD(chaos::GPURenderableLayerSystem, FindRenderableLayer);
		DEATH_FIND_RENDERABLE_CHILD(chaos::ParticleLayerBase, FindParticleLayer);
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

		/** returns the HUD */
		GameHUD * GetCurrentHUD() { return hud.get(); }
		/** returns the HUD */
		GameHUD const * GetCurrentHUD() const { return hud.get(); }

		/** get particle tools */
		GameParticleCreator & GetGameParticleCreator() { return particle_creator; }
		/** get particle tools */
		GameParticleCreator const & GetGameParticleCreator() const { return particle_creator; }

		/** get the size of the canvas / world */
		virtual glm::vec2 GetCanvasSize() const;
		/** get the view */
		chaos::box2 GetCanvasBox() const;

		/** getting the world boxes */
		virtual chaos::box2 GetWorldBox() const;

		/** play some sound */
		chaos::Sound * PlaySound(char const * name, bool paused, bool looping, float blend_time, chaos::TagType category_tag);
		/** play some sound */
		chaos::Sound * PlaySound(char const * name, chaos::PlaySoundDesc play_desc, chaos::TagType category_tag);
		/** change the in game music (fadeout previous music) */
		chaos::Sound * SetInGameMusic(char const * music_name);

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

		/** get any level */
		GameLevel * GetLevel(int level_index);
		/** get any level */
		GameLevel const * GetLevel(int level_index) const;

        /** get glfw window */
        virtual GLFWwindow* GetGLFWWindow() const override { return glfw_window; }

		/** reloading the configuration file */
		bool ReloadGameConfiguration();
		/** reloading current level and restart it */
		bool ReloadCurrentLevel();

		/** getter on mouse sensitivity */
		float GetMouseSensitivity() const { return mouse_sensitivity;}
		/** getter on gamepad sensitivity */
		float GetGamepadSensitivity() const { return gamepad_sensitivity; }

		/** get current state ID */
		chaos::TagType GetCurrentStateTag(bool strict_state = false, bool use_destination = false) const;
		/** returns whether were are in playing state */
		bool IsPlaying(bool strict_state = false, bool use_destination = false) const;
		/** returns whether were are in pause state */
		bool IsPaused(bool strict_state = false, bool use_destination = false) const;

		/** create the background image */
		virtual bool CreateBackgroundImage(char const * material_name, char const * texture_name);

		/** returns whether we are in free camera mode */
		bool IsFreeCameraMode() const;
		/** change free camera mode */
		void SetFreeCameraMode(bool free_camera_mode);

		/** get the free camera */
		Camera * GetFreeCamera();
		/** get the free camera */
		Camera const * GetFreeCamera() const;

		/** save the game into a checkpoint */
		GameCheckpoint * SaveIntoCheckpoint();

	protected:

		/** the tick method */
		virtual void Tick(double delta_time);
		/** tick the game inputs */
		virtual void TickGameInputs(double delta_time);

		/** whenever a key event is received */
		virtual bool OnKeyEventImpl(chaos::KeyEvent const & event) override;
		/** whenever a char event is received */
		virtual bool OnCharEventImpl(unsigned int c) override;
		/** whenever a mouse event is received */
		virtual bool OnMouseButtonImpl(int button, int action, int modifier) override;
		/** whenever mouse is displaced */
		virtual bool OnMouseMoveImpl(double x, double y) override;

		/** getting the required viewport for given window */
		virtual chaos::box2 GetRequiredViewport(glm::ivec2 const & size) const;

		/** the rendering method */
		virtual void Display(chaos::GPURenderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::GPURenderParams const & render_params);
		/** the user defined rendering function */
		virtual void DoDisplay(chaos::GPURenderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::GPURenderParams const & render_params);
		/** internal method to prepare rendering */
		virtual void DoPreDisplay(chaos::GPURenderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::GPURenderParams const & render_params);		
		/** internal  method to display the game content */
		virtual void DoDisplayGame(chaos::GPURenderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::GPURenderParams const & render_params);
		/** internal  method to display the HUD */
		virtual void DoDisplayHUD(chaos::GPURenderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::GPURenderParams const & render_params);

		/** fill the rendering params before rendering */
		virtual void FillUniformProvider(chaos::GPUProgramProvider & main_uniform_provider);

		/** initialization from the config file */
		virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path);
		/** called whenever the game values as been changed */
		virtual void OnGameValuesChanged(bool hot_reload);

		/** test whether the current score is higher than best score and save it */
		void ConditionnalSaveBestScore();
		/** save the best score */
		bool SerializeBestScore(bool save);
		/** data internal method serialization */
		virtual bool LoadBestScore(std::ifstream & file);
		/** data internal method serialization */
		virtual bool SaveBestScore(std::ofstream & file);

		/** initialization of the manager */
		virtual bool InitializeSoundManager();
		/** declare all particle types (for safety) */
		virtual bool DeclareParticleClasses();
		/** create the game state_machine */
		virtual bool CreateGameStateMachine();
		/** allocate the state machine */
		virtual chaos::SM::StateMachine * DoCreateGameStateMachine();
		/** create the game state_machine instance */
		virtual chaos::SM::StateMachineInstance * DoCreateGameStateMachineInstance(chaos::SM::StateMachine * state_machine);

		/** special action on gamepad input reception */
		virtual bool OnGamepadInput(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad);
		/** called whenever a gamepad input is comming */
		virtual bool OnPhysicalGamepadInput(chaos::MyGLFW::PhysicalGamepad * physical_gamepad);

		/** called whenever the input mode changes */
		virtual void OnInputModeChanged(chaos::InputMode new_mode, chaos::InputMode old_mode) override;

		/** create some clocks */
		virtual bool InitializeClocks();
		/** initialize the game data from configuration file */
		virtual bool InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path, bool hot_reload);
		/** initialize a mapping with button names / text generator joker */
		virtual bool InitializeGamepadButtonInfo();
		/** initialize the particle text generator */
		virtual bool InitializeParticleTextGenerator(nlohmann::json const & config, boost::filesystem::path const & config_path);
		/** initialize the particle manager */
		virtual bool InitializeParticleManager();
		/** initialize the GameParticleCreator */
		virtual bool InitializeGameParticleCreator();
		/** initialize the render layer */
		virtual bool InitializeRootRenderLayer();

		/** create the layers in the particle manager (returns the number of layer inserted => -1 for error) */
		virtual int AddParticleLayers();
		/** insert a rendering layering */
		chaos::GPURenderableLayerSystem * AddChildRenderLayer(char const * layer_name, chaos::TagType layer_tag, int render_order);

		/** generate the atlas for the whole game */
		virtual bool GenerateAtlas(nlohmann::json const & config, boost::filesystem::path const & config_path);
		/** fill atlas generation input */
		virtual bool FillAtlasGenerationInput(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path);
		/** fill atlas generation input (sprite directory) */
		virtual bool FillAtlasGenerationInputSprites(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path);
		/** fill atlas generation input (fonts) */
		virtual bool FillAtlasGenerationInputFonts(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path);
		/** fill atlas generation input from the tiled map manager */
		virtual bool FillAtlasGenerationInputTiledMapManager(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path);

		/** load object type sets concerned by the game (if required) */
		virtual bool GenerateObjectTypeSets(nlohmann::json const & config);
		/** load tileset concerned by the game (if required) */
		virtual bool GenerateTileSets(nlohmann::json const & config);

		/** a generic function to load some tiled map instances */
		template<typename FUNC>
		bool DoGenerateTiledMapEntity(nlohmann::json const & config, char const * property_name, char const * default_value, char const * extension, FUNC func);

		/** read in config file an entry and open in resource directory a directory iterator on that target */
		boost::filesystem::directory_iterator GetResourceDirectoryIteratorFromConfig(nlohmann::json const & config, char const * config_name, char const * default_path) const;

		/** load all the levels from the game (can be simple data) */
		virtual bool LoadLevels(nlohmann::json const & config);
		/* load one level */
		virtual GameLevel * DoLoadLevel(chaos::FilePathParam const & path);
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

		/** returns whether levels are to be looped */
		bool AreLevelsLooping() const { return looping_levels; }

		/** called on the very first time the game is started */
		virtual void OnEnterMainMenu(bool very_first);
		/** called whenever we leave the main menu */
		virtual void OnLeaveMainMenu();

		/** called whenever the game is lost */
		virtual void OnGameOver();
		/** called whenever we enter in pause mode */
		virtual bool OnEnterPause();
		/** called whenever we leave pause mode */
		virtual bool OnLeavePause();

		/** called whenever we enter in game mode */
		virtual bool OnEnterGame(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad);
		/** called whenever we leave game mode */
		virtual bool OnLeaveGame();

		/** require a pause */
		virtual bool RequirePauseGame();
		/** require a pause or resume */
		virtual bool RequireTogglePause();
		/** require a game Start */
		virtual bool RequireStartGame(chaos::MyGLFW::PhysicalGamepad * physical_gamepad);
		/** require a game exit */
		virtual bool RequireExitGame();
		/** require a game over */
		virtual bool RequireGameOver();

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
		virtual void OnLevelChanged(GameLevel * new_level, GameLevel * old_level, GameLevelInstance * new_level_instance);

		/** generate the game instance */
		virtual GameInstance * CreateGameInstance();

		/** all sounds flagged in game are set to pause */
		void SetInGameSoundPause(bool in_paused);

		/** create a free camera for the current level instance */
		Camera * CreateFreeCamera() const;
		/** create a free camera for the current level instance */
		Camera * DoCreateFreeCamera(Camera const * camera_to_copy, GameLevelInstance * level_instance) const;

	protected:

		/** the window in GLFW library */
		GLFWwindow * glfw_window = nullptr;

		/** the current gamepad manager */
		chaos::shared_ptr<chaos::MyGLFW::GamepadManager> gamepad_manager;

		/** the texture atlas */
		chaos::shared_ptr<chaos::BitmapAtlas::TextureArrayAtlas> texture_atlas;
		/** the particle manager */
		chaos::shared_ptr<chaos::ParticleManager> particle_manager;

		/** the rendering layer system */
		chaos::shared_ptr<chaos::GPURenderableLayerSystem> root_render_layer;

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

		/** score values */
		int best_score = 0;

		/** the configuration object to use for game instance */
		nlohmann::json game_instance_configuration;
		/** the configuration path */
		boost::filesystem::path configuration_path;

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
		
		/** whether levels are looping */
		bool looping_levels = true;

		/** the clocks */
		chaos::shared_ptr<chaos::Clock> root_clock;

		/** the particle tools */
		GameParticleCreator particle_creator;

		/** a tiled map manager */
		chaos::shared_ptr<chaos::TiledMap::Manager> tiled_map_manager;

		/** level data */
		std::vector<chaos::shared_ptr<GameLevel>> levels;
		/** the current level instance */
		chaos::shared_ptr<GameLevelInstance> current_level_instance;

		/** some allocations */
		chaos::shared_ptr<chaos::ParticleAllocationBase> background_allocations;

		/** the game instance */
		chaos::shared_ptr<GameInstance> game_instance;

		/** the free camera */
		mutable chaos::shared_ptr<Camera> free_camera;
		/** free camera mode */
		bool free_camera_mode = false;
	};

}; // namespace death
