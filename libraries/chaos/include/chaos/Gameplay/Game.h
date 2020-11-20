#ifdef CHAOS_FORWARD_DECLARATION



namespace chaos
{
	using PhysicalGamepadWrapper = ReferencedObjectDataWrapper<MyGLFW::PhysicalGamepad*>;

	class Game;

}; // namespace chaos

#else

namespace chaos
{

	

	class Game : public Object, public InputEventReceiver
	{
		friend class GameGamepadManager;
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

		CHAOS_GAMEPLAY_ALLFRIENDS;

	public:

		CHAOS_DECLARE_OBJECT_CLASS2(Game, Object);

		CHAOS_DECLARE_GAMEPLAY_GETTERS();

		/** constructor */
		Game();

		/** Get a level by its index */
		AutoCastable<Level> FindLevelByIndex(int level_index);
		/** Get a level by its index */
		AutoConstCastable<Level> FindLevelByIndex(int level_index) const;
		/** Get a level by request */
		AutoCastable<Level> Game::FindLevel(ObjectRequest request);
		/** Get a level by request */
		AutoConstCastable<Level> Game::FindLevel(ObjectRequest request) const;

		/** returns the game name */
		char const * GetGameName() const { return game_name; }
		/** returns the game instructions */
		char const * GetGameInstructions() const { return game_instructions; }

		/** returns the best score */
		int GetBestScore() const { return best_score; }
		/** returns the best score among existing players */
		int GetBestPlayerScore() const;

		/** getter on the tiled map manager */
		TiledMap::Manager * GetTiledMapManager() { return tiled_map_manager.get(); }
		TiledMap::Manager const * GetTiledMapManager() const { return tiled_map_manager.get(); }

		/** getter on the particle manager */
		ParticleManager * GetParticleManager() { return particle_manager.get(); }
		ParticleManager const * GetParticleManager() const { return particle_manager.get(); }

		/** getter on the texture atlas */
		BitmapAtlas::TextureArrayAtlas * GetTextureAtlas() { return texture_atlas.get(); }
		BitmapAtlas::TextureArrayAtlas const * GetTextureAtlas() const { return texture_atlas.get(); }

		/** getter on the text generator */
		ParticleTextGenerator::Generator * GetTextGenerator() { return particle_text_generator.get(); }
		ParticleTextGenerator::Generator const * GetTextGenerator() const { return particle_text_generator.get(); }

		// The clocks: 
		//   - root  clock : the top level clock. never reseted, never paused

		/** returns the root time */
		double GetRootClockTime() const;
		/** returns root clock */
		Clock * GetRootClock() { return root_clock.get(); }
		/** returns root clock */
		Clock const * GetRootClock() const { return root_clock.get(); }

		// GPURenderable layers
#define CHAOS_FIND_RENDERABLE_CHILD(result, funcname)\
		result * funcname(char const * name, GPURenderableLayerSystem * root = nullptr);\
		result const * funcname(char const * name, GPURenderableLayerSystem const * root = nullptr) const;\
		result * funcname(TagType tag, GPURenderableLayerSystem * root = nullptr);\
		result const * funcname(TagType tag, GPURenderableLayerSystem const * root = nullptr) const;
		CHAOS_FIND_RENDERABLE_CHILD(GPURenderableLayerSystem, FindRenderableLayer);
		CHAOS_FIND_RENDERABLE_CHILD(ParticleLayerBase, FindParticleLayer);
#undef CHAOS_FIND_RENDERABLE_CHILD

		/** utility function to get the sound manager */
		SoundManager * GetSoundManager();

		/** returns application clock */
		Clock * GetApplicationClock();
		/** returns application clock */
		Clock const * GetApplicationClock() const;

		/** returns the HUD */
		GameHUD * GetCurrentHUD() { return hud.get(); }
		/** returns the HUD */
		GameHUD const * GetCurrentHUD() const { return hud.get(); }

		/** get particle tools */
		GameParticleCreator & GetGameParticleCreator() { return particle_creator; }
		/** get particle tools */
		GameParticleCreator const & GetGameParticleCreator() const { return particle_creator; }

		/** get the view */
		box2 GetCanvasBox() const;

		/** getting the world boxes */
		virtual box2 GetWorldBox() const;

		/** play some sound */
		Sound * PlaySound(char const * name, bool paused, bool looping, float blend_time, TagType category_tag);
		/** play some sound */
		Sound * PlaySound(char const * name, PlaySoundDesc play_desc, TagType category_tag);
		/** change the in game music (fadeout previous music) */
		Sound * SetInGameMusic(char const * music_name);

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

		/** reloading the configuration file */
		bool ReloadGameConfiguration();
		/** reloading current level and restart it */
		bool ReloadCurrentLevel();

		/** getter on mouse sensitivity */
		float GetMouseSensitivity() const { return mouse_sensitivity;}
		/** getter on gamepad sensitivity */
		float GetGamepadSensitivity() const { return gamepad_sensitivity; }
		/** get the wanted viewport aspect */
		float GetViewportWantedAspect() const { return viewport_wanted_aspect; }

		/** get current state ID */
		TagType GetCurrentStateTag(bool strict_state = false, bool use_destination = false) const;
		/** returns whether were are in playing state */
		bool IsPlaying(bool strict_state = false, bool use_destination = false) const;
		/** returns whether were are in pause state */
		bool IsPaused(bool strict_state = false, bool use_destination = false) const;

		/** create the background image */
		virtual bool CreateBackgroundImage(ObjectRequest material_request, ObjectRequest texture_request);

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
		virtual void Tick(float delta_time);
		/** tick the game inputs */
		virtual void TickGameInputs(float delta_time);

		/** whenever a key event is received */
		virtual bool OnKeyEventImpl(KeyEvent const & event) override;
		/** whenever a char event is received */
		virtual bool OnCharEventImpl(unsigned int c) override;
		/** whenever a mouse event is received */
		virtual bool OnMouseButtonImpl(int button, int action, int modifier) override;
		/** whenever mouse is displaced */
		virtual bool OnMouseMoveImpl(double x, double y) override;

		/** getting the required viewport for given window */
		virtual box2 GetRequiredViewport(glm::ivec2 const & size) const;

		/** the rendering method */
		virtual void Display(GPURenderer * renderer, GPUProgramProvider * uniform_provider, GPURenderParams const & render_params);
		/** the user defined rendering function */
		virtual void DoDisplay(GPURenderer * renderer, GPUProgramProvider * uniform_provider, GPURenderParams const & render_params);
		/** internal method to prepare rendering */
		virtual void DoPreDisplay(GPURenderer * renderer, GPUProgramProvider * uniform_provider, GPURenderParams const & render_params);		
		/** internal  method to display the game content */
		virtual void DoDisplayGame(GPURenderer * renderer, GPUProgramProvider * uniform_provider, GPURenderParams const & render_params);
		/** internal  method to display the HUD */
		virtual void DoDisplayHUD(GPURenderer * renderer, GPUProgramProvider * uniform_provider, GPURenderParams const & render_params);

		/** fill the rendering params before rendering */
		virtual void FillUniformProvider(GPUProgramProvider & main_uniform_provider);

		/** initialization from the config file */
		virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path);
		/** called whenever the game values as been changed */
		virtual void OnGameValuesChanged(bool hot_reload);

		/** save the best score */
		bool SerializePersistentGameData(bool save);
		/** data internal method serialization */
		virtual bool LoadPersistentGameData(nlohmann::json const& game_data);
		/** data internal method serialization */
		virtual bool SavePersistentGameData(nlohmann::json & game_data) const;
		/** prepare data before saving */
		virtual void UpdatePersistentGameData();

		/** create the gamepad manager */
		virtual bool CreateGamepadManager(nlohmann::json const& config, boost::filesystem::path const& config_path);
		/** initialization of the manager */
		virtual bool InitializeSoundManager(nlohmann::json const& config, boost::filesystem::path const& config_path);
		/** create the game state_machine */
		virtual bool CreateGameStateMachine(nlohmann::json const& config, boost::filesystem::path const& config_path);
		/** allocate the state machine */
		virtual SM::StateMachine * DoCreateGameStateMachine();
		/** create the game state_machine instance */
		virtual SM::StateMachineInstance * DoCreateGameStateMachineInstance(SM::StateMachine * state_machine);

		/** special action on gamepad input reception */
		virtual bool OnGamepadInput(MyGLFW::PhysicalGamepad * in_physical_gamepad);
		/** called whenever a gamepad input is comming */
		virtual bool OnPhysicalGamepadInput(MyGLFW::PhysicalGamepad * physical_gamepad);

		/** called whenever the input mode changes */
		virtual void OnInputModeChanged(InputMode new_mode, InputMode old_mode) override;

		/** create some clocks */
		virtual bool InitializeClocks(nlohmann::json const& config, boost::filesystem::path const& config_path);
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
		GPURenderableLayerSystem * AddChildRenderLayer(char const * layer_name, TagType layer_tag, int render_order);

		/** generate the atlas for the whole game */
		virtual bool GenerateAtlas(nlohmann::json const & config, boost::filesystem::path const & config_path);
		/** fill atlas generation input */
		virtual bool FillAtlasGeneratorInput(BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path);
		/** fill atlas generation input (sprite directory) */
		virtual bool FillAtlasGeneratorInputSprites(BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path);
		/** fill atlas generation input (fonts) */
		virtual bool FillAtlasGeneratorInputFonts(BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path);
		/** fill atlas generation input from the tiled map manager */
		virtual bool FillAtlasGeneratorInputTiledMapManager(BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path);

		/** load object type sets concerned by the game (if required) */
		virtual bool GenerateObjectTypeSets(nlohmann::json const & config, boost::filesystem::path const& config_path);
		/** load tileset concerned by the game (if required) */
		virtual bool GenerateTileSets(nlohmann::json const & config, boost::filesystem::path const& config_path);

		/** a generic function to load some tiled map instances */
		template<typename FUNC>
		bool DoGenerateTiledMapEntity(nlohmann::json const & config, char const * property_name, char const * default_value, char const * extension, FUNC func);

		/** read in config file an entry and open in resource directory a directory iterator on that target */
		boost::filesystem::directory_iterator GetResourceDirectoryIteratorFromConfig(nlohmann::json const & config, char const * config_name, char const * default_path) const;

		/** load all the levels from the game (can be simple data) */
		virtual bool LoadLevels(nlohmann::json const & config, boost::filesystem::path const& config_path);
		/* load one level */
		virtual Level * DoLoadLevel(FilePathParam const & path);
		/** create one tiled map level */
		virtual TMLevel * CreateTMLevel();

		/** the game main loop */
		virtual bool TickGameLoop(float delta_time);
		/** test whether there is a game over */
		virtual bool CheckGameOverCondition();
		/** check whether we must go to next level */
		virtual bool CheckLevelCompletion();
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
		virtual bool OnEnterGame(MyGLFW::PhysicalGamepad * in_physical_gamepad);
		/** called whenever we leave game mode */
		virtual bool OnLeaveGame();

		/** require a pause */
		virtual bool RequirePauseGame();
		/** require a pause or resume */
		virtual bool RequireTogglePause();
		/** require a game Start */
		virtual bool RequireStartGame(MyGLFW::PhysicalGamepad * physical_gamepad);
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
		bool SetCurrentLevel(Level * new_level);
		/** change the current level for the next */
		bool SetNextLevel(bool looping_levels);
		/** reflex method whenever the level is changed */
		virtual void OnLevelChanged(Level * new_level, Level * old_level, LevelInstance * new_level_instance);

		/** generate the game instance entry point*/
		virtual GameInstance* CreateGameInstance();
		/** generate the game instance */
		virtual GameInstance* DoCreateGameInstance();

		/** all sounds flagged in game are set to pause */
		void SetInGameSoundPause(bool in_paused);

		/** create a free camera for the current level instance */
		Camera * CreateFreeCamera() const;
		/** create a free camera for the current level instance */
		Camera * DoCreateFreeCamera(Camera const * camera_to_copy, LevelInstance * level_instance) const;

	protected:

		/** the current gamepad manager */
		shared_ptr<MyGLFW::GamepadManager> gamepad_manager;

		/** the texture atlas */
		shared_ptr<BitmapAtlas::TextureArrayAtlas> texture_atlas;
		/** the particle manager */
		shared_ptr<ParticleManager> particle_manager;

		/** the rendering layer system */
		shared_ptr<GPURenderableLayerSystem> root_render_layer;

		/** the text generator */
		shared_ptr<ParticleTextGenerator::Generator> particle_text_generator;

		/** the sounds being played */
		shared_ptr<Sound> menu_music;
		shared_ptr<Sound> game_music;
		shared_ptr<Sound> pause_music;

		/** the HUDs */
		shared_ptr<GameHUD> hud;

		/** pointer on the state_machine */
		shared_ptr<SM::StateMachine> game_sm;
		SubClassOf<SM::StateMachine> game_sm_class;
		/** pointer on the state_machine instance */
		shared_ptr<SM::StateMachineInstance> game_sm_instance;

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
		shared_ptr<Clock> root_clock;

		/** the particle tools */
		GameParticleCreator particle_creator;

		/** a tiled map manager */
		shared_ptr<TiledMap::Manager> tiled_map_manager;

		/** level data */
		std::vector<shared_ptr<Level>> levels;
		/** the current level instance */
		shared_ptr<LevelInstance> level_instance;

		/** some allocations */
		shared_ptr<ParticleAllocationBase> background_allocations;

		/** the game instance */
		shared_ptr<GameInstance> game_instance;
		SubClassOf<GameInstance> game_instance_class;

		/** the free camera */
		mutable shared_ptr<Camera> free_camera;
		/** free camera mode */
		bool free_camera_mode = false;

		
	};

}; // namespace chaos



#endif // CHAOS_FORWARD_DECLARATION
