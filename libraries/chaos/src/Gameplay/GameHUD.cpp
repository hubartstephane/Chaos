#include <chaos/Chaos.h>

namespace chaos
{
	// =============================================
	// GameHUD
	// =============================================

	AutoCastable<GameInstance> GameHUD::GetGameInstance()
	{ 
		return game->GetGameInstance(); 
	}

	AutoConstCastable<GameInstance> GameHUD::GetGameInstance() const
	{ 
		return game->GetGameInstance(); 
	}

	AutoCastable<Level> GameHUD::GetLevel()
	{
		return game->GetLevel();
	}

	AutoConstCastable<Level> GameHUD::GetLevel() const
	{
		return game->GetLevel();
	}

	AutoCastable<LevelInstance> GameHUD::GetLevelInstance()
	{
		return game->GetLevelInstance();
	}

	AutoConstCastable<LevelInstance> GameHUD::GetLevelInstance() const
	{
		return game->GetLevelInstance();
	}

	AutoCastable<Player> GameHUD::GetPlayer(size_t player_index)
	{
		return game->GetPlayer(player_index);
	}

	AutoConstCastable<Player> GameHUD::GetPlayer(size_t player_index) const
	{
		return game->GetPlayer(player_index);
	}

	size_t GameHUD::GetPlayerCount() const
	{
		return game->GetPlayerCount();
	}

	void GameHUD::InitializeComponentFromConfiguration(TagType key, GameHUDComponent * component)
	{
		assert(component != nullptr);

		Application * application = Application::GetInstance();
		if (application == nullptr)
			return;

		nlohmann::json const & config = application->GetConfiguration();
		// get the hud config
		nlohmann::json const * hud_config = JSONTools::GetStructure(config, "hud");
		if (hud_config == nullptr)
			return;
		// get the component config (if existing)
		nlohmann::json const * component_config = JSONTools::GetStructure(*hud_config, (char const *)key);
		if (component_config == nullptr)
			return;
		// initialize the component from JSON
		component->InitializeFromConfiguration(*component_config, application->GetConfigurationPath());
	}

	bool GameHUD::Initialize(Game* in_game)
	{
		// ensure valid arguments and not already initialized
		assert(in_game != nullptr);
		assert(game == nullptr);
		game = in_game;

		// create the particle manager from the game texture atlas
		WindowApplication* window_application = Application::GetInstance();
		if (window_application == nullptr)
			return false;

		if (!CreateInternalData(nullptr, window_application->GetTextGenerator(), window_application->GetTextureAtlas()))
			return false;
		// Create the layers
		if (!CreateHUDLayers())
			return false;
		// create the particles
		if (!FillHUDContent())
			return false;

		return true;
	}

	bool GameHUD::CreateInternalData(ParticleManager * in_particle_manager, ParticleTextGenerator::Generator * in_particle_text_generator, BitmapAtlas::TextureArrayAtlas * in_texture_atlas)
	{
		// create the particle manager
		if (in_particle_manager != nullptr)
		{
			assert(in_texture_atlas == nullptr); // cannot have both parameters

			particle_manager = in_particle_manager;
			external_manager = true;
		}
		else if (in_texture_atlas != nullptr)
		{
			assert(in_particle_manager == nullptr); // cannot have both parameters

			particle_manager = in_particle_manager = new ParticleManager;
			if (particle_manager == nullptr)
				return false;
			particle_manager->SetTextureAtlas(in_texture_atlas);
			external_manager = false;
		}
		return true;
	}

	GameParticleCreator GameHUD::GetGameParticleCreator()
	{
		return { particle_manager.get() };
	}
		
	int GameHUD::CreateHUDLayers()
	{
		int render_order = 0;
		particle_manager->AddLayer<ParticleDefaultLayerTrait>(render_order++, GameHUDKeys::TEXT_LAYER_ID, "text");
		return render_order;
	}

	CHAOS_HELP_TEXT(CMD, "-ShowFPS");
	CHAOS_HELP_TEXT(CMD, "-HideFPS");

	bool GameHUD::FillHUDContent()
	{		
#if !_DEBUG
		if (Application::HasApplicationCommandLineFlag("-ShowFPS")) // CMDLINE
#endif
			if (!Application::HasApplicationCommandLineFlag("-HideFPS")) // CMDLINE
				RegisterComponent(GameHUDKeys::FPS_ID, new GameHUDFramerateComponent());
#if _DEBUG
		RegisterComponent(GameHUDKeys::FREECAMERA_ID, new GameHUDFreeCameraComponent(), "Free Camera Mode");
#endif
		return true;
	}

	void GameHUD::UnregisterComponent(TagType key)
	{
		auto it = components.find(key);
		if (it == components.end())
			return;
		GameHUDComponent * component = it->second.get();
		if (component != nullptr)
		{
			component->OnRemovedFromHUD();
			component->hud = nullptr;
		}
		components.erase(it);
	}

	void GameHUD::UnregisterComponent(GameHUDComponent * in_component)
	{
		auto it = components.begin();
		for (; it != components.end(); ++it)
		{
			if (it->second == in_component)
			{
				in_component->OnRemovedFromHUD();
				in_component->hud = nullptr;
				components.erase(it);
				return;
			}
		}
	}


	GameHUDComponent * GameHUD::FindComponent(TagType key)
	{
		auto it = components.find(key);
		if (it == components.end())
			return nullptr;
		return it->second.get();
	}

	GameHUDComponent const * GameHUD::FindComponent(TagType key) const
	{
		auto it = components.find(key);
		if (it == components.end())
			return nullptr;
		return it->second.get();
	}

	void GameHUD::RegisterParticles(TagType key, ParticleAllocationBase * allocation, bool remove_previous)
	{
		if (remove_previous)
			UnregisterParticles(key);
		particle_allocations.insert(std::make_pair(key, allocation));
	}
	
	void GameHUD::UnregisterParticles(TagType key)
	{
		particle_allocations.erase(key);
	}
		
	void GameHUD::Clear()
	{
		components.clear();
		particle_allocations.clear();
	}

	ParticleAllocationBase * GameHUD::FindParticleAllocation(TagType key)
	{
		auto it = particle_allocations.find(key);
		if (it == particle_allocations.end())
			return nullptr;
		return it->second.get();
	}
	ParticleAllocationBase const * GameHUD::FindParticleAllocation(TagType key) const
	{
		auto it = particle_allocations.find(key);
		if (it == particle_allocations.end())
			return nullptr;
		return it->second.get();
	}

	bool GameHUD::DoTick(float delta_time)
	{
		// tick the components
		for (auto it : components)
		{
			GameHUDComponent * component = it.second.get();
			if (component != nullptr)
				component->Tick(delta_time);
		}
		// tick the particle manager if necessary
		if (!external_manager && particle_manager != nullptr)
			particle_manager->Tick(delta_time);

		return true;
	}

	int GameHUD::DoDisplay(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params)
	{
		int result = 0; 
		// display components (most of them should do nothing while they re using the particle_manager
		for (auto it : components)
		{
			GameHUDComponent * component = it.second.get();
			if (component != nullptr)
				result += component->Display(renderer, uniform_provider, render_params);
		}
		// diplay the particle manager if internal
		if (!external_manager && particle_manager != nullptr)
			result += particle_manager->Display(renderer, uniform_provider, render_params);
		return result;
	}

		// =============================================
		// MainMenuHUD
		// =============================================

	bool MainMenuHUD::FillHUDContent()
	{
		// call super method
		if (!GameHUD::FillHUDContent())
			return false;
		// the title
		char const * game_name = game->GetGameName();
		if (game_name != nullptr)
			RegisterComponent(GameHUDKeys::TITLE_ID, new GameHUDTextComponent(
				ParticleTextGenerator::GeneratorParams("title", 150.0f, glm::vec2(0.0f, 0.0f), Hotpoint::CENTER), GameHUDKeys::TEXT_LAYER_ID), game_name);
		// the best score
		if (game->GetBestScore() > 0)
		{
			std::string best_score = StringTools::Printf("Best score: %d", game->GetBestScore());
			RegisterComponent(GameHUDKeys::BEST_SCORE_ID, new GameHUDTextComponent(
				ParticleTextGenerator::GeneratorParams("normal", 60.0f, glm::vec2(0.0f, -110.0f), Hotpoint::CENTER), GameHUDKeys::TEXT_LAYER_ID), best_score.c_str());
		}
		// the instructions
		char const * game_instructions = game->GetGameInstructions();
		if (game_instructions != nullptr)
			RegisterComponent(GameHUDKeys::INSTRUCTIONS_ID, new GameHUDTextComponent(
				ParticleTextGenerator::GeneratorParams("normal", 40.0f, glm::vec2(0.0f, 40.0f), Hotpoint::BOTTOM), GameHUDKeys::TEXT_LAYER_ID), game_instructions);

		return true;
	}

	// =============================================
	// PauseMenuHUD
	// =============================================

	bool PauseMenuHUD::FillHUDContent()
	{
		// call super method
		if (!GameHUD::FillHUDContent())
			return false;
		// the title
		RegisterComponent(GameHUDKeys::TITLE_ID, new GameHUDTextComponent(
			ParticleTextGenerator::GeneratorParams("title", 150.0f, glm::vec2(0.0f, 0.0f), Hotpoint::CENTER), GameHUDKeys::TEXT_LAYER_ID), "Pause");
		return true;
	}

	// =============================================
	// GameOverHUD
	// =============================================

	bool GameOverHUD::FillHUDContent()
	{
		// call super method
		if (!GameHUD::FillHUDContent())
			return false;
		// the title
		RegisterComponent(GameHUDKeys::TITLE_ID, new GameHUDTextComponent(
			ParticleTextGenerator::GeneratorParams("title", 150.0f, glm::vec2(0.0f, 0.0f), Hotpoint::CENTER), GameHUDKeys::TEXT_LAYER_ID), "Game Over");
		return true;
	}

		// =============================================
		// PlayingHUD
		// =============================================

	bool PlayingHUD::FillHUDContent()
	{
		// call super method
		if (!GameHUD::FillHUDContent())
			return false;
		RegisterComponent(GameHUDKeys::SCORE_ID, new GameHUDScoreComponent());
		return true;
	}
	
}; // namespace chaos
