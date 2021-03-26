#include <chaos/Chaos.h>

namespace chaos
{
	CHAOS_IMPLEMENT_GAMEPLAY_GETTERS(Game);

	Game::Game()
	{
		game_sm_class = GameStateMachine::GetStaticClass();
	}

	int Game::GetBestPlayerScore() const
	{
		if (game_instance != nullptr)
			return game_instance->GetBestPlayerScore();
		return 0;
	}

	void Game::OnInputModeChanged(InputMode new_mode, InputMode old_mode)
	{
	}

#if _DEBUG
	void Game::SetCheatSkipLevelRequired(bool value)
	{
		cheat_skip_level_required = value;
	}

	bool Game::GetCheatSkipLevelRequired() const
	{
		return cheat_skip_level_required;
	}

	void Game::SetCheatMode(bool value)
	{
		cheat_mode = value;
	}

	CHAOS_HELP_TEXT(CMD, "-CheatMode");

	bool Game::GetCheatMode() const
	{
		if (Application::HasApplicationCommandLineFlag("-CheatMode")) // CMDLINE
			return true;
		if (IsFreeCameraMode())
			return true;
		return cheat_mode;
	}
#endif // _DEBUG

	void Game::TickGameInputs(float delta_time)
	{
		// catch all stick inputs
		if (gamepad_manager != nullptr)
			gamepad_manager->Tick(delta_time);
	}

	void Game::Tick(float delta_time)
	{
		// update player inputs
		TickGameInputs(delta_time);
		// tick the free camera
		if (free_camera != nullptr)
			free_camera->Tick(delta_time);
		// update the game state_machine
		if (game_sm_instance != nullptr)
			game_sm_instance->Tick(delta_time, nullptr);
		// update the game instance
		if (game_instance != nullptr)
			game_instance->Tick(delta_time);		
		// tick the particle manager
		if (particle_manager != nullptr)
			particle_manager->Tick(delta_time);
		// tick the hud
		if (hud != nullptr)
			hud->Tick(delta_time);
	}

#if _DEBUG
	CHAOS_HELP_TEXT(SHORTCUTS, "F1  : SkipLevel");
	CHAOS_HELP_TEXT(SHORTCUTS, "F2  : ToggleCheatMode");
	CHAOS_HELP_TEXT(SHORTCUTS, "F3  : ReloadGameConfiguration");
	CHAOS_HELP_TEXT(SHORTCUTS, "F4  : ReloadCurrentLevel");
	CHAOS_HELP_TEXT(SHORTCUTS, "F5  : QuickSave");
	CHAOS_HELP_TEXT(SHORTCUTS, "F6  : ToggleFreeCameraMode");
#endif

	bool Game::OnKeyEventImpl(KeyEvent const& event)
	{
		// try start the game
		if (game_instance == nullptr)
		{
			if (event.IsKeyPressed())
				RequireStartGame(nullptr);
			return true;
		}

		// give opportunity to game instance to response		
		if (game_instance != nullptr)
			if (game_instance->OnKeyEvent(event))
				return true;

		// PLAYING to PAUSE
		if (event.IsKeyPressed(GLFW_KEY_KP_ENTER) || event.IsKeyPressed(GLFW_KEY_ENTER))
			if (RequireTogglePause())
				return true;
		// QUIT GAME
		if (event.IsKeyPressed(GLFW_KEY_ESCAPE, GLFW_MOD_SHIFT))
		{
			if (RequireExitGame())
				return true;
		}
		else if (event.IsKeyPressed(GLFW_KEY_ESCAPE))
		{
			if (RequireTogglePause())
				return true;
		}
		// CHEAT CODE TO SKIP LEVEL
#if _DEBUG
			
		// CMD GLFW_KEY_F1  : SetCheatSkipLevelRequired(...)	
		if (event.IsKeyPressed(GLFW_KEY_F1))
		{
			SetCheatSkipLevelRequired(true);
			return true;
		}
		// CMD GLFW_KEY_F2  : SetCheatMode(...)
		if (event.IsKeyPressed(GLFW_KEY_F2))
		{
			SetCheatMode(!GetCheatMode());
			return true;
		}
		// CMD GLFW_KEY_F3  : ReloadGameConfiguration(...)
		if (event.IsKeyPressed(GLFW_KEY_F3))
		{
			ReloadGameConfiguration();
			return true;
		}
		// CMD GLFW_KEY_F4  : ReloadCurrentLevel(...)
		if (event.IsKeyPressed(GLFW_KEY_F4))
		{
			ReloadCurrentLevel();
			return true;
		}
		// CMD GLFW_KEY_F5  : QuickSave = SaveToCheckpoint(...)
		if (event.IsKeyPressed(GLFW_KEY_F5))
		{
			SaveIntoCheckpoint();
			return true;
		}
		// CMD GLFW_KEY_F6  : SetFreeCameraMode(...)
		if (event.IsKeyPressed(GLFW_KEY_F6))
		{
			SetFreeCameraMode(!IsFreeCameraMode());
			return true;
		}
#endif

		return false;
	}

	bool Game::OnCharEventImpl(unsigned int c)
	{
		// give the game instance opportunity to capture the input
		if (game_instance != nullptr)
			if (game_instance->OnCharEvent(c))
				return true;
		return false;
	}

	bool Game::OnMouseButtonImpl(int button, int action, int modifier)
	{
		// try start the game
		if (game_instance == nullptr)
		{
			if (action == GLFW_PRESS)
				RequireStartGame(nullptr);
			return true;
		}
		// give opportunity to game instance to response
		if (game_instance->OnMouseButton(button, action, modifier))
			return true;
		return false;
	}

	bool Game::OnMouseMoveImpl(double x, double y)
	{
		// give the game instance opportunity to capture the input
		if (game_instance != nullptr)
			if (game_instance->OnMouseMove(x, y))
				return true;
		return false;
	}

	box2 Game::GetRequiredViewport(glm::ivec2 const & size) const
	{
		box2 viewport = box2(std::make_pair(
			glm::vec2(0.0f, 0.0f),
			RecastVector<glm::vec2>(size)
		));
		return ShrinkBoxToAspect(viewport, viewport_wanted_aspect);
	}

	void Game::Display(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params)
	{
		GPUProgramProviderChain main_uniform_provider(uniform_provider);
		// fill the provider(last inserted have greater precedence)
		FillUniformProvider(main_uniform_provider);
		if (game_instance != nullptr)
			game_instance->FillUniformProvider(main_uniform_provider);
		if (level_instance != nullptr)
			level_instance->FillUniformProvider(main_uniform_provider);
		// rendering
		DoDisplay(renderer, &main_uniform_provider, render_params);
	}

	void Game::FillUniformProvider(GPUProgramProvider & main_uniform_provider)
	{
		// the view box
		box2 view = GetCanvasBox();
		main_uniform_provider.AddVariable("canvas_box", EncodeBoxToVector(view));





		// the world
		box2 world = GetWorldBox();
		main_uniform_provider.AddVariable("world_box", EncodeBoxToVector(world));
		// the time
		double root_time = GetRootClockTime();
		main_uniform_provider.AddVariable("root_time", root_time);

		// some deduced transformations
		main_uniform_provider.AddProvider(new GPUProgramProviderCommonTransforms);
	}

	void Game::DoDisplay(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params)
	{
		// clear the main render target
		DoPreDisplay(renderer, uniform_provider, render_params);
		// display the level instance
		DoDisplayGame(renderer, uniform_provider, render_params);
		// display the hud (AFTER the level)
		DoDisplayHUD(renderer, uniform_provider, render_params);
	}


	void Game::DoPreDisplay(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params)
	{
		// clear the color buffers
		glm::vec4 clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		// clear the depth buffers
		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		// some states
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
	}

	
	void Game::DoDisplayBackground(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params)
	{
		if (background_mesh != nullptr)
			background_mesh->Display(renderer, uniform_provider, render_params);
	}

	void Game::DoDisplayGame(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params)
	{		
		// display the background
		DoDisplayBackground(renderer, uniform_provider, render_params);

		// shuwww   root_render_layer ??
		if (particle_manager != nullptr)
			particle_manager->Display(renderer, uniform_provider, render_params);
		if (level_instance != nullptr)
			level_instance->Display(renderer, uniform_provider, render_params);
	}

	void Game::DoDisplayHUD(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params)
	{	
		if (hud != nullptr)
			hud->Display(renderer, uniform_provider, render_params);
	}

	bool Game::FillAtlasGeneratorInput(BitmapAtlas::AtlasInput & input)
	{
		if (tiled_map_manager != nullptr)
		{
			// find or create folder
			BitmapAtlas::FolderInfoInput * folder_input = input.AddFolder("sprites", 0);
			if (folder_input == nullptr)
				return false;
			// add sprites from TiledMap
			if (!TMTools::AddIntoAtlasInput(tiled_map_manager.get(), folder_input))
				return false;
		}
		return true;
	}

	TMLevel * Game::CreateTMLevel()
	{
		return nullptr;
	}

	Level * Game::DoLoadLevel(FilePathParam const & path)
	{
		boost::filesystem::path const & resolved_path = path.GetResolvedPath();

		if (FileTools::IsTypedFile(resolved_path, "tmx"))
		{
			// create the tiledmap manager if necessary
			if (tiled_map_manager == nullptr)
			{
				tiled_map_manager = new TiledMap::Manager;
				if (tiled_map_manager == nullptr)
					return nullptr;
			}
			// load the resource
			TiledMap::Map* tiled_map = tiled_map_manager->LoadMap(path, false); // XXX : false => dont keep it in manager => necessary for HOTRELOAD

			if (tiled_map == nullptr)
				return false;
			// allocate a level
			TMLevel * result = CreateTMLevel();
			if (result == nullptr)
				return false;
			// some additionnal computation
			if (!result->Initialize(tiled_map))
			{
				delete(result);
				return nullptr;
			}
			return result;
		}

		return nullptr;
	}

	boost::filesystem::directory_iterator Game::GetResourceDirectoryIteratorFromConfig(nlohmann::json const & config, char const * config_name, char const * default_path) const
	{
		// read in the config file the whole path
		std::string directory;
		if (JSONTools::GetAttribute(config, config_name, directory))
			return FileTools::GetDirectoryIterator(directory);
		// concat the argument path with 'resources'
		if (default_path != nullptr)
		{
			// get the application
			WindowApplication * application = Application::GetInstance();
			if (application != nullptr)
			{
				// compute resource path
				boost::filesystem::path resources_path = application->GetResourcesPath();
				boost::filesystem::path result_path = resources_path / default_path;

				return FileTools::GetDirectoryIterator(result_path);
			}
		}
		return boost::filesystem::directory_iterator();
	}

	template<typename FUNC>
	bool Game::DoGenerateTiledMapEntity(nlohmann::json const & config, char const * property_name, char const * default_value, char const * extension, FUNC func)
	{
		// iterate the files and load the tilesets
		boost::filesystem::directory_iterator end;

		for (boost::filesystem::directory_iterator it = GetResourceDirectoryIteratorFromConfig(config, property_name, default_value); it != end; ++it)
		{
			boost::filesystem::path p = it->path();

			if (FileTools::IsTypedFile(p, extension))
			{
				// create the tiledmap manager if necessary
				if (tiled_map_manager == nullptr)
				{
					tiled_map_manager = new TiledMap::Manager;
					if (tiled_map_manager == nullptr)
						return false;
				}
				if (!func(tiled_map_manager.get(), p))
					return false;
			}
		}
		return true;
	}

	bool Game::GenerateObjectTypeSets(nlohmann::json const & config, boost::filesystem::path const& config_path)
	{
		return DoGenerateTiledMapEntity(config, "objecttypesets_directory", "objecttypesets", "xml", [](TiledMap::Manager * manager, boost::filesystem::path const & path) {
			if (!manager->LoadObjectTypeSet(path))
				return false; 
			return true;
		});
	}

	bool Game::GenerateTileSets(nlohmann::json const & config, boost::filesystem::path const& config_path)
	{
		return DoGenerateTiledMapEntity(config, "tilesets_directory", "tilesets", "tsx", [](TiledMap::Manager * manager, boost::filesystem::path const & path) {
			if (!manager->LoadTileSet(path))
				return false;
			return true;
		});
	}

	bool Game::LoadLevels(nlohmann::json const & config, boost::filesystem::path const& config_path)
	{
		// iterate the files and load the levels
		boost::filesystem::directory_iterator end;
		for (boost::filesystem::directory_iterator it = GetResourceDirectoryIteratorFromConfig(config, "levels_directory", "levels"); it != end; ++it)
		{
			int level_index = StringTools::SkipAndAtoi(it->path().filename().string().c_str());

			// create the level
			Level * level = DoLoadLevel(it->path());
			if (level == nullptr)
				continue;
			// initialize it
			level->SetName(BoostTools::PathToName(it->path()).c_str());
			level->SetPath(it->path());
			level->level_index = level_index;
			// store it
			levels.push_back(level);
		}

		// sort the levels
		std::sort(levels.begin(), levels.end(),
			[](shared_ptr<Level> & l1, shared_ptr<Level> & l2)
		{
			return (l1->level_index < l2->level_index);
		});
		return true;
	}

#define CHAOS_FIND_RENDERABLE_CHILD(result, funcname, constness, param_type)\
	result constness * Game::funcname(param_type param, GPURenderableLayerSystem constness * root) constness\
	{\
		if (root == nullptr)\
		{\
			root = root_render_layer.get();\
			if (root == nullptr)\
				return nullptr;\
		}\
		return auto_cast(root->FindChildRenderable(param));\
	}
#define CHAOS_FIND_RENDERABLE_CHILD_ALL(result, funcname)\
	CHAOS_FIND_RENDERABLE_CHILD(result, funcname, BOOST_PP_EMPTY(), char const *);\
	CHAOS_FIND_RENDERABLE_CHILD(result, funcname, BOOST_PP_EMPTY(), TagType);\
	CHAOS_FIND_RENDERABLE_CHILD(result, funcname, const, char const *);\
	CHAOS_FIND_RENDERABLE_CHILD(result, funcname, const, TagType);\

	CHAOS_FIND_RENDERABLE_CHILD_ALL(GPURenderableLayerSystem, FindRenderableLayer);
	CHAOS_FIND_RENDERABLE_CHILD_ALL(ParticleLayerBase, FindParticleLayer);

#undef CHAOS_FIND_RENDERABLE_CHILD_ALL
#undef CHAOS_FIND_RENDERABLE_CHILD

	GPURenderableLayerSystem * Game::AddChildRenderLayer(char const * layer_name, TagType layer_tag, int render_order)
	{
		GPURenderableLayerSystem * result = new GPURenderableLayerSystem();
		if (root_render_layer == nullptr)
			return result;
		result->SetName(layer_name);
		result->SetTag(layer_tag);
		root_render_layer->AddChildRenderable(result, render_order);
		return result;
	}

	bool Game::CreateRootRenderLayer()
	{
		root_render_layer = new GPURenderableLayerSystem();
		if (root_render_layer == nullptr)
			return false;



		// shuwww



		if (AddChildRenderLayer("GAME", GameHUDKeys::GAME_LAYER_ID, 1) == nullptr)
			return false;
		if (AddChildRenderLayer("PLAYER", GameHUDKeys::PLAYER_LAYER_ID, 2) == nullptr) // maybe the player will go in another layer
			return false;
		if (AddChildRenderLayer("HUD", GameHUDKeys::HUD_LAYER_ID, 3) == nullptr)
			return false;

		return true;
	}

	bool Game::CreateParticleManager()
	{
		// get the application for atlas
		WindowApplication* window_application = Application::GetInstance();
		if (window_application == nullptr)
			return false;
		// create the manager
		particle_manager = new ParticleManager();
		if (particle_manager == nullptr)
			return false;
		particle_manager->SetTextureAtlas(window_application->GetTextureAtlas());
		if (AddParticleLayers() < 0)
			return false;
		return true;
	}

	int Game::AddParticleLayers()
	{
		return 0;
	}

	bool Game::CreateBackgroundImage(ObjectRequest material_request, ObjectRequest texture_request)
	{
		if (material_request.IsNoneRequest())
			material_request = "background";

		GPUResourceManager* resource_manager = WindowApplication::GetGPUResourceManagerInstance();
		if (resource_manager != nullptr)
		{
			// search declared material
			shared_ptr<GPURenderMaterial> background_material = resource_manager->FindRenderMaterial(material_request); // use shared_ptr because we may create a child material
			if (background_material != nullptr)
			{
				if (!texture_request.IsNoneRequest())
				{
					// search the corresponding texture
					GPUTexture* texture = resource_manager->FindTexture(texture_request);
					if (texture == nullptr)
						return false;
					// create a child material
					GPURenderMaterial* child_material = new GPURenderMaterial();
					if (child_material == nullptr)
						return false;
					// initialize the material with parent ande texture
					child_material->SetParentMaterial(background_material.get());
					child_material->GetUniformProvider().AddTexture("background", texture);
					background_material = child_material;
				}

				shared_ptr<GPUVertexDeclaration> vertex_declaration = new GPUVertexDeclaration;
				if (vertex_declaration != nullptr)
				{
					background_mesh = new GPUDynamicMesh;
					if (background_mesh != nullptr)
					{
						GetTypedVertexDeclaration(vertex_declaration.get(), boost::mpl::identity<VertexDefault>());
						PrimitiveOutput<VertexDefault> output(background_mesh.get(), nullptr, vertex_declaration.get(), background_material.get(), 4);

						auto quad = output.AddQuads(1);

						quad[0].position = { -1.0f , -1.0f };
						quad[1].position = {  1.0f , -1.0f };
						quad[2].position = {  1.0f ,  1.0f };
						quad[3].position = { -1.0f ,  1.0f };

						for (auto& v : quad)
						{
							v.texcoord.x = v.position.x * 0.5f + 0.5f;
							v.texcoord.y = v.position.y * 0.5f + 0.5f;
							v.texcoord.z = 0.0f;
						}
					}
				}
			}
		}
		return true;
	}

	bool Game::CreateClocks(nlohmann::json const& config, boost::filesystem::path const& config_path)
	{
		Clock* application_clock = GetApplicationClock();
		if (application_clock == nullptr)
			return false;

		root_clock = application_clock->CreateChildClock("root_clock");
		if (root_clock == nullptr)
			return false;
		return true;
	}

	bool Game::CreateGamepadManager(nlohmann::json const& config, boost::filesystem::path const& config_path)
	{
		bool gamepad_enabled = true;
		JSONTools::GetAttribute(config, "gamepad_enabled", gamepad_enabled, true);
		if (gamepad_enabled)
		{
			gamepad_manager = new GameGamepadManager(this);
			if (gamepad_manager == nullptr)
				return false;
		}
		return true;
	}

	bool Game::InitializeFromConfiguration(nlohmann::json const& config, boost::filesystem::path const& config_path)
	{
		// initialize the gamepad manager
		if (!CreateGamepadManager(config, config_path))
			return false;
		// create game state_machine
		if (!CreateGameStateMachine(config, config_path))
			return false;
		// initialize clocks
		if (!CreateClocks(config, config_path))
			return false;
		// initialize game values
		if (!InitializeGameValues(config, config_path, false)) // false => not hot_reload
			return false;
		OnGameValuesChanged(false);
		// loading object type sets
		if (!GenerateObjectTypeSets(config, config_path))
			return false;
		// loading tilemapset
		if (!GenerateTileSets(config, config_path))
			return false;
		// load exisiting levels
		if (!LoadLevels(config, config_path))
			return false;

		// load the best score if any
		SerializePersistentGameData(false);
		return true;
	}

	bool Game::CreateGPUResources()
	{
		// initialize the root render system
		if (!CreateRootRenderLayer())
			return false;
		// initialize the particle manager
		if (!CreateParticleManager())
			return false;
		// create the game background
		if (!CreateBackgroundImage(nullptr, nullptr))
			return false;
		return true;
	}

	SoundManager * Game::GetSoundManager()
	{
		WindowApplication * application = Application::GetInstance();
		if (application == nullptr)
			return nullptr;
		return application->GetSoundManager();
	}

	Clock * Game::GetApplicationClock()
	{
		WindowApplication * application = Application::GetInstance();
		if (application == nullptr)
			return nullptr;
		return application->GetMainClock();
	}

	Clock const * Game::GetApplicationClock() const
	{
		WindowApplication const * application = Application::GetInstance();
		if (application == nullptr)
			return nullptr;
		return application->GetMainClock();
	}

	double Game::GetRootClockTime() const
	{
		if (root_clock == nullptr)
			return 0.0;
		return root_clock->GetClockTime();
	}

	void Game::UpdatePersistentGameData()
	{
		best_score = std::max(best_score, GetBestPlayerScore());
	}

	bool Game::LoadPersistentGameData(nlohmann::json const& game_data)
	{
		JSONTools::GetAttribute(game_data, "best_score", best_score);
		return true;
	}

	bool Game::SavePersistentGameData(nlohmann::json & game_data) const
	{
		JSONTools::SetAttribute(game_data, "best_score", best_score);
		return true;
	}

	bool Game::SerializePersistentGameData(bool save)
	{
		// get application
		Application * application = Application::GetInstance();
		if (application == nullptr)
			return false;
		// get user temp directory
		boost::filesystem::path filepath = application->GetUserLocalTempPath() / "game_data.json";

		nlohmann::json game_data;
		// save the game data
		if (save)
		{
			if (!SavePersistentGameData(game_data))
				return false;
			std::ofstream file(filepath.string().c_str());
			if (!file)
				return false;
			file << game_data.dump();
			return true;
		}
		// load the game data
		else
		{	
			if (!JSONTools::LoadJSONFile(filepath, game_data, false))
				return false;
			return LoadPersistentGameData(game_data);
		}
	}

	CHAOS_HELP_TEXT(CMD, "-MuteMusic");

	Sound * Game::SetInGameMusic(char const * music_name)
	{
		assert(music_name != nullptr);

#if _DEBUG
		if (Application::HasApplicationCommandLineFlag("-MuteMusic")) // CMDLINE
			return nullptr;
#endif
		
		// ensure there is a real music change
		if (game_music != nullptr && !game_music->IsPendingKill())
		{
			SoundManager * sound_manager = GetSoundManager();
			if (sound_manager == nullptr)
				return nullptr;

			SoundSource * new_source = sound_manager->FindSource(music_name);

			if (new_source == game_music->GetSource())
				return game_music.get();
		}

		// effectively change the music
		float BLEND_TIME = 2.0f;

		// destroy previous music
		bool previous_music = false;
		if (game_music != nullptr)
		{
			game_music->FadeOut(BLEND_TIME, true);
			game_music = nullptr;
			previous_music = true;
		}
		// start new music
		game_music = PlaySound(music_name, false, true, (previous_music) ? BLEND_TIME : 0.0f, SoundContext::GAME);
		return game_music.get();
	}

	Sound * Game::PlaySound(char const * name, PlaySoundDesc play_desc, TagType category_tag)
	{
		// search manager
		SoundManager * sound_manager = GetSoundManager();
		if (sound_manager == nullptr)
			return nullptr;
		// search source
		SoundSource * source = sound_manager->FindSource(name);
		if (source == nullptr)
			return nullptr;

		// Add some categories depending on the wanted sound context
		if (category_tag == SoundContext::GAME || category_tag == SoundContext::LEVEL)
		{
			if (game_instance != nullptr)
				if (game_instance->sound_category != nullptr && !game_instance->sound_category->IsPendingKill())
					play_desc.categories.push_back(game_instance->sound_category.get());

			if (category_tag == SoundContext::LEVEL)
			{
				if (level_instance != nullptr)
					if (level_instance->sound_category != nullptr && !level_instance->sound_category->IsPendingKill())
						play_desc.categories.push_back(level_instance->sound_category.get());
			}
		}

		return source->Play(play_desc);
	}

	Sound * Game::PlaySound(char const * name, bool paused, bool looping, float blend_in_time, TagType category_tag)
	{
		PlaySoundDesc play_desc;
		play_desc.paused = paused;
		play_desc.looping = looping;
		play_desc.blend_in_time = blend_in_time;

		return PlaySound(name, play_desc, category_tag);
	}

	bool Game::CreateGameStateMachine(nlohmann::json const& config, boost::filesystem::path const& config_path)
	{
		game_sm = DoCreateGameStateMachine();
		if (game_sm == nullptr)
			return false;
		if (!game_sm->Initialize()) // create all internal states and transition
			return false;
		game_sm_instance = DoCreateGameStateMachineInstance(game_sm.get());
		if (game_sm_instance == nullptr)
			return false;
		return true;
	}

	SM::StateMachine * Game::DoCreateGameStateMachine()
	{
		return game_sm_class.CreateInstance();
	}

	SM::StateMachineInstance * Game::DoCreateGameStateMachineInstance(SM::StateMachine * state_machine)
	{
		return new GameStateMachineInstance(this, state_machine);
	}

	bool Game::OnGamepadInput(PhysicalGamepad * in_physical_gamepad) // an uncatched gamepad input incomming
	{
		assert(in_physical_gamepad != nullptr && !in_physical_gamepad->IsAllocated());

		// try start the game
		if (game_instance == nullptr)
		{
			if (in_physical_gamepad->IsAnyButtonJustPressed())
				RequireStartGame(in_physical_gamepad);
			return true;
		}
		// give opportunity to game instance to response
		if (game_instance->OnGamepadInput(in_physical_gamepad))
			return true;
		return false;
	}

	bool Game::OnPhysicalGamepadInput(PhysicalGamepad * physical_gamepad)
	{
		// ignore invalid gamepad : should never happen
		if (!physical_gamepad->IsAnyAction())
			return true;
		// change the application mode
		SetInputMode(InputMode::GAMEPAD);
		// special action on gamepad input
		OnGamepadInput(physical_gamepad);

		return true;
	}

	bool Game::InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path, bool hot_reload)
	{
		// keep a reference on the configuration path
		configuration_path = config_path;

		// capture the game instance configuration
		nlohmann::json const* gi_config = JSONTools::GetStructure(config, "game_instance");
		if (gi_config != nullptr && gi_config->is_object())
			game_instance_configuration = *gi_config;
		else
			game_instance_configuration = nlohmann::json();

		// read dedicated game values
		CHAOS_JSON_ATTRIBUTE(config, mouse_sensitivity);
		CHAOS_JSON_ATTRIBUTE(config, gamepad_sensitivity);
		CHAOS_JSON_ATTRIBUTE(config, viewport_wanted_aspect);
		return true;
	}

	void Game::OnGameValuesChanged(bool hot_reload)
	{
		if (game_instance != nullptr)
			if (game_instance->InitializeGameValues(game_instance_configuration, configuration_path, hot_reload))
				game_instance->OnGameValuesChanged(hot_reload);
	}

	void Game::OnEnterMainMenu(bool very_first)
	{
		// start the music
#if _DEBUG
		if (Application::HasApplicationCommandLineFlag("-MuteMusic")) // CMDLINE
			menu_music = nullptr;
		else
#endif
		menu_music = PlaySound("menu_music", false, true, 0.0f, SoundContext::MAINMENU);
		game_music = nullptr;
		pause_music = nullptr;
		// restore the background image
		CreateBackgroundImage(nullptr, nullptr);
		// create the main menu HUD
		CreateMainMenuHUD();
	}

	void Game::OnLeaveMainMenu()
	{
		// stop the music
		if (menu_music != nullptr)
		{
			menu_music->FadeOut(0.5f, true);
			menu_music = nullptr;
		}
	}

	void Game::OnGameOver()
	{
		// internal code
		CreateGameOverHUD();
		// give opportunity to other game classes to respond
		if (level_instance != nullptr)
			level_instance->OnGameOver();
		if (game_instance != nullptr)
			game_instance->OnGameOver();		
	}

	bool Game::OnEnterPause()
	{
		// start sound
#if _DEBUG
		if (Application::HasApplicationCommandLineFlag("-MuteMusic")) // CMDLINE
			pause_music = nullptr;
		else
#endif
		pause_music = PlaySound("pause_music", false, true, 0.0f, SoundContext::PAUSEMENU);
		// internal code
		CreatePauseMenuHUD();
		// give opportunity to other game classes to respond
		if (level_instance != nullptr)
			level_instance->OnEnterPause();
		if (game_instance != nullptr)
			game_instance->OnEnterPause();
		// pause in-game sounds
		SetInGameSoundPause(true);
		return true;
	}

	bool Game::OnLeavePause()
	{
		// destroy the pause music
		if (pause_music != nullptr)
		{
			pause_music->FadeOut(0.5f, true);
			pause_music = nullptr;
		}
		// internal code
		CreatePlayingHUD();
		// give opportunity to other game classes to respond
		if (level_instance != nullptr)
			level_instance->OnLeavePause();
		if (game_instance != nullptr)
			game_instance->OnLeavePause();
		// resume in-game sounds
		SetInGameSoundPause(false);

		return true;
	}

	void Game::SetInGameSoundPause(bool in_paused)
	{
		SoundCategory * categories[2] = { nullptr, nullptr };

		if (game_instance != nullptr)
			categories[0] = game_instance->GetSoundCategory();
		if (level_instance != nullptr)
			categories[1] = level_instance->GetSoundCategory();

		for (int i = 0; i < 2; ++i)
		{
			SoundCategory * category = categories[i];
			if (category == nullptr)
				continue;
			if (in_paused)
				category->StartBlend(BlendVolumeDesc::BlendOut(0.5f, true, false), true);
			else
			{
				category->StartBlend(BlendVolumeDesc::BlendIn(0.5f), true);
				category->Pause(false);
			}
		}
	}

	GameInstance* Game::CreateGameInstance()
	{
		// create the game instance
		GameInstance * result = DoCreateGameInstance();
		if (result == nullptr)
			return nullptr;
		// initialize the instance
		if (!result->Initialize(this))
		{
			delete(result);
			return nullptr;
		}
		return result;
	}

	bool Game::OnEnterGame(PhysicalGamepad * in_physical_gamepad)
	{
		assert(game_instance == nullptr);
		// create the game instance
		game_instance = CreateGameInstance();
		if (game_instance == nullptr)
			return false;
		// create other resources
		game_instance->OnEnterGame();
		// game entered			
		CreatePlayingHUD();
		// start the level
		SetNextLevel(true);
		// create a first player and insert it
		game_instance->CreatePlayer(in_physical_gamepad);

		return true;
	}

	bool Game::OnLeaveGame()
	{
		// update game data that must me prevent from destruction
		UpdatePersistentGameData();
		// save the best score (and other values)
		SerializePersistentGameData(true);
		// restore main menu condition (level, music ...)
		SetCurrentLevel(nullptr);	
		// game instance stop
		game_instance->OnLeaveGame();
		// destroy the game instance 
		game_instance = nullptr;
		return true;
	}

	bool Game::CheckGameOverCondition()
	{
		// check for game over in game instance
		if (game_instance != nullptr)
			if (game_instance->DoCheckGameOverCondition())
				return true;
		return false; // no gameover
	}


	bool Game::CheckLevelCompletion()
	{
		// cheat code
#if _DEBUG
		if (GetCheatSkipLevelRequired())
			return true;
#endif
		// level knows about that
		LevelInstance const * level_instance = GetLevelInstance();
		if (level_instance != nullptr)
			if (level_instance->CheckLevelCompletion())
				return true;
		return false;
	}

	bool Game::CanCompleteLevel()
	{
		// shurefactor




		// game instance knows about that
		GameInstance const* game_instance = GetGameInstance();
		if (game_instance != nullptr)
			if (!game_instance->CanCompleteLevel())
				return false;
		// level knows about that
		LevelInstance const * level_instance = GetLevelInstance();
		if (level_instance != nullptr)
			if (!level_instance->CanCompleteLevel())
				return false;
		return true;
	}

	

	bool Game::TickGameLoop(float delta_time)
	{

		// shurefactor


		// level finished
		if (CheckLevelCompletion())
		{
			bool can_complete = 
#if _DEBUG
				GetCheatSkipLevelRequired() ||
#endif
				CanCompleteLevel();

			if (can_complete) // maybe there is a small delay for an animation or a sound
			{
				if (!SetNextLevel(looping_levels))
					RequireExitGame();
				return false; // do not call remaining code in TickGameLoop(...) specialization
			}
		}

		// shu46 : can have game over even if CheckLevelCompletion() !!!



		// game over ? (not if level is finished and wait for completion)
		if (CheckGameOverCondition())
		{
			RequireGameOver();
			return false;
		}
		// tick the level
		if (level_instance != nullptr)
			level_instance->Tick(delta_time);
		return true;
	}

	TagType Game::GetCurrentStateTag(bool strict_state, bool use_destination) const
	{
		if (game_sm_instance == nullptr)
			return -1;
		SM::StateBase const * current_state = (strict_state)?
			game_sm_instance->GetCurrentStrictState(use_destination) :
			game_sm_instance->GetCurrentState();
		if (current_state == nullptr)
			return -1;
		return current_state->GetTag();
	}

	bool Game::IsPlaying(bool strict_state, bool use_destination) const
	{
		if (GetCurrentStateTag(strict_state, use_destination) == GameStateMachineKeys::STATE_PLAYING)
			return true;
		return false;
	}

	bool Game::IsPaused(bool strict_state, bool use_destination) const
	{
		if (GetCurrentStateTag(strict_state, use_destination) == GameStateMachineKeys::STATE_PAUSE)
			return true;
		return false;
	}

	bool Game::RequireGameOver()
	{
		if (game_sm_instance->SendEvent(GameStateMachineKeys::EVENT_GAME_OVER, nullptr))
			return true;
		return false;
	}

	bool Game::RequirePauseGame()
	{
		if (!IsPaused())
			return RequireTogglePause();
		return false;
	}

	bool Game::RequireTogglePause()
	{
		if (game_sm_instance->SendEvent(GameStateMachineKeys::EVENT_TOGGLE_PAUSE, nullptr))
			return true;
		return false;
	}

	bool Game::RequireExitGame()
	{
		if (game_sm_instance->SendEvent(GameStateMachineKeys::EVENT_EXIT_GAME, nullptr))
			return true;
		return false;
	}

	bool Game::RequireStartGame(PhysicalGamepad * physical_gamepad)
	{
		PhysicalGamepadWrapper game_pad_wrapper = PhysicalGamepadWrapper(physical_gamepad);

		if (game_sm_instance->SendEvent(GameStateMachineKeys::EVENT_START_GAME, &game_pad_wrapper))
			return true;
		return false;
	}

#define CHAOS_IMPLEMENT_HUD_FUNCTIONS(classname)\
	bool Game::Create##classname()\
	{\
		hud = DoCreate##classname();\
		if (hud == nullptr)\
			return false;\
		if (!hud->Initialize(this))\
		{\
			hud = nullptr;\
			return false;\
		}\
		return true;\
	}\
	GameHUD * Game::DoCreate##classname()\
	{\
		return new classname();\
	}
	CHAOS_IMPLEMENT_HUD_FUNCTIONS(PauseMenuHUD);
	CHAOS_IMPLEMENT_HUD_FUNCTIONS(MainMenuHUD);
	CHAOS_IMPLEMENT_HUD_FUNCTIONS(PlayingHUD);
	CHAOS_IMPLEMENT_HUD_FUNCTIONS(GameOverHUD);

#undef CHAOS_IMPLEMENT_HUD_FUNCTIONS

	void Game::DestroyHUD()
	{
		hud = nullptr;
	}

	box2 Game::GetCanvasBox() const
	{
		glm::vec2 canvas_size;
		canvas_size.x = 1600.0f;
		canvas_size.y = canvas_size.x / viewport_wanted_aspect;

		box2 result;
		result.position = glm::vec2(0.0f, 0.0f);
		result.half_size = canvas_size * 0.5f;
		return result;
	}

	box2 Game::GetWorldBox() const
	{
		// look at the level instance
		if (level_instance != nullptr)
		{
			box2 result = level_instance->GetBoundingBox();
			if (!IsGeometryEmpty(result))
				return result;
		}
		// by default, the world will be the same size than the screen
		return GetCanvasBox();
	}


	bool Game::SetNextLevel(bool looping_levels)
	{
		// existing any level
		size_t count = levels.size();
		if (count == 0)
			return false;
		// very first level
		Level * current_level = GetLevel();
		if (current_level == nullptr)
			return SetCurrentLevel(levels[0].get());
		// search the current level
		size_t i = 0;
		for (; i < count; ++i)
			if (levels[i].get() == current_level)
				break;
		// level not found ?
		if (i == count)
			return false;
		// very last level
		if (i == count - 1)
		{
			if (looping_levels)
				return SetCurrentLevel(levels[0].get());
			return false;
		}
		// default
		return SetCurrentLevel(levels[i + 1].get());
	}
	
	AutoCastable<Level> Game::FindLevel(ObjectRequest request)
	{
		return request.FindObject(levels);
	}

	AutoConstCastable<Level> Game::FindLevel(ObjectRequest request) const
	{
		return request.FindObject(levels);
	}

	AutoCastable<Level> Game::FindLevelByIndex(int level_index)
	{
		size_t count = levels.size();
		for (size_t i = 0; i < count; ++i)
			if (levels[i]->GetLevelIndex() == level_index)
				return levels[i].get();
		return nullptr;
	}

	AutoConstCastable<Level> Game::FindLevelByIndex(int level_index) const
	{
		size_t count = levels.size();
		for (size_t i = 0; i < count; ++i)
			if (levels[i]->GetLevelIndex() == level_index)
				return levels[i].get();
		return nullptr;
	}

	bool Game::SetCurrentLevel(Level * new_level) // new_level can be set to nullptr, just to clear every thing
	{
		shared_ptr<Level> old_level = GetLevel();

#if _DEBUG
		SetCheatSkipLevelRequired(false);
#endif

		// destroy current level instance, so that new instance can get all resources it want
		if (level_instance != nullptr)
		{
			level_instance->OnLevelEnded();
			level_instance = nullptr;
		}
		// create the new level instance if required
		if (new_level != nullptr)
		{
			level_instance = new_level->CreateLevelInstance(this);
			if (level_instance == nullptr)
				return false;
			level_instance->OnLevelStarted();
		}
		// change the level
		OnLevelChanged(new_level, old_level.get(), level_instance.get());

		return true;
	}

	void Game::OnLevelChanged(Level * new_level, Level * old_level, LevelInstance * new_level_instance)
	{
		// free camera points an invalid 'level_instance'
		if (free_camera != nullptr)
		{
			if (new_level != nullptr && old_level != nullptr && new_level->GetLevelIndex() == old_level->GetLevelIndex()) // level unchanged : want to keep free cam position
				free_camera = DoCreateFreeCamera(free_camera.get(), new_level_instance);
			else
				free_camera = nullptr; // the camera will be created at next tick
		}
		// update the instance
		if (game_instance != nullptr)
			game_instance->OnLevelChanged(new_level, old_level, new_level_instance);
	}

	bool Game::ReloadGameConfiguration()
	{
		// gets application
		WindowApplication * application = Application::GetInstance();
		if (application == nullptr)
			return false;
		// this call may take a while. Avoid Frame rate jump
		application->FreezeNextFrameTickDuration();
		// reload the whole configuration file
		nlohmann::json config;
		if (!application->ReloadConfigurationFile(config))
			return false;
		// extract the part of interest for us
		nlohmann::json const * game_config = JSONTools::GetStructure(config, "game");
		if (game_config == nullptr)
			return false;
		// update game values
		if (!InitializeGameValues(*game_config, application->GetConfigurationPath(), true)) // true => hot_reload
			return false;
		OnGameValuesChanged(true);
		return true;
	}

	bool Game::ReloadCurrentLevel()
	{
		if (level_instance == nullptr)
			return false;

		shared_ptr<Level> old_level = level_instance->GetLevel(); // keep a reference to prevent the destruction when it will be removed from the levels array
		assert(old_level != nullptr);

		// reload the level
		shared_ptr<Level> level = DoLoadLevel(old_level->GetPath()); 
		if (level == nullptr)
			return false;
		// initialize it
		level->SetPath(old_level->GetPath());
		level->level_index = old_level->GetLevelIndex();
		// the effective index of a level is not necessary its position in the array. search it
		size_t index = 0;
		while (index < levels.size() && levels[index].get() != old_level)
			++index;
		if (index == levels.size())
			return false;
		levels[index] = level; // replace the level in the array
		// restart
		SetCurrentLevel(level.get());

		return true;
	}

	GameInstance * Game::DoCreateGameInstance()
	{
		return game_instance_class.CreateInstance();
	}

	bool Game::IsFreeCameraMode() const
	{
		return free_camera_mode;
	}

	void Game::SetFreeCameraMode(bool in_free_camera_mode)
	{
		free_camera_mode = in_free_camera_mode;
		if (!in_free_camera_mode)
			free_camera = nullptr;
	}

	Camera * Game::GetFreeCamera()
	{		
		if (free_camera == nullptr)
			free_camera = CreateFreeCamera();
		return free_camera.get();
	}
	
	Camera const * Game::GetFreeCamera() const
	{
		if (free_camera == nullptr)
			free_camera = CreateFreeCamera();
		return free_camera.get();
	}

	Camera * Game::CreateFreeCamera() const
	{
		LevelInstance const * level_instance = GetLevelInstance();
		if (level_instance != nullptr)
		{
			if (level_instance->GetCameraCount() > 0)
			{
				Camera const * first_camera = level_instance->DoGetCamera(0, false); // do not accept free camera
				if (first_camera != nullptr)
					return DoCreateFreeCamera(first_camera, (LevelInstance *)level_instance);
			}
		}
		return nullptr;
	}

	Camera * Game::DoCreateFreeCamera(Camera const * camera_to_copy, LevelInstance * level_instance) const
	{
		Camera* result = level_instance->CreateCamera();
		if (result != nullptr)
		{
			float zoom_value = 1.0f;

			FreeCameraComponent const * other_free_camera_component = camera_to_copy->FindComponentByClass<FreeCameraComponent>();
			if (other_free_camera_component != nullptr)
				zoom_value = other_free_camera_component->GetZoomValue();

			result->camera_box = camera_to_copy->camera_box;
			result->initial_camera_obox = camera_to_copy->initial_camera_obox;
			result->safe_zone = camera_to_copy->safe_zone;

			FreeCameraComponent * new_free_camera_component = new FreeCameraComponent(0); // player 0
			if (new_free_camera_component != nullptr)
			{
				new_free_camera_component->SetZoomValue(zoom_value);
				result->AddComponent(new_free_camera_component);
			}			

			result->AddComponent(new SoundListenerCameraComponent());
		}
		return result;
	}

	GameCheckpoint * Game::SaveIntoCheckpoint()
	{
		if (game_instance == nullptr)
			return nullptr;
		return game_instance->SaveIntoCheckpoint();
	}

	GameParticleCreator Game::GetGameParticleCreator()
	{
		if (particle_manager != nullptr)
			return particle_manager->GetGameParticleCreator();
		return {};
	}

}; // namespace chaos

