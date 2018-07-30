#include <death/Game.h>
#include <death/GamepadManager.h>

#include <chaos/InputMode.h>

namespace death
{

	void Game::OnInputModeChanged(int new_mode, int old_mode)
	{

	}
	
	void Game::Tick(double delta_time)
	{

	}
	
	bool Game::OnKeyEvent(int key, int action)
	{
		return false;
	}
	
	bool Game::OnCharEvent(unsigned int c)
	{
		return true;
	}
	
	void Game::OnMouseButton(int button, int action, int modifier)
	{

	}
	
	void Game::OnMouseMove(double x, double y)
	{

	}
	
	void Game::Display(glm::ivec2 const & size)
	{
		

	}

	bool Game::FillAtlasGenerationInput(chaos::BitmapAtlas::AtlasInput & input, nlohmann::json const & config, boost::filesystem::path const & config_path)
	{
		return true;
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

	bool Game::CreateAllMusics()
	{
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

	}

	void Game::OnGamepadInput(chaos::MyGLFW::GamepadData & in_gamepad_data)
	{

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
		return true;
	}

}; // namespace death

