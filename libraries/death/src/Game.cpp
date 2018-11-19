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
	void Game::OnInputModeChanged(int new_mode, int old_mode)
	{

	}

	void Game::SetCheatSkipLevelRequired(bool value)
	{
		cheat_skip_level_required = value;
	}

	bool Game::GetCheatSkipLevelRequired() const
	{
		return cheat_skip_level_required;
	}

	void Game::HandleKeyboardInputs()
	{
		// test whether the stick position can be overriden
		glm::vec2 simulated_stick = glm::vec2(0.0f, 0.0f);

		if (glfwGetKey(glfw_window, GLFW_KEY_LEFT))
			simulated_stick.x -= 1.0f;
		if (glfwGetKey(glfw_window, GLFW_KEY_RIGHT))
			simulated_stick.x += 1.0f;

		if (glfwGetKey(glfw_window, GLFW_KEY_DOWN))
			simulated_stick.y += 1.0f;
		if (glfwGetKey(glfw_window, GLFW_KEY_UP))
			simulated_stick.y -= 1.0f;

		if (glm::length2(simulated_stick) > 0)
			left_stick_position = gamepad_sensitivity * simulated_stick;
	}

	void Game::ResetPlayerCachedInputs()
	{
		left_stick_position = glm::vec2(0.0f, 0.0f);
		right_stick_position = glm::vec2(0.0f, 0.0f);
	}
	
	void Game::Tick(double delta_time)
	{
		// catch all stick inputs
		if (gamepad_manager != nullptr)
			gamepad_manager->Tick((float)delta_time);
		// handle keyboard inputs
		HandleKeyboardInputs();
		// update the game state_machine
		if (game_state_machine_instance != nullptr)
			game_state_machine_instance->Tick(delta_time);
		// clear the cached inputs
		ResetPlayerCachedInputs();
		// tick the particle manager
		if (particle_manager != nullptr)
			particle_manager->Tick((float)delta_time);
		// tick the hud
		if (hud != nullptr)
			hud->Tick(delta_time);
	}
	
	bool Game::OnKeyEvent(int key, int action)
	{
		// MAIN MENU to PLAYING
		if (action == GLFW_PRESS)
			if (RequireStartGame())
				return true;

		// PLAYING to PAUSE
		if ((key == GLFW_KEY_KP_ENTER || key == GLFW_KEY_ENTER) && action == GLFW_PRESS)
			if (RequireTogglePause())
				return true;

		// QUIT GAME
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			if (RequireExitGame())
				return true;

		// CHEAT CODE TO SKIP LEVEL
#if _DEBUG
		if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
			SetCheatSkipLevelRequired(true);
#endif

		return false;
	}
	
	bool Game::OnCharEvent(unsigned int c)
	{
		return true;
	}
	
	void Game::OnMouseButton(int button, int action, int modifier)
	{
		if (action == GLFW_PRESS)
			RequireStartGame();
	}
	
	void Game::OnMouseMove(double x, double y)
	{

	}

	void Game::AddBoxVariable(chaos::GPUProgramProvider & uniform_provider, char const * variable_name, chaos::box2 const & b)
	{
		glm::vec4 box_vec;
		box_vec.x = b.position.x;
		box_vec.y = b.position.y;
		box_vec.z = b.half_size.x;
		box_vec.w = b.half_size.y;
		uniform_provider.AddVariableValue(variable_name, box_vec);
	}
	
	void Game::Display(glm::ivec2 const & size)
	{
		chaos::box2 viewport = chaos::GLTools::SetViewportWithAspect(size, viewport_wanted_aspect);

		// keep camera, player inside the world (can be done at rendering time)
		RestrictCameraToPlayerAndWorld();

		// a variable provider
		chaos::GPUProgramProvider main_uniform_provider;

		// the viewport
		main_uniform_provider.AddVariableValue("viewport_size", viewport.half_size * 2.0f);

		// the related box
		glm::vec2 view_size = GetViewSize();
		main_uniform_provider.AddVariableValue("view_size", view_size);

		// boxes
		chaos::box2 player = GetPlayerBox();
		AddBoxVariable(main_uniform_provider, "player_box", player);
		chaos::box2 camera = GetCameraBox();
		AddBoxVariable(main_uniform_provider, "camera_box", camera);
		chaos::box2 world  = GetWorldBox();
		AddBoxVariable(main_uniform_provider, "world_box", world);
		chaos::box2 view   = GetViewBox();
		AddBoxVariable(main_uniform_provider, "view_box", world);
			 
		// the time
		double root_time = GetRootClockTime();
		main_uniform_provider.AddVariableValue("root_time", root_time);
		double main_time = GetMainClockTime();
		main_uniform_provider.AddVariableValue("main_time", main_time);
		double game_time = GetMainClockTime();
		main_uniform_provider.AddVariableValue("game_time", game_time);
		double pause_time = GetMainClockTime();
		main_uniform_provider.AddVariableValue("pause_time", pause_time);

		chaos::RenderParams render_params;
		render_params.viewport = viewport;
		DoDisplay(render_params, main_uniform_provider);
	}

	void Game::DoDisplay(chaos::RenderParams const & render_params, chaos::GPUProgramProvider & uniform_provider)
	{
		// display the level instance
		if (current_level_instance != nullptr)
			current_level_instance->Display(&uniform_provider, render_params);
		// display the hud (AFTER the level)
		if (hud != nullptr)
			hud->Display(&uniform_provider, render_params);
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
			[](boost::intrusive_ptr<GameLevel> l1, boost::intrusive_ptr<GameLevel> l2)
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
		return true;
	}

	bool Game::CreateBackgroundImage()
	{
		return true;
	}
	
	bool Game::DestroyInGameClocks()
	{
		if (main_clock != nullptr)
		{
			main_clock->RemoveFromParent();
			main_clock = nullptr;
		}

		if (game_clock != nullptr)
		{
			game_clock->RemoveFromParent();
			game_clock = nullptr;
		}

		if (pause_clock != nullptr)
		{
			pause_clock->RemoveFromParent();
			pause_clock = nullptr;
		}
		return true;
	}

	void Game::PauseInGameClocks(bool pause)
	{
		if (main_clock != nullptr)
			main_clock->SetPause(pause);

		if (game_clock != nullptr)
			game_clock->SetPause(pause);

		if (pause_clock != nullptr)
			pause_clock->SetPause(pause);
	}

	bool Game::CreateInGameClocks()
	{
		if (root_clock == nullptr)
			return false;	

		main_clock = root_clock->CreateChildClock("main_clock"); 
		if (main_clock == nullptr)
			return false;

		game_clock = root_clock->CreateChildClock("game_clock"); 
		if (game_clock == nullptr)
			return false;

		chaos::ClockCreateParams pause_clock_params;
		pause_clock_params.paused = true;
		pause_clock = root_clock->CreateChildClock("pause_clock", pause_clock_params); // start paused ...
		if (pause_clock == nullptr)
			return false;

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
		if (!InitializeGameValues(config, config_path))
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
		if (!CreateBackgroundImage())
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

#define CHAOS_EMPTY_TOKEN
#define CHAOS_IMPL_GET_CLOCK(func_name, member_name, constess)\
	chaos::Clock constess * Game::func_name() constess\
	{\
		return member_name.get();\
	}
	CHAOS_IMPL_GET_CLOCK(GetRootClock, root_clock, CHAOS_EMPTY_TOKEN);
	CHAOS_IMPL_GET_CLOCK(GetRootClock, root_clock, const);
	CHAOS_IMPL_GET_CLOCK(GetMainClock, main_clock, CHAOS_EMPTY_TOKEN);
	CHAOS_IMPL_GET_CLOCK(GetMainClock, main_clock, const);
	CHAOS_IMPL_GET_CLOCK(GetGameClock, game_clock, CHAOS_EMPTY_TOKEN);
	CHAOS_IMPL_GET_CLOCK(GetGameClock, game_clock, const);
	CHAOS_IMPL_GET_CLOCK(GetPauseClock, pause_clock, CHAOS_EMPTY_TOKEN);
	CHAOS_IMPL_GET_CLOCK(GetPauseClock, pause_clock, const);
#undef CHAOS_IMPL_GET_CLOCK

#define CHAOS_IMPL_GET_CLOCK_TIME(func_name, clock_func_name)\
	double Game::func_name() const\
	{\
		chaos::Clock const * clock = clock_func_name();\
		if (clock == nullptr)\
			return 0.0;\
		return clock->GetClockTime();\
	}
	CHAOS_IMPL_GET_CLOCK_TIME(GetRootClockTime, GetRootClock);
	CHAOS_IMPL_GET_CLOCK_TIME(GetMainClockTime, GetMainClock);
	CHAOS_IMPL_GET_CLOCK_TIME(GetGameClockTime, GetGameClock);
	CHAOS_IMPL_GET_CLOCK_TIME(GetPauseClockTime, GetPauseClock);
#undef CHAOS_IMPL_GET_CLOCK_TIME
#undef CHAOS_EMPTY_TOKEN

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
		if (best_score < current_score)
		{
			best_score = current_score;
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

	void Game::HandleGamepadInput(chaos::MyGLFW::GamepadData & in_gamepad_data)
	{
		// cache the stick position
		glm::vec2 lsp = in_gamepad_data.GetXBOXStickDirection(chaos::MyGLFW::XBOX_LEFT_AXIS);
		if (glm::length2(lsp) > 0.0f)
			left_stick_position = gamepad_sensitivity * lsp;
		else
		{
			if (in_gamepad_data.IsButtonPressed(chaos::MyGLFW::XBOX_BUTTON_LEFT))
				left_stick_position.x = -gamepad_sensitivity * 1.0f;
			else if (in_gamepad_data.IsButtonPressed(chaos::MyGLFW::XBOX_BUTTON_RIGHT))
				left_stick_position.x = gamepad_sensitivity * 1.0f;

			if (in_gamepad_data.IsButtonPressed(chaos::MyGLFW::XBOX_BUTTON_UP))
				left_stick_position.y = -gamepad_sensitivity * 1.0f;
			else if (in_gamepad_data.IsButtonPressed(chaos::MyGLFW::XBOX_BUTTON_DOWN))
				left_stick_position.y = gamepad_sensitivity * 1.0f;
		}

		glm::vec2 rsp = in_gamepad_data.GetXBOXStickDirection(chaos::MyGLFW::XBOX_RIGHT_AXIS);
		if (glm::length2(rsp) > 0.0f)
			right_stick_position = gamepad_sensitivity * rsp;
	}

	bool Game::OnGamepadInput(chaos::MyGLFW::GamepadData & in_gamepad_data)
	{
		// maybe a start game
		if (in_gamepad_data.IsAnyButtonPressed())
			if (RequireStartGame())
				return true;

		// maybe a game/pause resume
		if (
			(in_gamepad_data.GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_SELECT) == chaos::MyGLFW::BUTTON_BECOME_PRESSED) ||
			(in_gamepad_data.GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_START) == chaos::MyGLFW::BUTTON_BECOME_PRESSED))
		{
			if (RequireTogglePause())
				return true;
		}
		return false;
	}

	bool Game::OnPhysicalGamepadInput(chaos::MyGLFW::PhysicalGamepad * physical_gamepad)
	{
		// ignore invalid gamepad : should never happen
		if (!physical_gamepad->IsAnyAction())
			return true;

		// change the application mode
		chaos::Application::SetApplicationInputMode(chaos::InputMode::Gamepad);

		// copy the gamepad information into the game and handle it
		gamepad_data = physical_gamepad->GetGamepadData();

		HandleGamepadInput(gamepad_data);

		// special action on gamepad input
		OnGamepadInput(gamepad_data);

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


	bool Game::InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path)
	{
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
		SetCurrentLevel(nullptr);
		ConditionnalSaveBestScore();		
	}
	void Game::OnPauseStateUpdateClocks(bool enter_pause)
	{
		if (pause_clock != nullptr)
		{
			pause_clock->SetPause(!enter_pause);
			pause_clock->Reset();
		}
		if (game_clock != nullptr)
			game_clock->SetPause(enter_pause);
	}

	bool Game::OnEnterPause()
	{
		StartPauseMusic(true);
		CreatePauseMenuHUD();
		OnPauseStateUpdateClocks(true);
		return true;
	}

	bool Game::OnLeavePause()
	{
		StartGameMusic(false);
		CreatePlayingHUD();
		OnPauseStateUpdateClocks(false);
		return true;
	}

	bool Game::OnEnterGame()
	{		
		CreatePlayingHUD();
		ResetGameVariables();
		SetNextLevel(true); // select the very first
		StartGameMusic(true);
		CreateInGameClocks();
		return true;
	}

	bool Game::OnLeaveGame(bool gameover)
	{
		StartMainMenuMusic(true);
		if (gameover)
			CreateGameOverHUD();
		PauseInGameClocks(true); // clocks will be destroyed when EnterMainMenu
		return true;
	}

	bool Game::OnAbordGame()
	{
		SetCurrentLevel(nullptr);
		return true;
	}

	bool Game::IsPauseEnterComplete()
	{
		if (pause_music == nullptr)
			return true;
		return !pause_music->HasVolumeBlending();
	}

	bool Game::IsPauseLeaveComplete()
	{
		if (game_music == nullptr)
			return true;
		return !game_music->HasVolumeBlending();
	}

	bool Game::IsGameEnterComplete()
	{
		if (game_music == nullptr)
			return true;
		return !game_music->HasVolumeBlending();
	}

	bool Game::IsGameLeaveComplete()
	{
		if (menu_music == nullptr)
			return true;
		return !menu_music->HasVolumeBlending();
	}

	bool Game::CheckGameOverCondition()
	{
		return false; // no game hover
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
			return false;
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
		current_score = 0;
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

	bool Game::RequireStartGame()
	{
		if (game_state_machine_instance->SendEvent(GameStateMachineKeys::EVENT_START_GAME, nullptr))
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
	classname * Game::DoCreate##classname()\
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
			chaos::box2 result = current_level_instance->GetWorldBox();
			if (!result.IsEmpty())
				return result;
		}
		// by default, the world will be the same size than the screen
		return GetViewBox();
	}

	chaos::box2 Game::GetCameraBox() const
	{
		// default initialization of the camera if required
		if (camera_box.IsEmpty())
			camera_box = GetViewBox();
		// stored value
		return camera_box;
	}

	void Game::SetCameraBox(chaos::box2 const & in_camera_box)
	{
		camera_box = in_camera_box;
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
		SetCheatSkipLevelRequired(false); 

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
		boost::intrusive_ptr<GameLevelInstance> old_level_instance = current_level_instance; // copy and keep a reference
		boost::intrusive_ptr<GameLevel> old_level = (old_level_instance != nullptr) ?
			old_level_instance->GetLevel() :
			nullptr;

		// clear the level
		if (new_level == nullptr)
		{
			current_level_instance = nullptr;
			OnLevelChanged(nullptr, old_level.get(), nullptr, old_level_instance.get());
			return true;
		}
		// change the level
		boost::intrusive_ptr<GameLevelInstance> new_level_instance = new_level->CreateLevelInstance(this);
		if (new_level_instance == nullptr)
			return false;
		current_level_instance = new_level_instance;
		OnLevelChanged(new_level, old_level.get(), current_level_instance.get(), old_level_instance.get());
		return true;
	}

	void Game::OnLevelChanged(GameLevel * new_level, GameLevel * old_level, GameLevelInstance * new_level_instance, GameLevelInstance * old_level_instance)
	{
		if (old_level_instance != nullptr)
			old_level_instance->OnLevelEnded();
		if (new_level_instance != nullptr)
			new_level_instance->OnLevelStarted();
	}

	void Game::RestrictCameraToPlayerAndWorld()
	{
		// get camera, cannot continue if it is empty
		chaos::box2 camera = GetCameraBox();
		if (camera.IsEmpty())
			return;

		// keep player inside camera safe zone
		chaos::box2 player = GetPlayerBox();
		if (!player.IsEmpty())
		{
			chaos::box2 safe_camera = camera;
			safe_camera.half_size *= camera_safe_zone;

			if (chaos::RestrictToInside(safe_camera, player, true)) // apply the safe_zone displacement to the real camera
				camera.position = safe_camera.position;
		}
			
		// try to keep the camera in the world
		chaos::box2 world = GetWorldBox();
		if (!world.IsEmpty())
			chaos::RestrictToInside(world, camera, false);

		// apply camera changes
		SetCameraBox(camera);
	}

	chaos::ParticleDefault::Particle * Game::GetObjectParticle(chaos::ParticleAllocation * allocation, size_t index)
	{
		if (allocation == nullptr)
			return nullptr;
		if (index >= allocation->GetParticleCount())
			return nullptr;

		chaos::ParticleAccessor<chaos::ParticleDefault::Particle> particles = allocation->GetParticleAccessor<chaos::ParticleDefault::Particle>();
		if (particles.GetCount() == 0)
			return nullptr;
		return &particles[index];
	}

	chaos::ParticleDefault::Particle const * Game::GetObjectParticle(chaos::ParticleAllocation const * allocation, size_t index) const
	{
		if (allocation == nullptr)
			return nullptr;
		if (index >= allocation->GetParticleCount())
			return nullptr;

		chaos::ParticleConstAccessor<chaos::ParticleDefault::Particle> particles = allocation->GetParticleAccessor<chaos::ParticleDefault::Particle>();
		if (particles.GetCount() == 0)
			return nullptr;
		return &particles[index];
	}

	glm::vec2 Game::GetObjectPosition(chaos::ParticleAllocation const * allocation, size_t index) const
	{
		return GetObjectBox(allocation, index).position;
	}

	chaos::box2 Game::GetObjectBox(chaos::ParticleAllocation const * allocation, size_t index) const
	{
		chaos::ParticleDefault::Particle const * object = GetObjectParticle(allocation, index);
		if (object == nullptr)
			return chaos::box2();
		return object->bounding_box;
	}

	bool Game::SetObjectPosition(chaos::ParticleAllocation * allocation, size_t index, glm::vec2 const & position)
	{
		chaos::ParticleDefault::Particle * particle = GetObjectParticle(allocation, index);
		if (particle == nullptr)
			return false;
		particle->bounding_box.position = position;
		return true;
	}

	bool Game::SetObjectBox(chaos::ParticleAllocation * allocation, size_t index, chaos::box2 const & box)
	{
		chaos::ParticleDefault::Particle * object = GetObjectParticle(allocation, index);
		if (object == nullptr)
			return false;
		object->bounding_box = box;
		return true;
	}

	chaos::ParticleDefault::Particle * Game::GetPlayerParticle()
	{
		return GetObjectParticle(GetPlayerAllocation(), 0);
	}

	chaos::ParticleDefault::Particle const * Game::GetPlayerParticle() const
	{
		return GetObjectParticle(GetPlayerAllocation(), 0);
	}

	glm::vec2 Game::GetPlayerPosition() const
	{
		return GetObjectPosition(GetPlayerAllocation(), 0);
	}

	chaos::box2 Game::GetPlayerBox() const
	{
		return GetObjectBox(GetPlayerAllocation(), 0);
	}

	bool Game::SetPlayerPosition(glm::vec2 const & position)
	{
		return SetObjectPosition(GetPlayerAllocation(), 0, position);
	}
	bool Game::SetPlayerBox(chaos::box2 const & box)
	{
		return SetObjectBox(GetPlayerAllocation(), 0, box);
	}


}; // namespace death

