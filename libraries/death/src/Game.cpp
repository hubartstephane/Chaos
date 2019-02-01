#include <death/Game.h>
#include <death/GameStateMachine.h>
#include <death/GamepadManager.h>
#include <death/GameLevel.h>
#include <death/TiledMapLevel.h>

#include <chaos/InputMode.h>
#include <chaos/FileTools.h>
#include <chaos/TiledMap.h>
#include <chaos/TiledMapTools.h>
#include <chaos/CollisionFramework.h>
#include <chaos/ParticleDefault.h>

#include <Death/GameParticles.h>

namespace death
{

	int Game::GetBestPlayerScore() const
	{
		if (game_instance != nullptr)
			return game_instance->GetBestPlayerScore();
		return 0;
	}

	void Game::OnInputModeChanged(int new_mode, int old_mode)
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

	bool Game::GetCheatMode() const
	{
		return cheat_mode;
	}
#endif

	void Game::HandleKeyboardInputs()
	{

	}

	void Game::TickGameInputs(double delta_time)
	{
		// catch all stick inputs
		if (gamepad_manager != nullptr)
			gamepad_manager->Tick((float)delta_time);
		// handle keyboard inputs
		HandleKeyboardInputs();
	}

	void Game::Tick(double delta_time)
	{
		// update player inputs
		TickGameInputs(delta_time);
		// update the game state_machine
		if (game_state_machine_instance != nullptr)
			game_state_machine_instance->Tick(delta_time, nullptr);
		// update the game instance
		if (game_instance != nullptr)
			game_instance->Tick(delta_time);		
		// tick the particle manager
		if (particle_manager != nullptr)
			particle_manager->Tick((float)delta_time);
		// tick the hud
		if (hud != nullptr)
			hud->Tick(delta_time);
	}

	bool Game::OnKeyEvent(int key, int action)
	{
		// give the game instance opportunity to capture the input
		if (game_instance != nullptr)
			if (game_instance->OnKeyEvent(key, action))
				return true;

		// MAIN MENU to PLAYING
		if (action == GLFW_PRESS)
			if (RequireStartGame(nullptr))
				return true;

		// PLAYING to PAUSE
		if ((key == GLFW_KEY_KP_ENTER || key == GLFW_KEY_ENTER || key == GLFW_KEY_P) && action == GLFW_PRESS)
			if (RequireTogglePause())
				return true;

		// QUIT GAME
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			if (RequireExitGame())
				return true;

		// CHEAT CODE TO SKIP LEVEL
#if _DEBUG
		if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
		{
			SetCheatSkipLevelRequired(true);
			return true;
		}
		if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
		{
			SetCheatMode(!GetCheatMode());
			return true;
		}
		if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
		{
			ReloadConfigurationFile();
			return true;
		}
#endif

		return false;
	}

	bool Game::OnCharEvent(unsigned int c)
	{
		// give the game instance opportunity to capture the input
		if (game_instance != nullptr)
			if (game_instance->OnCharEvent(c))
				return true;
		return false;
	}

	bool Game::OnMouseButton(int button, int action, int modifier)
	{
		// give the game instance opportunity to capture the input
		if (game_instance != nullptr)
			if (game_instance->OnMouseButton(button, action, modifier))
				return true;
		// start the game on a 'OnClick'
		if (action == GLFW_PRESS)
			if (RequireStartGame(nullptr))
				return true;
		return false;
	}

	bool Game::OnMouseMove(double x, double y)
	{
		// give the game instance opportunity to capture the input
		if (game_instance != nullptr)
			if (game_instance->OnMouseMove(x, y))
				return true;
		return false;
	}

	void Game::Display(chaos::Renderer * renderer, glm::ivec2 const & size)
	{
		chaos::box2 viewport = chaos::GLTools::SetViewportWithAspect(size, viewport_wanted_aspect);

		// a variable provider
		chaos::GPUProgramProvider main_uniform_provider;
		FillUniformProvider(main_uniform_provider);

		// the viewport
		main_uniform_provider.AddVariableValue("viewport_size", viewport.half_size * 2.0f);

		// the related box
		glm::vec2 view_size = GetViewSize();
		main_uniform_provider.AddVariableValue("view_size", view_size);

		// boxes
		chaos::box2 world = GetWorldBox();
		main_uniform_provider.AddVariableValue("world_box", chaos::EncodeBoxToVector(world));
		chaos::box2 view = GetViewBox();
		main_uniform_provider.AddVariableValue("view_box", chaos::EncodeBoxToVector(world));

		// the time
		double root_time = GetRootClockTime();
		main_uniform_provider.AddVariableValue("root_time", root_time);


		chaos::RenderParams render_params;
		render_params.viewport = viewport;
		render_params.screen_size = size;
		DoDisplay(renderer, &main_uniform_provider, render_params);
	}

	void Game::FillUniformProvider(chaos::GPUProgramProvider & main_uniform_provider)
	{
		if (game_instance != nullptr)
			game_instance->FillUniformProvider(main_uniform_provider);
		if (current_level_instance != nullptr)
			current_level_instance->FillUniformProvider(main_uniform_provider);
	}

	void Game::DoDisplay(chaos::Renderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::RenderParams const & render_params)
	{
		DoDisplayLevel(renderer, uniform_provider, render_params);
		DoDisplayHUD(renderer, uniform_provider, render_params);
	}

	void Game::DoDisplayLevel(chaos::Renderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::RenderParams const & render_params)
	{
		// display the level instance
		if (current_level_instance != nullptr)
			current_level_instance->Display(renderer, uniform_provider, render_params);
	}

	void Game::DoDisplayHUD(chaos::Renderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::RenderParams const & render_params)
	{
		// display the hud (AFTER the level)
		if (hud != nullptr)
			hud->Display(renderer, uniform_provider, render_params);
	}

	bool Game::FillAtlasGenerationInput(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path)
	{
		if (!FillAtlasGenerationInputSprites(input, config, config_path))
			return false;
		if (!FillAtlasGenerationInputFonts(input, config, config_path))
			return false;
		if (!FillAtlasGenerationInputTiledMapManager(input, config, config_path))
			return false;
		return true;
	}

	bool Game::FillAtlasGenerationInputTiledMapManager(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path)
	{
		if (tiled_map_manager != nullptr)
		{
			chaos::BitmapAtlas::FolderInfoInput * folder_input = input.AddFolder("sprites", 0);
			if (folder_input == nullptr)
				return false;
			if (!chaos::TiledMapTools::AddIntoAtlasInput(tiled_map_manager.get(), folder_input))
				return false;
		}
		return true;
	}

	bool Game::FillAtlasGenerationInputSprites(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path)
	{
		// get the directory where the sprites are
		std::string sprite_directory;
		chaos::JSONTools::GetAttribute(config, "sprite_directory", sprite_directory);

		// Add sprites
		chaos::BitmapAtlas::FolderInfoInput * folder_info = input.AddFolder("sprites", 0);
		if (folder_info == nullptr)
			return false;

		folder_info->AddBitmapFilesFromDirectory(sprite_directory, true);

		return true;
	}

	bool Game::FillAtlasGenerationInputFonts(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path)
	{
		if (!FillAtlasGenerationInputOneFont("font_path", "normal", input, config, config_path))
			return false;
		if (!FillAtlasGenerationInputOneFont("title_font_path", "title", input, config, config_path))
			return false;
		return true;
	}

	bool Game::FillAtlasGenerationInputOneFont(char const * font_config_name, char const * font_name, chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path)
	{
		// get the path of the font
		std::string font_path;
		chaos::JSONTools::GetAttribute(config, font_config_name, font_path);

		// Add the font
		chaos::BitmapAtlas::FontInfoInputParams font_params;
		font_params.max_character_width = 64;
		font_params.max_character_height = 64;

		chaos::BitmapAtlas::FontInfoInput * font_info1 =
			input.AddFont(font_path.c_str(), nullptr, true, font_name, 0, font_params);

		return (font_info1 != nullptr);
	}

	bool Game::GenerateAtlas(nlohmann::json const & config, boost::filesystem::path const & config_path)
	{
		// Try to load already computed data (in debug only)
#if _DEBUG
		chaos::BitmapAtlas::TextureArrayAtlas * tmp_texture_atlas = new chaos::BitmapAtlas::TextureArrayAtlas;
		if (tmp_texture_atlas != nullptr)
		{
			chaos::Application * application = chaos::Application::GetInstance();
			if (application != nullptr)
			{
				if (tmp_texture_atlas->LoadAtlas(application->GetUserLocalTempPath() / "LudumAtlas"))
				{
					texture_atlas = tmp_texture_atlas;
					return true;
				}
				delete(tmp_texture_atlas);
			}
		}
#endif

		chaos::BitmapAtlas::AtlasInput input;
		if (!FillAtlasGenerationInput(input, config, config_path))
			return false;

		// generate the atlas
		int ATLAS_SIZE = 1024;
		int ATLAS_PADDING = 10;
		chaos::BitmapAtlas::AtlasGeneratorParams params = chaos::BitmapAtlas::AtlasGeneratorParams(ATLAS_SIZE, ATLAS_SIZE, ATLAS_PADDING, chaos::PixelFormatMergeParams());

#if _DEBUG
		params.debug_dump_atlas_dirname = "LudumAtlas";
#endif

		chaos::BitmapAtlas::TextureArrayAtlasGenerator generator;
		texture_atlas = generator.ComputeResult(input, params);
		if (texture_atlas == nullptr)
			return false;

		return true;
	}

	death::TiledMap::Level * Game::CreateTiledMapLevel()
	{
		return nullptr;
	}

	death::GameLevel * Game::DoLoadLevel(int level_index, chaos::FilePathParam const & path)
	{
		boost::filesystem::path const & resolved_path = path.GetResolvedPath();

		if (chaos::FileTools::IsTypedFile(resolved_path, "tmx"))
		{
			// create the tiledmap manager if necessary
			if (tiled_map_manager == nullptr)
			{
				tiled_map_manager = new chaos::TiledMap::Manager;
				if (tiled_map_manager == nullptr)
					return nullptr;
			}

			// load the resource
			chaos::TiledMap::Map * tiled_map = tiled_map_manager->LoadMap(path);
			if (tiled_map == nullptr)
				return false;

			// allocate a level
			death::TiledMap::Level * result = CreateTiledMapLevel();
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

	bool Game::LoadLevels()
	{
		chaos::MyGLFW::SingleWindowApplication * application = chaos::MyGLFW::SingleWindowApplication::GetGLFWApplicationInstance();
		if (application == nullptr)
			return false;

		// compute resource path
		boost::filesystem::path resources_path = application->GetResourcesPath();
		boost::filesystem::path levels_path = resources_path / "levels";

		// iterate the files and load the levels
		boost::filesystem::directory_iterator end;
		for (boost::filesystem::directory_iterator it(levels_path); it != end; ++it)
		{
			int level_index = chaos::StringTools::SkipAndAtoi(it->path().filename().string().c_str());

			// create the level
			death::GameLevel * level = DoLoadLevel(level_index, it->path());
			if (level == nullptr)
				continue;
			// initialize it
			level->level_index = level_index;
			// store it
			levels.push_back(level);
		}

		// sort the levels
		std::sort(levels.begin(), levels.end(),
			[](chaos::shared_ptr<GameLevel> l1, chaos::shared_ptr<GameLevel> l2)
		{
			return (l1->level_index < l2->level_index);
		});
		return true;
	}

#define DEATH_EMPTY_TOKEN
#define DEATH_FIND_RENDERABLE_CHILD(result, funcname, constness, param_type)\
	result constness * Game::funcname(param_type param, chaos::RenderableLayerSystem constness * root) constness\
	{\
		if (root == nullptr)\
		{\
			root = root_render_layer.get();\
			if (root == nullptr)\
				return nullptr;\
		}\
		return dynamic_cast<result constness*>(root->FindChildRenderable(param));\
	}
#define DEATH_FIND_RENDERABLE_CHILD_ALL(result, funcname)\
	DEATH_FIND_RENDERABLE_CHILD(result, funcname, DEATH_EMPTY_TOKEN, char const *);\
	DEATH_FIND_RENDERABLE_CHILD(result, funcname, DEATH_EMPTY_TOKEN, chaos::TagType);\
	DEATH_FIND_RENDERABLE_CHILD(result, funcname, const, char const *);\
	DEATH_FIND_RENDERABLE_CHILD(result, funcname, const, chaos::TagType);\

	DEATH_FIND_RENDERABLE_CHILD_ALL(chaos::RenderableLayerSystem, FindRenderableLayer);
	DEATH_FIND_RENDERABLE_CHILD_ALL(chaos::ParticleLayer, FindParticleLayer);

#undef DEATH_FIND_RENDERABLE_CHILD_ALL
#undef DEATH_FIND_RENDERABLE_CHILD
#undef DEATH_EMPTY_TOKEN

	chaos::RenderableLayerSystem * Game::AddChildRenderLayer(char const * layer_name, chaos::TagType layer_tag, int render_order)
	{
		chaos::RenderableLayerSystem * result = new chaos::RenderableLayerSystem();
		if (root_render_layer == nullptr)
			return result;
		result->SetName(layer_name);
		result->SetTag(layer_tag);
		root_render_layer->AddChildRenderable(result, render_order);
		return result;
	}

	bool Game::InitializeRootRenderLayer()
	{
		root_render_layer = new chaos::RenderableLayerSystem();
		if (root_render_layer == nullptr)
			return false;
		if (AddChildRenderLayer("GAME", death::GameHUDKeys::GAME_LAYER_ID, 1) == nullptr)
			return false;
		if (AddChildRenderLayer("PLAYER", death::GameHUDKeys::PLAYER_LAYER_ID, 2) == nullptr) // maybe the player will go in another layer
			return false;
		if (AddChildRenderLayer("HUD", death::GameHUDKeys::HUD_LAYER_ID, 3) == nullptr)
			return false;

		return true;
	}

	bool Game::InitializeParticleManager()
	{
		// create the manager
		particle_manager = new chaos::ParticleManager();
		if (particle_manager == nullptr)
			return false;
		particle_manager->SetTextureAtlas(texture_atlas.get());
		if (AddParticleLayers() < 0)
			return false;
		return true;
	}

	int Game::AddParticleLayers()
	{
		int render_order = 0;
		particle_manager->AddLayer<death::ParticleBackgroundTrait>(++render_order, death::GameHUDKeys::BACKGROUND_LAYER_ID, "background");

		return true;
	}

	bool Game::CreateBackgroundImage(char const * material_name, char const * texture_name)
	{
		if (material_name == nullptr)
			material_name = "background";

		// create the particle allocation if necessary
		if (background_allocations == nullptr)
		{
			background_allocations = GetGameParticleCreator().SpawnParticles(death::GameHUDKeys::BACKGROUND_LAYER_ID, 1);
			if (background_allocations == nullptr)
				return false;

			chaos::ParticleAccessor<death::ParticleBackground> particles = background_allocations->GetParticleAccessor<death::ParticleBackground>();
			if (particles.GetCount() == 0)
				return false;

			particles->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		}

		// create a material
		chaos::GPUResourceManager * resource_manager = chaos::MyGLFW::SingleWindowApplication::GetGPUResourceManagerInstance();
		if (resource_manager != nullptr)
		{
			// search declared material
			chaos::GPURenderMaterial * result = resource_manager->FindRenderMaterial(material_name);
			if (result != nullptr)
			{
				if (texture_name != nullptr)
				{
					// search the corresponding texture
					chaos::GPUTexture * texture = resource_manager->FindTexture(texture_name);
					if (texture == nullptr)
						return false;
					// create a child material
					chaos::GPURenderMaterial * child_material = new chaos::GPURenderMaterial();
					if (child_material == nullptr)
						return false;
					// initialize the material with parent ande texture
					child_material->SetParentMaterial(result);
					child_material->GetUniformProvider().AddVariableTexture("background", texture);
					result = child_material;
				}
				// assign the material to the background
				chaos::ParticleLayer * layer = particle_manager->FindLayer(death::GameHUDKeys::BACKGROUND_LAYER_ID);
				if (layer != nullptr)
					layer->SetRenderMaterial(result);
			}
		}

		return true;
	}

	bool Game::InitializeClocks()
	{
		chaos::Clock * application_clock = GetApplicationClock();
		if (application_clock == nullptr)
			return false;

		root_clock = application_clock->CreateChildClock("root_clock");
		if (root_clock == nullptr)
			return false;
		return true;
	}

	bool Game::InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path)
	{
		// initialize clocks
		if (!InitializeClocks())
			return false;
		// initialize the button map
		if (!InitializeGamepadButtonInfo())
			return false;
		// initialize game values
		if (!InitializeGameValues(config, config_path, false)) // false => not hot_reload
			return false;
		// load exisiting levels
		if (!LoadLevels())
			return false;
		// the atlas
		if (!GenerateAtlas(config, config_path))  // require to have loaded level first
			return false;
		// initialize the root render system
		if (!InitializeRootRenderLayer())
			return false;
		// initialize the particle manager
		if (!InitializeParticleManager())
			return false;
		// initialize the particle text generator manager
		if (!InitializeParticleTextGenerator())
			return false;
		// initialize game particles creator
		if (!InitializeGameParticleCreator())
			return false;
		// create the game background
		if (!CreateBackgroundImage(nullptr, nullptr))
			return false;

		// load the best score if any
		SerializeBestScore(false);
		return true;
	}

	chaos::MyGLFW::SingleWindowApplication * Game::GetApplication()
	{
		return chaos::MyGLFW::SingleWindowApplication::GetGLFWApplicationInstance();
	}

	chaos::MyGLFW::SingleWindowApplication const * Game::GetApplication() const
	{
		return chaos::MyGLFW::SingleWindowApplication::GetGLFWApplicationConstInstance();
	}

	chaos::SoundManager * Game::GetSoundManager()
	{
		chaos::MyGLFW::SingleWindowApplication * application = GetApplication();
		if (application == nullptr)
			return nullptr;
		return application->GetSoundManager();
	}

	chaos::Clock * Game::GetApplicationClock()
	{
		chaos::MyGLFW::SingleWindowApplication * application = GetApplication();
		if (application == nullptr)
			return nullptr;
		return application->GetMainClock();
	}

	chaos::Clock const * Game::GetApplicationClock() const
	{
		chaos::MyGLFW::SingleWindowApplication const * application = GetApplication();
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

	bool Game::LoadBestScore(std::ifstream & file)
	{
		file >> best_score;
		return true;
	}

	bool Game::SaveBestScore(std::ofstream & file)
	{
		file << best_score;
		return true;
	}

	void Game::ConditionnalSaveBestScore()
	{
		int player_best_score = GetBestPlayerScore();
		if (best_score < player_best_score)
		{
			best_score = player_best_score;
			SerializeBestScore(true);
		}
	}

	bool Game::SerializeBestScore(bool save)
	{
		// get application
		chaos::Application * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;
		// get user temp directory
		boost::filesystem::path filepath = application->GetUserLocalTempPath() / "best_score.txt";

		// save the score
		if (save)
		{
			std::ofstream file(filepath.string().c_str());
			if (!file)
				return false;
			return SaveBestScore(file);
		}
		// load the score
		else
		{
			std::ifstream file(filepath.string().c_str());
			if (!file)
				return false;
			return LoadBestScore(file);
		}
	}

	chaos::Sound * Game::PlaySound(char const * name, bool paused, bool looping)
	{
		chaos::SoundManager * sound_manager = GetSoundManager();
		if (sound_manager == nullptr)
			return nullptr;

		chaos::SoundSource * source = sound_manager->FindSource(name);
		if (source == nullptr)
			return nullptr;

		chaos::PlaySoundDesc play_desc;
		play_desc.paused = paused;
		play_desc.looping = looping;
		return source->PlaySound(play_desc);
	}

	void Game::BlendMusic(chaos::Sound * music, bool blend_in)
	{
		if (music == nullptr)
			return;

		chaos::BlendVolumeDesc blend_desc;
		blend_desc.blend_type = (blend_in) ?
			chaos::BlendVolumeDesc::BLEND_IN :
			chaos::BlendVolumeDesc::BLEND_OUT;
		blend_desc.blend_time = 0.4f,
			blend_desc.kill_at_end = false;
		blend_desc.pause_at_end = !blend_in;

		music->StartBlend(blend_desc, true, true);
	}

	void Game::ChangeMusic(chaos::Sound ** musics, size_t count, bool restart_first)
	{
		if (musics == nullptr || count == 0)
			return;

		// restore the main music
		chaos::Sound * music1 = musics[0];
		if (music1 != nullptr)
		{
			music1->Pause(false);
			if (restart_first)
				music1->SetSoundTrackPosition(0);
			BlendMusic(music1, true);
		}

		// pause all other musics
		for (size_t i = 1; i < count; ++i)
		{
			chaos::Sound * music = musics[i];
			if (music == nullptr)
				continue;
			BlendMusic(music, false);
		}
	}

	bool Game::CreateAllMusics()
	{
		if (menu_music == nullptr)
			menu_music = PlaySound("menu_music", true, true);
		if (pause_music == nullptr)
			pause_music = PlaySound("pause_music", true, true);
		if (game_music == nullptr)
			game_music = PlaySound("game_music", true, true);
		return true;
	}

	bool Game::DeclareParticleClasses()
	{
		// XXX : cannot register ParticleDefault, because if will be registered later as ParticleObject
		//       require refactor
		//chaos::ClassTools::DeclareClass<chaos::ParticleDefault::Particle>("ParticleDefault");
		chaos::ClassTools::DeclareClass<death::ParticleBackground>("ParticleBackground");
		return true;
	}

	bool Game::CreateGameStateMachine()
	{
		game_state_machine = DoCreateGameStateMachine();
		if (game_state_machine == nullptr)
			return false;
		if (!game_state_machine->InitializeStateMachine()) // create all internal states and transition
			return false;
		game_state_machine_instance = DoCreateGameStateMachineInstance(game_state_machine.get());
		if (game_state_machine_instance == nullptr)
			return false;
		return true;
	}

	chaos::SM::StateMachine * Game::DoCreateGameStateMachine()
	{
		return nullptr;
	}

	chaos::SM::StateMachineInstance * Game::DoCreateGameStateMachineInstance(chaos::SM::StateMachine * state_machine)
	{
		return new GameStateMachineInstance(this, state_machine);
	}

	bool Game::InitializeGame(GLFWwindow * in_glfw_window)
	{
		// initialize the window
		assert(in_glfw_window != nullptr);
		glfw_window = in_glfw_window;

		// initialize the gamepad manager
		gamepad_manager = new GamepadManager(this);
		if (gamepad_manager == nullptr)
			return false;

		// create game state_machine
		if (!CreateGameStateMachine())
			return false;

		// create the musics
		if (!CreateAllMusics())
			return false;

		// initialize particle classes
		if (!DeclareParticleClasses())
			return false;

		return true;
	}

	bool Game::OnGamepadInput(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad) // an uncatched gamepad input incomming
	{
		assert(in_physical_gamepad != nullptr && !in_physical_gamepad->IsAllocated());

		// maybe a start game
		if (in_physical_gamepad->IsAnyButtonPressed())
			if (RequireStartGame(in_physical_gamepad))
				return true;
		// maybe a player is interrested in capturing this device
		if (game_instance != nullptr)
			game_instance->GivePhysicalGamepadToPlayer(in_physical_gamepad); // single player mode
		return false;
	}

	bool Game::OnPhysicalGamepadInput(chaos::MyGLFW::PhysicalGamepad * physical_gamepad)
	{
		// ignore invalid gamepad : should never happen
		if (!physical_gamepad->IsAnyAction())
			return true;
		// change the application mode
		chaos::Application::SetApplicationInputMode(chaos::InputMode::Gamepad);
		// special action on gamepad input
		OnGamepadInput(physical_gamepad);

		return true;
	}

	bool Game::InitializeGamepadButtonInfo()
	{
		// the map [button ID] => [bitmap name + text generator alias]
#define DEATHGAME_ADD_BUTTONMAP(x, y) gamepad_button_map[chaos::MyGLFW::x] = std::pair<std::string, std::string>("xboxController" #y, #y)
		DEATHGAME_ADD_BUTTONMAP(XBOX_BUTTON_A, ButtonA);
		DEATHGAME_ADD_BUTTONMAP(XBOX_BUTTON_B, ButtonB);
		DEATHGAME_ADD_BUTTONMAP(XBOX_BUTTON_X, ButtonX);
		DEATHGAME_ADD_BUTTONMAP(XBOX_BUTTON_Y, ButtonY);
		DEATHGAME_ADD_BUTTONMAP(XBOX_BUTTON_LEFTBUT, LeftShoulder);
		DEATHGAME_ADD_BUTTONMAP(XBOX_BUTTON_RIGHTBUT, RightShoulder);
		DEATHGAME_ADD_BUTTONMAP(XBOX_BUTTON_LEFTTRIGGER, LeftTrigger);
		DEATHGAME_ADD_BUTTONMAP(XBOX_BUTTON_RIGHTTRIGGER, RightTrigger);
#undef LUDUMGAME_ADDTO_BUTTONMAP

		return true;
	}

	bool Game::InitializeParticleTextGenerator()
	{
		// create the generator
		particle_text_generator = new chaos::ParticleTextGenerator::Generator(*texture_atlas);
		if (particle_text_generator == nullptr)
			return false;

		// for each bitmap, that correspond to a button, register a [NAME] in the generator	
		chaos::BitmapAtlas::FolderInfo const * folder_info = texture_atlas->GetFolderInfo("sprites");
		if (folder_info != nullptr)
		{
			for (auto it = gamepad_button_map.begin(); it != gamepad_button_map.end(); ++it)
			{
				std::string const & bitmap_name = it->second.first;
				chaos::BitmapAtlas::BitmapInfo const * info = folder_info->GetBitmapInfo(bitmap_name.c_str());
				if (info == nullptr)
					continue;
				std::string const & generator_alias = it->second.second;
				particle_text_generator->AddBitmap(generator_alias.c_str(), info);
			}
		}

		return true;
	}

	bool Game::InitializeGameParticleCreator()
	{
		return particle_creator.Initialize(particle_manager.get(), particle_text_generator.get(), texture_atlas.get());
	}


	bool Game::InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path, bool hot_reload)
	{
		DEATHGAME_JSON_ATTRIBUTE(initial_life);
		DEATHGAME_JSON_ATTRIBUTE(max_life);
		DEATHGAME_JSON_ATTRIBUTE(mouse_sensitivity);
		DEATHGAME_JSON_ATTRIBUTE(gamepad_sensitivity);
		return true;
	}

	void Game::OnEnterMainMenu(bool very_first)
	{
		if (very_first)
			StartMainMenuMusic(true);
		CreateMainMenuHUD();
	}

	void Game::OnGameOver()
	{
		// internal code
		CreateGameOverHUD();
		// give opportunity to other game classes to respond
		if (current_level_instance != nullptr)
			current_level_instance->OnGameOver();
		if (game_instance != nullptr)
			game_instance->OnGameOver();		
	}

	bool Game::OnEnterPause()
	{
		// internal code
		StartPauseMusic(true);
		CreatePauseMenuHUD();
		// give opportunity to other game classes to respond
		if (current_level_instance != nullptr)
			current_level_instance->OnEnterPause();
		if (game_instance != nullptr)
			game_instance->OnEnterPause();

		return true;
	}

	bool Game::OnLeavePause()
	{
		// internal code
		StartGameMusic(false);
		CreatePlayingHUD();
		// give opportunity to other game classes to respond
		if (current_level_instance != nullptr)
			current_level_instance->OnLeavePause();
		if (game_instance != nullptr)
			game_instance->OnLeavePause();

		return true;
	}

	bool Game::OnEnterGame(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad)
	{
		// create the game instance
		game_instance = CreateGameInstance();
		if (game_instance == nullptr)
			return false;
		if (!game_instance->Initialize(this))
			return false;
		// create a player
		Player * first_player = game_instance->CreatePlayer(in_physical_gamepad);
		if (first_player == nullptr)
		{
			game_instance = nullptr; // destroy the game instance
			return false;
		}






		ResetGameVariables();
		CreatePlayingHUD();
		SetNextLevel(true); // select the very first
		StartGameMusic(true);

		return true;
	}

	bool Game::OnLeaveGame()
	{
		ConditionnalSaveBestScore();
		SetCurrentLevel(nullptr);	
		StartMainMenuMusic(true);
		game_instance = nullptr;
		return true;
	}

	bool Game::CheckGameOverCondition()
	{
		if (game_instance != nullptr)
		{
			// check for game over in game instance
			if (game_instance->CheckGameOverCondition())
				return true;
			// check level game over only if game is started. It could be a background level in main menu
			if (current_level_instance != nullptr)
				if (current_level_instance->CheckGameOverCondition()) 
					return true;
		}
		return false; // no gameover
	}


	bool Game::CheckLevelCompleted()
	{
		// cheat code
#if _DEBUG
		if (GetCheatSkipLevelRequired())
			return true;
#endif
		// game finished
		death::GameLevelInstance const * level_instance = GetCurrentLevelInstance();
		if (level_instance != nullptr)
			if (level_instance->IsLevelCompleted())
				return true;
		return false;
	}

	bool Game::CanCompleteLevel()
	{
		death::GameLevelInstance const * level_instance = GetCurrentLevelInstance();
		if (level_instance != nullptr)
			if (level_instance->CanCompleteLevel())
				return true;
		return false;
	}

	bool Game::TickGameLoop(double delta_time)
	{
		// game over ?
		if (CheckGameOverCondition())
		{
			RequireGameOver();
			return false;
		}
		// level finished
		if (CheckLevelCompleted())
		{
			if (CanCompleteLevel())
			{
				SetNextLevel(true);
				return false; // do not call remaining code in TickGameLoop(...) specialization
			}
		}
		// tick the level
		if (current_level_instance != nullptr)
			current_level_instance->Tick(delta_time);
		return true;
	}

	void Game::ResetGameVariables()
	{
		
	}

	void Game::StartMainMenuMusic(bool restart_first)
	{
		chaos::Sound * musics[] = {
			menu_music.get(),
			pause_music.get(),
			game_music.get()
		};
		ChangeMusic(musics, 3, restart_first);
	}

	void Game::StartGameMusic(bool restart_first)
	{
		chaos::Sound * musics[] = {
			game_music.get(),
			pause_music.get(),
			menu_music.get()
		};
		ChangeMusic(musics, 3, restart_first);
	}

	void Game::StartPauseMusic(bool restart_first)
	{
		chaos::Sound * musics[] = {
			pause_music.get(),
			menu_music.get(),
			game_music.get()
		};
		ChangeMusic(musics, 3, restart_first);
	}

	int Game::GetCurrentStateTag() const
	{
		if (game_state_machine_instance == nullptr)
			return -1;
		chaos::SM::StateBase const * current_state = game_state_machine_instance->GetCurrentState();
		if (current_state == nullptr)
			return -1;
		return current_state->GetTag();
	}

	bool Game::IsPlaying() const
	{
		if (GetCurrentStateTag() == GameStateMachineKeys::STATE_PLAYING)
			return true;
		return false;
	}

	bool Game::IsPaused() const
	{
		if (GetCurrentStateTag() == GameStateMachineKeys::STATE_PAUSE)
			return true;
		return false;
	}


	bool Game::RequireGameOver()
	{
		if (game_state_machine_instance->SendEvent(GameStateMachineKeys::EVENT_GAME_OVER, nullptr))
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
		if (game_state_machine_instance->SendEvent(GameStateMachineKeys::EVENT_TOGGLE_PAUSE, nullptr))
			return true;
		return false;
	}

	bool Game::RequireExitGame()
	{
		if (game_state_machine_instance->SendEvent(GameStateMachineKeys::EVENT_EXIT_GAME, nullptr))
			return true;
		return false;
	}

	bool Game::RequireStartGame(chaos::MyGLFW::PhysicalGamepad * physical_gamepad)
	{
		PhysicalGamepadWrapper game_pad_wrapper = PhysicalGamepadWrapper(physical_gamepad);

		if (game_state_machine_instance->SendEvent(GameStateMachineKeys::EVENT_START_GAME, &game_pad_wrapper))
			return true;
		return false;
	}

#define DEATH_IMPLEMENTHUD_FUNC(classname)\
	bool Game::Create##classname()\
	{\
		hud = DoCreate##classname();\
		if (hud == nullptr)\
			return false;\
		if (!hud->InitializeHUD())\
		{\
			hud = nullptr;\
			return false;\
		}\
		return true;\
	}\
	GameHUD * Game::DoCreate##classname()\
	{\
		return new classname(this);\
	}
	DEATH_IMPLEMENTHUD_FUNC(PauseMenuHUD);
	DEATH_IMPLEMENTHUD_FUNC(MainMenuHUD);
	DEATH_IMPLEMENTHUD_FUNC(PlayingHUD);
	DEATH_IMPLEMENTHUD_FUNC(GameOverHUD);

#undef DEATH_IMPLEMENTHUD_FUNC

	void Game::DestroyHUD()
	{
		hud = nullptr;
	}

	glm::vec2 Game::GetViewSize() const
	{
		glm::vec2 result;
		result.x = 1600.0f;
		result.y = result.x / viewport_wanted_aspect;
		return result;
	}

	chaos::box2 Game::GetViewBox() const
	{
		chaos::box2 result;
		result.position = glm::vec2(0.0f, 0.0f);
		result.half_size = GetViewSize() * 0.5f;
		return result;
	}

	chaos::box2 Game::GetWorldBox() const
	{
		// look at the level instance
		if (current_level_instance != nullptr)
		{
			chaos::box2 result = current_level_instance->GetBoundingBox();
			if (!result.IsEmpty())
				return result;
		}
		// by default, the world will be the same size than the screen
		return GetViewBox();
	}

	GameLevel * Game::GetCurrentLevel()
	{
		GameLevelInstance * li = GetCurrentLevelInstance();
		if (li == nullptr)
			return nullptr;
		return li->GetLevel();
	}

	GameLevel const * Game::GetCurrentLevel() const
	{
		GameLevelInstance const * li = GetCurrentLevelInstance();
		if (li == nullptr)
			return nullptr;
		return li->GetLevel();
	}

	GameLevelInstance * Game::GetCurrentLevelInstance()
	{
		return current_level_instance.get();
	}

	GameLevelInstance const * Game::GetCurrentLevelInstance() const
	{
		return current_level_instance.get();
	}

	GameLevel * Game::GetLevel(int level_index)
	{
		size_t count = levels.size();
		for (size_t i = 0; i < count; ++i)
			if (levels[i]->GetLevelIndex() == level_index)
				return levels[i].get();
		return nullptr;
	}

	GameLevel const * Game::GetLevel(int level_index) const
	{
		size_t count = levels.size();
		for (size_t i = 0; i < count; ++i)
			if (levels[i]->GetLevelIndex() == level_index)
				return levels[i].get();
		return nullptr;
	}

	bool Game::SetNextLevel(bool looping_levels)
	{
#if _DEBUG
		SetCheatSkipLevelRequired(false);
#endif

		// existing any level
		size_t count = levels.size();
		if (count == 0)
			return false;
		// very first level
		GameLevel * current_level = GetCurrentLevel();
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

	bool Game::SetCurrentLevel(int level_index)
	{
		GameLevel * new_level = GetLevel(level_index); // we required a level_index, level should not be nullptr !
		if (new_level == nullptr)
			return false;
		return SetCurrentLevel(new_level);
	}

	bool Game::SetCurrentLevel(GameLevel * new_level) // new_level can be set to nullptr, just to clear every thing
	{
		// copy and keep a reference
		chaos::shared_ptr<GameLevelInstance> old_level_instance = current_level_instance; 
		chaos::shared_ptr<GameLevel> old_level = (old_level_instance != nullptr) ?
			old_level_instance->GetLevel() :
			nullptr;

		// create the new level instance if required
		chaos::shared_ptr<GameLevelInstance> new_level_instance;
		if (new_level != nullptr)
		{
			new_level_instance = new_level->CreateLevelInstance(this);
			if (new_level_instance == nullptr)
				return false;
		}

		// change the level
		current_level_instance = new_level_instance;
		OnLevelChanged(new_level, old_level.get(), current_level_instance.get(), old_level_instance.get());
		return true;
	}

	void Game::OnLevelChanged(GameLevel * new_level, GameLevel * old_level, GameLevelInstance * new_level_instance, GameLevelInstance * old_level_instance)
	{
		// leave previous level
		if (old_level_instance != nullptr)
			old_level_instance->OnLevelEnded();
		// start new level. Create a new level clock
		if (new_level_instance != nullptr)
			new_level_instance->OnLevelStarted();
	}

	Player * Game::GetPlayer(int player_index)
	{
		// game even not started : no player
		if (game_instance == nullptr)
			return nullptr;
		// give the instance the responsability 
		return game_instance->GetPlayer(player_index);
	}

	Player const * Game::GetPlayer(int player_index) const
	{
		// game even not started : no player
		if (game_instance == nullptr)
			return nullptr;
		// give the instance the responsability 
		return game_instance->GetPlayer(player_index);
	}








	bool Game::ReloadConfigurationFile()
	{
		chaos::Application * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		nlohmann::json config;
		if (!application->ReloadConfigurationFile(config))
			return false;

		nlohmann::json const * game_config = chaos::JSONTools::GetStructure(config, "game");
		if (game_config == nullptr)
			return false;

		return InitializeGameValues(*game_config, application->GetConfigurationPath(), true); // true => hot_reload
	}

	GameInstance * Game::CreateGameInstance()
	{
		return new GameInstance(this);
	}

}; // namespace death

