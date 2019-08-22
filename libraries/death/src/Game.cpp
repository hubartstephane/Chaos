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
		if (chaos::Application::HasApplicationCommandLineFlag("-CheatMode"))
			return true;
		return cheat_mode;
	}
#endif // _DEBUG

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

	bool Game::OnKeyEvent(int key, int scan_code, int action, int modifier)
	{
		// give the game instance opportunity to capture the input
		if (game_instance != nullptr)
			if (game_instance->OnKeyEvent(key, scan_code, action, modifier))
				return true;

		// only care for keys that are PRESSED (ignore RELEASE, ignore REPEAT)
		if (action == GLFW_PRESS)
		{
			// MAIN MENU to PLAYING
			if (key != GLFW_KEY_ESCAPE && key != GLFW_KEY_LEFT_SHIFT && key != GLFW_KEY_RIGHT_SHIFT)
				if (RequireStartGame(nullptr))
					return true;
			// PLAYING to PAUSE
			if (key == GLFW_KEY_KP_ENTER || key == GLFW_KEY_ENTER)
				if (RequireTogglePause())
					return true;
			// QUIT GAME
			if (key == GLFW_KEY_ESCAPE)
			{		
				if (modifier & GLFW_MOD_SHIFT)
				{
					if (RequireExitGame())
						return true;
				}
				else
				{
					if (RequireTogglePause())
						return true;
				}
			}
			// CHEAT CODE TO SKIP LEVEL
#if _DEBUG
			if (key == GLFW_KEY_F1)
			{
				SetCheatSkipLevelRequired(true);
				return true;
			}
			if (key == GLFW_KEY_F2)
			{
				SetCheatMode(!GetCheatMode());
				return true;
			}
			if (key == GLFW_KEY_F3)
			{
				ReloadGameConfiguration();
				return true;
			}
			if (key == GLFW_KEY_F4)
			{
				ReloadGPUResources();
				return true;
			}
			if (key == GLFW_KEY_F5)
			{
				ReloadCurrentLevel();
				return true;
			}
#endif
		}
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

	chaos::box2 Game::GetRequiredViewport(glm::ivec2 const & size) const
	{
		chaos::box2 viewport = chaos::box2(std::make_pair(
			glm::vec2(0.0f, 0.0f),
			chaos::GLMTools::RecastVector<glm::vec2>(size)
		));
		return ShrinkBoxToAspect(viewport, viewport_wanted_aspect);
	}

	void Game::Display(chaos::GPURenderer * renderer, glm::ivec2 const & size)
	{
		chaos::box2 viewport = chaos::GLTools::SetViewportWithAspect(size, viewport_wanted_aspect);

		// a variable provider
		chaos::GPUProgramProvider main_uniform_provider;
		FillUniformProvider(main_uniform_provider);
		// the window size
		main_uniform_provider.AddVariableValue("window_size", chaos::GLMTools::RecastVector<glm::vec2>(size));
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

		// rendering
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

	void Game::DoDisplay(chaos::GPURenderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::RenderParams const & render_params)
	{
		// clear the main render target
		DoPreDisplay(renderer, uniform_provider, render_params);
		// display the level instance
		DoDisplayGame(renderer, uniform_provider, render_params);
		// display the hud (AFTER the level)
		DoDisplayHUD(renderer, uniform_provider, render_params);
	}


	void Game::DoPreDisplay(chaos::GPURenderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::RenderParams const & render_params)
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

	void Game::DoDisplayGame(chaos::GPURenderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::RenderParams const & render_params)
	{		

		// shuwww   root_render_layer ??










		if (particle_manager != nullptr)
			particle_manager->Display(renderer, uniform_provider, render_params);
		if (current_level_instance != nullptr)
			current_level_instance->Display(renderer, uniform_provider, render_params);
	}

	void Game::DoDisplayHUD(chaos::GPURenderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::RenderParams const & render_params)
	{	
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
			// find or create folder
			chaos::BitmapAtlas::FolderInfoInput * folder_input = input.AddFolder("sprites", 0);
			if (folder_input == nullptr)
				return false;
			// add sprites from TiledMap
			if (!chaos::TiledMapTools::AddIntoAtlasInput(tiled_map_manager.get(), folder_input))
				return false;
		}
		return true;
	}

	bool Game::FillAtlasGenerationInputSprites(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path)
	{
		// get the directory where the sprites are
		std::string sprite_directory;
		if (!chaos::JSONTools::GetAttribute(config, "sprite_directory", sprite_directory))
			return true;
		// find or create folder
		chaos::BitmapAtlas::FolderInfoInput * folder_info = input.AddFolder("sprites", 0);
		if (folder_info == nullptr)
			return false;
		// Add sprites
		folder_info->AddBitmapFilesFromDirectory(sprite_directory, true);

		return true;
	}

	bool Game::FillAtlasGenerationInputFonts(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path)
	{
		nlohmann::json const * fonts_config = chaos::JSONTools::GetStructure(config, "fonts");
		if (fonts_config != nullptr)
		{
			nlohmann::json const * fonts_json = chaos::JSONTools::GetStructure(*fonts_config, "fonts");
			if (fonts_json != nullptr && fonts_json->is_object())
			{
				for (nlohmann::json::const_iterator it = fonts_json->begin(); it != fonts_json->end(); ++it)
				{
					if (!it->is_string())
						continue;
					// read information
					std::string font_name = it.key();
					std::string font_path = it->get<std::string>();
					// Add the font
					chaos::BitmapAtlas::FontInfoInputParams font_params;
					font_params.max_character_width = 64;
					font_params.max_character_height = 64;

					if (input.AddFont(font_path.c_str(), nullptr, true, font_name.c_str(), 0, font_params) == nullptr)
						return false;
				}			
			}		
		}

		return true;
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

	death::GameLevel * Game::DoLoadLevel(chaos::FilePathParam const & path)
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
			chaos::TiledMap::Map * tiled_map = tiled_map_manager->LoadMap(path, false); // false : the map is not kept in the manager
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

	boost::filesystem::directory_iterator Game::GetResourceDirectoryIteratorFromConfig(nlohmann::json const & config, char const * config_name, char const * default_path) const
	{
		// read in the config file the whole path
		std::string directory;
		if (chaos::JSONTools::GetAttribute(config, config_name, directory))
			return chaos::FileTools::GetDirectoryIterator(directory);
		// concat the argument path with 'resources'
		if (default_path != nullptr)
		{
			// get the application
			chaos::MyGLFW::SingleWindowApplication * application = chaos::MyGLFW::SingleWindowApplication::GetGLFWApplicationInstance();
			if (application != nullptr)
			{
				// compute resource path
				boost::filesystem::path resources_path = application->GetResourcesPath();
				boost::filesystem::path result_path = resources_path / default_path;

				return chaos::FileTools::GetDirectoryIterator(result_path);
			}
		}
		return boost::filesystem::directory_iterator();
	}

	bool Game::GenerateTileSets(nlohmann::json const & config)
	{
		// iterate the files and load the tilesets
		boost::filesystem::directory_iterator end;
		for (boost::filesystem::directory_iterator it = GetResourceDirectoryIteratorFromConfig(config, "tilesets_directory", "tilesets"); it != end; ++it)
		{
			boost::filesystem::path p = it->path();

			if (chaos::FileTools::IsTypedFile(p, "tsx"))
			{
				// create the tiledmap manager if necessary
				if (tiled_map_manager == nullptr)
				{
					tiled_map_manager = new chaos::TiledMap::Manager;
					if (tiled_map_manager == nullptr)
						return false;
				}
				// load the tileset
				if (!tiled_map_manager->LoadTileSet(p))
					return false;
			}
		}
		return true;
	}

	bool Game::LoadLevels(nlohmann::json const & config)
	{
		// iterate the files and load the levels
		boost::filesystem::directory_iterator end;
		for (boost::filesystem::directory_iterator it = GetResourceDirectoryIteratorFromConfig(config, "levels_directory", "levels"); it != end; ++it)
		{
			int level_index = chaos::StringTools::SkipAndAtoi(it->path().filename().string().c_str());

			// create the level
			death::GameLevel * level = DoLoadLevel(it->path());
			if (level == nullptr)
				continue;
			// initialize it
			level->SetPath(it->path());
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

#define DEATH_FIND_RENDERABLE_CHILD(result, funcname, constness, param_type)\
	result constness * Game::funcname(param_type param, chaos::RenderableLayerSystem constness * root) constness\
	{\
		if (root == nullptr)\
		{\
			root = root_render_layer.get();\
			if (root == nullptr)\
				return nullptr;\
		}\
		return auto_cast(root->FindChildRenderable(param));\
	}
#define DEATH_FIND_RENDERABLE_CHILD_ALL(result, funcname)\
	DEATH_FIND_RENDERABLE_CHILD(result, funcname, BOOST_PP_EMPTY(), char const *);\
	DEATH_FIND_RENDERABLE_CHILD(result, funcname, BOOST_PP_EMPTY(), chaos::TagType);\
	DEATH_FIND_RENDERABLE_CHILD(result, funcname, const, char const *);\
	DEATH_FIND_RENDERABLE_CHILD(result, funcname, const, chaos::TagType);\

	DEATH_FIND_RENDERABLE_CHILD_ALL(chaos::RenderableLayerSystem, FindRenderableLayer);
	DEATH_FIND_RENDERABLE_CHILD_ALL(chaos::ParticleLayerBase, FindParticleLayer);

#undef DEATH_FIND_RENDERABLE_CHILD_ALL
#undef DEATH_FIND_RENDERABLE_CHILD

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



		// shuwww



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

		// shuwww on pourrait faire CreateParticleManager(...) appelable depuis l exterieur


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
		particle_manager->AddLayer<death::ParticleBackgroundTrait>(render_order++, death::GameHUDKeys::BACKGROUND_LAYER_ID, "background"); // shuwww est ce que ca doit etre dans le particle_manager
		return render_order;
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
				chaos::ParticleLayerBase * layer = particle_manager->FindLayer(death::GameHUDKeys::BACKGROUND_LAYER_ID);
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
		// loading tilemapset
		if (!GenerateTileSets(config))
			return false;
		// the atlas
		if (!GenerateAtlas(config, config_path))  // require to have loaded level first
			return false;
		// load exisiting levels
		if (!LoadLevels(config))
			return false;
		// initialize the root render system
		if (!InitializeRootRenderLayer())
			return false;
		// initialize the particle manager
		if (!InitializeParticleManager())
			return false;
		// initialize the particle text generator manager
		if (!InitializeParticleTextGenerator(config, config_path))
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

	chaos::Sound * Game::SetInGameMusic(char const * music_name)
	{
		assert(music_name != nullptr);
		
		// ensure there is a real music change
		if (game_music != nullptr && !game_music->IsPendingKill())
		{
			chaos::SoundManager * sound_manager = GetSoundManager();
			if (sound_manager == nullptr)
				return nullptr;

			chaos::SoundSource * new_source = sound_manager->FindSource(music_name);

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
		game_music = Play(music_name, false, true, (previous_music) ? BLEND_TIME : 0.0f);
		return game_music.get();
	}

	chaos::Sound * Game::Play(char const * name, bool paused, bool looping, float blend_in_time)
	{
		// search manager
		chaos::SoundManager * sound_manager = GetSoundManager();
		if (sound_manager == nullptr)
			return nullptr;
		// search source
		chaos::SoundSource * source = sound_manager->FindSource(name);
		if (source == nullptr)
			return nullptr;

		chaos::PlaySoundDesc play_desc;
		play_desc.paused = paused;
		play_desc.looping = looping;
		play_desc.blend_in_time = blend_in_time;

		// Flag some sounds as "in_game"
		//
		// XXX : IsPlaying() is not the indicator we want because it only tests for one state (there are some Transitions that does not match)
		//       Nevertheless the combinaison:
		//             game_instance != nullptr && !IsPaused()
		//       is exactly what we want
		//if (game_instance != nullptr && !IsPaused())
		if (IsPlaying(true, true))
		{
			chaos::SoundCategory * category = sound_manager->FindCategory("in_game");
			if (category != nullptr && !category->IsPendingKill())
				play_desc.categories.push_back(category);
		}

		return source->Play(play_desc);
	}

	bool Game::InitializeSoundManager()
	{
		// search manager
		chaos::SoundManager * sound_manager = GetSoundManager();
		if (sound_manager == nullptr)
			return false;
		// create the internal "in_game" category for our own purpose
		sound_manager->AddCategory("in_game");

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
		// create the sound manager
		if (!InitializeSoundManager())
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

	bool Game::InitializeParticleTextGenerator(nlohmann::json const & config, boost::filesystem::path const & config_path)
	{
		// get the font sub objects
		nlohmann::json const * fonts_config = chaos::JSONTools::GetStructure(config, "fonts");

		// create the generator
		particle_text_generator = new chaos::ParticleTextGenerator::Generator(*texture_atlas);
		if (particle_text_generator == nullptr)
			return false;

		// bitmaps in generator
		chaos::BitmapAtlas::FolderInfo const * folder_info = texture_atlas->GetFolderInfo("sprites");
		if (folder_info != nullptr)
		{

			// for each bitmap, that correspond to a button, register a [NAME] in the generator	
			for (auto it = gamepad_button_map.begin(); it != gamepad_button_map.end(); ++it)
			{
				std::string const & bitmap_name = it->second.first;
				chaos::BitmapAtlas::BitmapInfo const * info = folder_info->GetBitmapInfo(bitmap_name.c_str());
				if (info == nullptr)
					continue;
				std::string const & generator_alias = it->second.second;
				particle_text_generator->AddBitmap(generator_alias.c_str(), info);
			}
			// embedded sprites
			if (fonts_config != nullptr)
			{
				nlohmann::json const * font_bitmaps_json = chaos::JSONTools::GetStructure(*fonts_config, "bitmaps");
				if (font_bitmaps_json != nullptr && font_bitmaps_json->is_object())
				{
					for (nlohmann::json::const_iterator it = font_bitmaps_json->begin(); it != font_bitmaps_json->end(); ++it)
					{
						if (!it->is_string())
							continue;
						std::string bitmap_name = it.key();
						std::string bitmap_path = it->get<std::string>();
						chaos::BitmapAtlas::BitmapInfo const * info = folder_info->GetBitmapInfo(bitmap_path.c_str());
						if (info == nullptr)
							continue;
						particle_text_generator->AddBitmap(bitmap_name.c_str(), info);
					}
				}
			}
		}

		// the colors
		if (fonts_config != nullptr)
		{
			nlohmann::json const * font_colors_json = chaos::JSONTools::GetStructure(*fonts_config, "colors");
			if (font_colors_json != nullptr && font_colors_json->is_object())
			{
				for (nlohmann::json::const_iterator it = font_colors_json->begin(); it != font_colors_json->end(); ++it)
				{
					glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);  // initialization for if input is smaller than 4
					if (!chaos::LoadFromJSON(*it, color))
						continue;
					std::string color_name = it.key();
					particle_text_generator->AddColor(color_name.c_str(), color);
				}
			}		
		}

		return true;
	}

	bool Game::InitializeGameParticleCreator()
	{

		// shuwww

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
		// purge + fade out all sounds
		chaos::SoundManager * sound_manager = GetSoundManager();
		if (sound_manager != nullptr)
		{
			size_t sound_count = sound_manager->GetSoundCount();
			for (size_t i = 0; i < sound_count; ++i)
			{
				chaos::Sound * sound = sound_manager->GetSound(i);
				if (sound == nullptr)
					continue;
				sound->FadeOut(0.5f, true, true);
			}
		}
		// start the music
		menu_music = Play("menu_music", false, true);
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
		if (current_level_instance != nullptr)
			current_level_instance->OnGameOver();
		if (game_instance != nullptr)
			game_instance->OnGameOver();		
	}

	bool Game::OnEnterPause()
	{
		// start sound
		pause_music = Play("pause_music", false, true);
		// internal code
		CreatePauseMenuHUD();
		// give opportunity to other game classes to respond
		if (current_level_instance != nullptr)
			current_level_instance->OnEnterPause();
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
		if (current_level_instance != nullptr)
			current_level_instance->OnLeavePause();
		if (game_instance != nullptr)
			game_instance->OnLeavePause();
		// resume in-game sounds
		SetInGameSoundPause(false);

		return true;
	}

	void Game::SetInGameSoundPause(bool in_paused)
	{
		// pause/resume in-game sounds
		chaos::SoundManager * sound_manager = GetSoundManager();
		if (sound_manager == nullptr)
			return;
		chaos::SoundCategory * category = sound_manager->FindCategory("in_game");
		if (category == nullptr || category->IsPendingKill())
			return;

		if (in_paused)
			category->StartBlend(chaos::BlendVolumeDesc::BlendOut(0.5f, true, false), true);
		else
		{
			category->StartBlend(chaos::BlendVolumeDesc::BlendIn(0.5f), true);
			category->Pause(false);
		}
	}

	bool Game::OnEnterGame(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad)
	{
		// create the game instance
		game_instance = CreateGameInstance();
		if (game_instance == nullptr)
			return false;
		if (!game_instance->Initialize(this))
			return false;
		// create other resources
		CreatePlayingHUD();
		// create a first player and insert it
		game_instance->CreatePlayer(in_physical_gamepad);
		// notify all players start the game instance
		for (size_t i = 0; i < game_instance->players.size(); ++i)
			if (game_instance->players[i] != nullptr)
				game_instance->OnPlayerEntered(game_instance->players[i].get());
		// select the very first level
		SetNextLevel(true); 

		return true;
	}

	bool Game::OnLeaveGame()
	{
		// save the best score (and other values)
		ConditionnalSaveBestScore();
		// restore main menu condition (level, music ...)
		SetCurrentLevel(nullptr);	
		// notify all players start the game instance
		for (size_t i = 0; i < game_instance->players.size(); ++i)
			if (game_instance->players[i] != nullptr)
				game_instance->OnPlayerLeaved(game_instance->players[i].get());
		// destroy the game instance 
		game_instance = nullptr;
		return true;
	}

	bool Game::CheckGameOverCondition()
	{
		if (game_instance != nullptr)
		{
			// check level game over only if game is started. It could be a background level in main menu
			if (current_level_instance != nullptr)
			{
				bool loop_levels = false; // unused
				if (current_level_instance->IsLevelCompleted(loop_levels)) // level completed => forbid GAME OVER, even if game instance says yes
					return false;
				if (current_level_instance->DoCheckGameOverCondition())
					return true;
			}
			// check for game over in game instance
			if (game_instance->DoCheckGameOverCondition())
				return true;
		}
		return false; // no gameover
	}


	bool Game::CheckLevelCompleted(bool & loop_levels)
	{
		// cheat code
#if _DEBUG
		if (GetCheatSkipLevelRequired())
		{
			loop_levels = true;
			return true;
		}
#endif
		// level knows about that
		death::GameLevelInstance const * level_instance = GetLevelInstance();
		if (level_instance != nullptr)
			if (level_instance->IsLevelCompleted(loop_levels))
				return true;
		return false;
	}

	bool Game::CanCompleteLevel()
	{
		// cheat code
#if _DEBUG
		if (GetCheatSkipLevelRequired())
			return true;
#endif
		// level knows about that
		death::GameLevelInstance const * level_instance = GetLevelInstance();
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
		bool loop_level = false;
		if (CheckLevelCompleted(loop_level))
		{
			if (CanCompleteLevel()) // maybe there is a small delay for an animation or a sound
			{
				if (!SetNextLevel(loop_level))
					RequireExitGame();					
				return false; // do not call remaining code in TickGameLoop(...) specialization
			}
		}
		// tick the level
		if (current_level_instance != nullptr)
			current_level_instance->Tick(delta_time);
		return true;
	}

	int Game::GetCurrentStateTag(bool strict_state, bool use_destination) const
	{
		if (game_state_machine_instance == nullptr)
			return -1;
		chaos::SM::StateBase const * current_state = (strict_state)?
			game_state_machine_instance->GetCurrentStrictState(use_destination) :
			game_state_machine_instance->GetCurrentState();
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
			if (!IsGeometryEmpty(result))
				return result;
		}
		// by default, the world will be the same size than the screen
		return GetViewBox();
	}

	GameLevel * Game::GetLevel()
	{
		GameLevelInstance * li = GetLevelInstance();
		if (li == nullptr)
			return nullptr;
		return li->GetLevel();
	}

	GameLevel const * Game::GetLevel() const
	{
		GameLevelInstance const * li = GetLevelInstance();
		if (li == nullptr)
			return nullptr;
		return li->GetLevel();
	}

	GameLevelInstance * Game::GetLevelInstance()
	{
		return current_level_instance.get();
	}

	GameLevelInstance const * Game::GetLevelInstance() const
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
		GameLevel * current_level = GetLevel();
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
		chaos::shared_ptr<GameLevel> old_level = GetLevel();

		// destroy current level instance, so that new instance can get all resources it want
		if (current_level_instance != nullptr)
		{
			current_level_instance->OnLevelEnded();
			current_level_instance = nullptr;
		}

		// create the new level instance if required
		if (new_level != nullptr)
		{
			current_level_instance = new_level->CreateLevelInstance(this);
			if (current_level_instance == nullptr)
				return false;
			current_level_instance->OnLevelStarted();
		}

		// change the level
		OnLevelChanged(new_level, old_level.get(), current_level_instance.get());
		return true;
	}

	void Game::OnLevelChanged(GameLevel * new_level, GameLevel * old_level, GameLevelInstance * new_level_instance)
	{
		if (game_instance != nullptr)
			game_instance->OnLevelChanged(new_level, old_level, new_level_instance);
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

	size_t Game::GetPlayerCount() const 
	{ 
		// game even not started : no player
		if (game_instance == nullptr)
			return 0;
		// give the instance the responsability 
		return game_instance->GetPlayerCount();
	}

	bool Game::ReloadGameConfiguration()
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

	bool Game::ReloadGPUResources()
	{
		chaos::MyGLFW::SingleWindowApplication * application = chaos::MyGLFW::SingleWindowApplication::GetGLFWApplicationInstance();
		if (application == nullptr)
			return false;
		return application->ReloadGPUResources();
	}

	bool Game::ReloadCurrentLevel()
	{
		if (current_level_instance == nullptr)
			return false;

		chaos::shared_ptr<GameLevel> old_level = current_level_instance->GetLevel(); // keep a reference to prevent the destruction when it will be removed from the levels array
		assert(old_level != nullptr);

		// reload the level
		chaos::shared_ptr<GameLevel> level = DoLoadLevel(old_level->GetPath()); 
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

	GameInstance * Game::CreateGameInstance()
	{
		return new GameInstance(this);
	}

}; // namespace death

