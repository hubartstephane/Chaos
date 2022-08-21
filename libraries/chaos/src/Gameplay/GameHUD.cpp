#include "chaos/Chaos.h"

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

		Application const * application = Application::GetInstance();
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
		component->InitializeFromConfiguration(*component_config);
	}

	bool GameHUD::Initialize(Game* in_game)
	{
		// ensure valid arguments and not already initialized
		assert(in_game != nullptr);
		assert(game == nullptr);
		game = in_game;
		// create the components
		if (!FillHUDContent())
			return false;
		return true;
	}

#if !_DEBUG
	CHAOS_APPLICATION_ARG(bool, ShowFPS);
	CHAOS_APPLICATION_ARG(bool, ShowPerfs);
#endif
	CHAOS_APPLICATION_ARG(bool,HideFPS);
	CHAOS_APPLICATION_ARG(bool, HidePerfs);

	bool GameHUD::FillHUDContent()
	{
		// FPS
#if !_DEBUG
		if (Arguments::ShowFPS) // CMDLINE
#endif
			if (!Arguments::HideFPS) // CMDLINE
				RegisterComponent(GameHUDKeys::FPS_ID, new GameHUDFramerateComponent());
		// PERFS
#if !_DEBUG
		if (Arguments::ShowPerfs) // CMDLINE
#endif
			if (!Arguments::HidePerfs) // CMDLINE
				RegisterComponent(GameHUDKeys::PERFS_ID, new GameHUDPerfsComponent());

#if _DEBUG
		// FREECAMERA
		RegisterComponent(GameHUDKeys::FREECAMERA_ID, new GameHUDFreeCameraComponent());
		// DEBUG VALUES
		RegisterComponent(GameHUDKeys::DEBUG_VALUES_ID, new GameHUDDebugValuesComponent());
		// DEBUG DRAW
		RegisterComponent(GameHUDKeys::DEBUG_DRAW_ID, new GameHUDDebugDrawComponent());
#endif
		return true;
	}

	void GameHUD::RegisterComponent(TagType key, GameHUDComponent* in_component)
	{
		assert(in_component != nullptr);
		assert(in_component->hud == nullptr);
		// remove previous component with the key
		UnregisterComponent(key);
		// register new component for that key
		components.insert(std::make_pair(key, in_component));
		in_component->SetHUD(this);
		InitializeComponentFromConfiguration(key, in_component); // this will override the component position from JSON file
		in_component->OnInsertedInHUD();
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

	void GameHUD::Clear()
	{
		components.clear();
	}

	bool GameHUD::DoTick(float delta_time)
	{
		// tick the components
		for (auto & it : components)
		{
			GameHUDComponent * component = it.second.get();
			if (component != nullptr)
				component->Tick(delta_time);
		}
		return true;
	}

	int GameHUD::DoDisplay(GPURenderer * renderer, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const & render_params)
	{
		int result = 0;
		// display components (most of them should do nothing while they re using the particle_manager
		for (auto & it : components)
		{
			GameHUDComponent * component = it.second.get();
			if (component != nullptr)
				result += component->Display(renderer, uniform_provider, render_params);
		}
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
				game_name,
				ParticleTextGenerator::GeneratorParams("title", 150.0f, glm::vec2(0.0f, 0.0f), Hotpoint::CENTER)));
		// the best score
		if (game->GetBestScore() > 0)
		{
			RegisterComponent(GameHUDKeys::BEST_SCORE_ID, new GameHUDBestScoreComponent());
		}
		// the instructions
		char const * game_instructions = game->GetGameInstructions();
		if (game_instructions != nullptr)
			RegisterComponent(GameHUDKeys::INSTRUCTIONS_ID, new GameHUDTextComponent(
				game_instructions,
				ParticleTextGenerator::GeneratorParams("normal", 40.0f, glm::vec2(0.0f, 40.0f), Hotpoint::BOTTOM)));

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
			"Pause",
			ParticleTextGenerator::GeneratorParams("title", 150.0f, glm::vec2(0.0f, 0.0f), Hotpoint::CENTER)));
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
			"Game Over",
			ParticleTextGenerator::GeneratorParams("title", 150.0f, glm::vec2(0.0f, 0.0f), Hotpoint::CENTER)));
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
