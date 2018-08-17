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

#include <death/GameHUD.h>
#include <death/GameLevel.h>

namespace death
{
#define DEATHGAME_JSON_ATTRIBUTE(x) chaos::JSONTools::GetAttribute(config, #x, x)

	class Game : public chaos::ReferencedObject
	{
		friend class GamepadManager;
		friend class GameWindow;

		friend class GameAutomata;
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

		/** some ID for layers */
		static int const TEXT_LAYER_ID = 0;
		static int const LAST_LAYER_ID = 0;

		/** initialization of the game */
		virtual bool InitializeGame(GLFWwindow * in_glfw_window);

		/** getter on the particle manager */
		chaos::ParticleManager * GetParticleManager(){ return particle_manager.get(); }
		chaos::ParticleManager const * GetParticleManager() const { return particle_manager.get(); }


		/** getter on the texture atlas */
		chaos::BitmapAtlas::TextureArrayAtlas * GetTextureAtlas(){ return texture_atlas.get(); }
		chaos::BitmapAtlas::TextureArrayAtlas const * GetTextureAtlas() const { return texture_atlas.get(); }

		/** returns the main time */
		double GetMainClockTime() const;
		/** returns the game time */
		double GetGameClockTime() const;
		/** returns the pause time */
		double GetPauseClockTime() const;

	protected:

		/** the tick method */
		virtual void Tick(double delta_time);
		/** whenever a key event is received */
		virtual bool OnKeyEvent(int key, int action);
		/** whenever a char event is received */
		virtual bool OnCharEvent(unsigned int c);
		/** whenever a mouse event is received */
		virtual void OnMouseButton(int button, int action, int modifier);
		/** whenever mouse is displaced */
		virtual void OnMouseMove(double x, double y);
		/** the rendering method */
		virtual void Display(glm::ivec2 const & size);
		/** the user defined rendering function */
		virtual void DoDisplay(chaos::box2 const & viewport, chaos::GPUProgramProvider & uniform_provider);

		/** utility method to set a box uniform */
		void AddBoxVariable(chaos::GPUProgramProvider & uniform_provider, char const * variable_name, chaos::box2 const & b);


		/** initialization from the config file */
		virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path);

		/** utility function to get the application */
		chaos::MyGLFW::SingleWindowApplication * GetApplication();
		/** utility function to get the application */
		chaos::MyGLFW::SingleWindowApplication const * GetApplication() const;
		/** utility function to get the sound manager */
		chaos::SoundManager * GetSoundManager();

		/** returns main clock */
		chaos::Clock * GetMainClock();
		/** returns main clock */
		chaos::Clock const * GetMainClock() const;

		/** returns game clock */
		chaos::Clock * GetGameClock();
		/** returns game clock */
		chaos::Clock const * GetGameClock() const;

		/** returns pause clock */
		chaos::Clock * GetPauseClock();
		/** returns pause clock */
		chaos::Clock const * GetPauseClock() const;

		/** get the size of the world */
		virtual glm::vec2 GetViewSize() const;
		/** get the view */
		chaos::box2 GetViewBox() const;

		/** getting the world boxes */
		virtual chaos::box2 GetWorldBox() const;
		/** getting the camera box */
		virtual chaos::box2 GetCameraBox() const;
		/** getting the player box */
		virtual chaos::box2 GetPlayerBox() const;

		/** update the camera box */
		virtual void SetCameraBox(chaos::box2 const & in_camera_box);
		/** update the player box */
		virtual bool SetPlayerBox(chaos::box2 const & in_player_box);







		/** update the player and the camera position so that they remains inside the world */
		void RestrictCameraToPlayerAndWorld();

		/** play some sound */
		chaos::Sound * PlaySound(char const * name, bool paused, bool looping);
		/** blend out a music */
		void BlendMusic(chaos::Sound * music, bool blend_in);
		/** start music[0], stop all others */
		void ChangeMusic(chaos::Sound ** musics, size_t count, bool restart_first);

		/** change the game music */
		void StartMainMenuMusic(bool restart_first);
		void StartGameMusic(bool restart_first);
		void StartPauseMusic(bool restart_first);

		/** get current state ID */
		int GetCurrentStateID() const;
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
		/** create the game automata */
		virtual bool CreateGameAutomata();
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
		/** initialize the game data from configuration file */
		virtual bool InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path);

		/** initialize a mapping with button names / text generator joker */
		virtual bool InitializeGamepadButtonInfo();
		/** initialize the particle text generator */
		virtual bool InitializeParticleTextGenerator();
		/** initialize the particle manager */
		virtual bool InitializeParticleManager();

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

		/** load all the levels from the game (can be simple data) */
		virtual bool LoadLevels();
		/* load one level */
		virtual class death::GameLevel * DoLoadLevel(int level_index, chaos::FilePathParam const & path);

		/** the game main loop */
		virtual bool TickGameLoop(double delta_time);
		/** test whether there is a game over */
		virtual bool CheckGameOverCondition(double delta_time);
		/** update the score particles if necessary */
		void UpdateScoreParticles();

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

		/** create a text particle system */
		chaos::ParticleAllocation * CreateTextParticles(char const * text, chaos::ParticleTextGenerator::GeneratorParams const & params, int layer_id = TEXT_LAYER_ID);
		/** create a title */
		chaos::ParticleAllocation * CreateTitle(char const * title, bool normal, int layer_id = TEXT_LAYER_ID);
		/** create a score text at the top left corner */
		chaos::ParticleAllocation * Game::CreateScoringText(char const * format, int value, float Y, int layer_id = TEXT_LAYER_ID);

		/** create the pause HUD */
		void CreatePauseMenuHUD();
		/** create the main menu HUD */
		void CreateMainMenuHUD();
		/** create the game HUD */
		void CreatePlayingHUD();
		/** create the gameover HUD */
		void CreateGameOverHUD();

		/** destroy the pause HUD */
		void DestroyPauseMenuHUD();
		/** destroy the main menu HUD */
		void DestroyMainMenuHUD();
		/** destroy the game HUD */
		void DestroyPlayingHUD();
		/** destroy the gameover HUD */
		void DestroyGameOverHUD();

		/** user defined method to create the pause HUD */
		virtual PauseMenuHUD * DoCreatePauseMenuHUD();
		/** user defined method to create the main menu HUD */
		virtual MainMenuHUD * DoCreateMainMenuHUD();
		/** user defined method to create the game HUD */
		virtual PlayingHUD * DoCreatePlayingHUD();
		/** user defined method to create the gameover HUD */
		virtual GameOverHUD * DoCreateGameOverHUD();

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

		/** the camera */
		mutable chaos::box2 camera_box;
		/** the safe zone of the camera */
		glm::vec2 camera_safe_zone = glm::vec2(0.8f, 0.8f);


		/** the current gamepad manager */
		boost::intrusive_ptr<chaos::MyGLFW::GamepadManager> gamepad_manager;
		/** the gamepad data */
		chaos::MyGLFW::GamepadData gamepad_data;
		/** the current stick position */
		glm::vec2 left_stick_position = glm::vec2(0.0f, 0.0f);
		glm::vec2 right_stick_position = glm::vec2(0.0f, 0.0f);

		/** the texture atlas */
		boost::intrusive_ptr<chaos::BitmapAtlas::TextureArrayAtlas> texture_atlas;
		/** the particle manager */
		boost::intrusive_ptr<chaos::ParticleManager> particle_manager;
		/** the text generator */
		boost::intrusive_ptr<chaos::ParticleTextGenerator::Generator> particle_text_generator;

		/** the sounds being played */
		boost::intrusive_ptr<chaos::Sound> menu_music;
		boost::intrusive_ptr<chaos::Sound> game_music;
		boost::intrusive_ptr<chaos::Sound> pause_music;

		/** the HUDs */
		boost::intrusive_ptr<MainMenuHUD> main_menu_hud;
		boost::intrusive_ptr<PauseMenuHUD> pause_menu_hud;
		boost::intrusive_ptr<PlayingHUD> playing_hud;
		boost::intrusive_ptr<GameOverHUD> gameover_hud;

		/** pointer on the automata */
		boost::intrusive_ptr<class GameAutomata> game_automata;

		/** a mapping between the button index and its resource name + text generator alias */
		std::map<int, std::pair<std::string, std::string>> gamepad_button_map;


		/** score values */
		int best_score = 0;
		int current_score = 0;

		/** game settings */
		float mouse_sensitivity = 1.0f;
		float gamepad_sensitivity = 1.0f;

		/** the wanted viewport aspect */
		float viewport_wanted_aspect = (16.0f / 9.0f);
		/** some HUD settings */
		float title_size = 150.0f;
		float title_placement_y = 0;

		/** name of the game */
		char const * game_name = nullptr;

		/** the clocks */
		boost::intrusive_ptr<chaos::Clock> pause_clock;
		boost::intrusive_ptr<chaos::Clock> game_clock;

		/** level data */
		std::vector<boost::intrusive_ptr<class GameLevel>> levels;
		/** the current level instance */
		boost::intrusive_ptr<GameLevelInstance> current_level_instance;
	};

}; // namespace death
