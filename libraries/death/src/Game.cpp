#include <death/Game.h>
#include <death/GameAutomata.h>
#include <death/GamepadManager.h>

#include <chaos/InputMode.h>

namespace death
{

	void Game::OnInputModeChanged(int new_mode, int old_mode)
	{

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
			simulated_stick.y -= 1.0f;
		if (glfwGetKey(glfw_window, GLFW_KEY_UP))
			simulated_stick.y += 1.0f;

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
		// update the game automata
		if (game_automata != nullptr)
			game_automata->Tick(delta_time);
		// clear the cached inputs
		ResetPlayerCachedInputs();
		// tick the particle manager
		if (particle_manager != nullptr)
			particle_manager->Tick((float)delta_time);
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

		return false;
	}
	
	bool Game::OnCharEvent(unsigned int c)
	{
		return true;
	}
	
	void Game::OnMouseButton(int button, int action, int modifier)
	{
		if (GetCurrentStateID() == GameAutomata::STATE_MAINMENU)
		{
			if (action == GLFW_PRESS)
				RequireStartGame();
		}
	}
	
	void Game::OnMouseMove(double x, double y)
	{

	}
	
	void Game::Display(glm::ivec2 const & size)
	{
		

	}

	bool Game::FillAtlasGenerationInput(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path)
	{
		if (!FillAtlasGenerationInputSprites(input, config, config_path))
			return false;
		if (!FillAtlasGenerationInputFonts(input, config, config_path))
			return false;
		return true;
	}

	bool Game::FillAtlasGenerationInputSprites(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path)
	{
		// get the directory where the sprites are
		std::string sprite_directory;
		chaos::JSONTools::GetAttribute(config, "sprite_directory", sprite_directory);

		// Add sprites
		chaos::BitmapAtlas::BitmapSetInput * bitmap_set = input.AddBitmapSet("sprites");
		if (bitmap_set == nullptr)
			return false;

		bitmap_set->AddBitmapFilesFromDirectory(sprite_directory);

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
		chaos::BitmapAtlas::CharacterSetInputParams font_params;
		font_params.max_character_width = 64;
		font_params.max_character_height = 64;

		chaos::BitmapAtlas::CharacterSetInput * character_set1 =
			input.AddCharacterSet(font_name, nullptr, font_path.c_str(), nullptr, true, font_params);

		return (character_set1 != nullptr);
	}

	bool Game::GenerateAtlas(nlohmann::json const & config, boost::filesystem::path const & config_path)
	{
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

	bool Game::LoadLevels()
	{
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
	
	bool Game::InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path)
	{
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
		// initialize the particle manager
		if (!InitializeParticleManager())
			return false;
		// initialize the particle text generator manager
		if (!InitializeParticleTextGenerator())
			return false;
		// load the best score if any
		SerializeBestScore(false);
		return true;
	}

	chaos::MyGLFW::SingleWindowApplication * Game::GetApplication()
	{
		return chaos::MyGLFW::SingleWindowApplication::GetGLFWApplicationInstance();
	}

	chaos::SoundManager * Game::GetSoundManager()
	{
		chaos::MyGLFW::SingleWindowApplication * application = GetApplication();
		if (application == nullptr)
			return nullptr;
		return application->GetSoundManager();
	}

	bool Game::LoadBestScore(std::ifstream & file)
	{
		return true;
	}

	bool Game::SaveBestScore(std::ofstream & file)
	{
		return true;
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
		return true;
	}

	bool Game::CreateGameAutomata()
	{
		return true;
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

		// create game automata
		if (!CreateGameAutomata())		
			return false;
		if (!game_automata->CreateAutomata())
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

	void Game::OnGamepadInput(chaos::MyGLFW::GamepadData & in_gamepad_data)
	{
		// maybe a start game
		if (in_gamepad_data.IsAnyButtonPressed())
			if (game_automata->main_menu_to_playing->TriggerTransition(true))
				return;

		// maybe a game/pause resume
		if (
			(in_gamepad_data.GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_SELECT) == chaos::MyGLFW::BUTTON_BECOME_PRESSED) ||
			(in_gamepad_data.GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_START) == chaos::MyGLFW::BUTTON_BECOME_PRESSED))
		{
			if (RequireTogglePause())
				return;
		}
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
		chaos::BitmapAtlas::BitmapSet const * bitmap_set = texture_atlas->GetBitmapSet("sprites");
		if (bitmap_set != nullptr)
		{
			for (auto it = gamepad_button_map.begin(); it != gamepad_button_map.end(); ++it)
			{
				std::string const & bitmap_name = it->second.first;
				chaos::BitmapAtlas::BitmapEntry const * entry = bitmap_set->GetEntry(bitmap_name.c_str());
				if (entry == nullptr)
					continue;
				std::string const & generator_alias = it->second.second;
				particle_text_generator->AddBitmap(generator_alias.c_str(), entry);
			}
		}

		return true;
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
	}

	void Game::OnGameOver()
	{

	}

	bool Game::OnEnterPause()
	{
		StartPauseMusic(true);
		return true;
	}

	bool Game::OnLeavePause()
	{
		StartGameMusic(false);
		return true;
	}

	bool Game::OnEnterGame()
	{
		StartGameMusic(true);
		return true;
	}

	bool Game::OnLeaveGame(bool gameover)
	{
		StartMainMenuMusic(true);
		return true;
	}

	bool Game::OnAbordGame()
	{
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

	void Game::TickGameLoop(double delta_time)
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

	int Game::GetCurrentStateID() const
	{
		if (game_automata == nullptr)
			return -1;
		chaos::StateMachine::State const * current_state = game_automata->GetCurrentState();
		if (current_state == nullptr)
			return -1;
		return current_state->GetStateID();
	}

	bool Game::IsPlaying() const
	{
		if (GetCurrentStateID() == GameAutomata::STATE_PLAYING)
			return true;
		return false;
	}


	bool Game::RequireGameOver()
	{
		if (game_automata->playing_to_gameover->TriggerTransition(true))
			return true;
		return false;
	}

	bool Game::RequireTogglePause()
	{
		if (game_automata->playing_to_pause->TriggerTransition(true))
			return true;
		if (game_automata->pause_to_playing->TriggerTransition(true))
			return true;
		return false;
	}

	bool Game::RequireExitGame()
	{
		if (game_automata->playing_to_main_menu->TriggerTransition(true))
			return true;
		return false;
	}

	bool Game::RequireStartGame()
	{
		if (game_automata->main_menu_to_playing->TriggerTransition(true))
			return true;
		return false;
	}

	chaos::ParticleAllocation * Game::CreateTextParticles(char const * text, chaos::ParticleTextGenerator::GeneratorParams const & params, int layer_id)
	{
		// find layer of concern
		chaos::ParticleLayer * layer = particle_manager->FindLayer(layer_id);
		if (layer == nullptr)
			return nullptr;

		// generate the tokens
		chaos::ParticleTextGenerator::GeneratorResult result;

		particle_text_generator->Generate(text, result, params);

		chaos::ParticleAllocation * allocation = chaos::ParticleTextGenerator::CreateTextAllocation(layer, result);

		return allocation;
	}


#if 0

	GameHUD * CreatePauseHUD()
	{

	}

	GameHUD * CreateMainMenuHUD()
	{

	}

	GameHUD * CreateGameHUD()
	{

	}

#endif

}; // namespace death

