#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

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

	bool GameHUD::Initialize(Game* in_game)
	{
		// ensure valid arguments and not already initialized
		assert(in_game != nullptr);
		assert(game == nullptr);
		// set the game
		game = in_game;
		// read the configuration file
		if (!ReadConfigurableProperties(ReadConfigurablePropertiesContext::INITIALIZATION, false))
			return false;
		// create the components
		if (!FillHUDContent())
			return false;
		return true;
	}

	bool GameHUD::FillHUDContent()
	{
		// FPS
		RegisterComponent(CreateHUDComponent<GameHUDFramerateComponent>(GameHUDKeys::FPS_ID));
		// PERFS
		RegisterComponent(CreateHUDComponent<GameHUDPerfsComponent>(GameHUDKeys::PERFS_ID));
#if _DEBUG
		// FREECAMERA
		RegisterComponent(CreateHUDComponent<GameHUDFreeCameraComponent>(GameHUDKeys::FREECAMERA_ID));
		// DEBUG VALUES
		RegisterComponent(CreateHUDComponent<GameHUDDebugValuesComponent>(GameHUDKeys::DEBUG_VALUES_ID));
		// DEBUG DRAW
		RegisterComponent(CreateHUDComponent<GameHUDDebugDrawComponent>(GameHUDKeys::DEBUG_DRAW_ID));
#endif
		return true;
	}

	void GameHUD::RegisterComponent(GameHUDComponent* in_component)
	{
		assert(in_component != nullptr);
		assert(in_component->hud == nullptr);
		// remove previous component with the key
		UnregisterComponent(in_component->GetTag());
		// register new component for that key
		components.insert(std::make_pair(in_component->GetTag(), in_component));
		// additional initializations
		in_component->hud = this;
		in_component->OnInsertedInHUD();
	}

	void GameHUD::UnregisterComponent(TagType key)
	{
		auto it = components.find(key);
		if (it == components.end())
			return;
		if (GameHUDComponent* component = it->second.get())
		{
			component->OnRemovedFromHUD();
			component->hud = nullptr;
		}
		components.erase(it);
	}

	void GameHUD::UnregisterComponent(GameHUDComponent * in_component)
	{
		assert(in_component != nullptr);
		assert(in_component->hud == this);

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

	int GameHUD::DoDisplay(GPURenderContext * render_context, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const & render_params)
	{
		int result = 0;
		// display components (most of them should do nothing while they re using the particle_manager
		for (auto & it : components)
		{
			GameHUDComponent * component = it.second.get();
			if (component != nullptr)
				result += component->Display(render_context, uniform_provider, render_params);
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
			RegisterComponent(CreateHUDComponent<GameHUDTextComponent>(
				GameHUDKeys::TITLE_ID,
				game_name,
				ParticleTextGenerator::GeneratorParams("title", 150.0f, glm::vec2(0.0f, 0.0f), Hotpoint::CENTER)));
		// the best score
		if (game->GetBestScore() > 0)
		{
			RegisterComponent(CreateHUDComponent<GameHUDBestScoreComponent>(GameHUDKeys::BEST_SCORE_ID));
		}
		// the instructions
		char const * game_instructions = game->GetGameInstructions();
		if (game_instructions != nullptr)
			RegisterComponent(CreateHUDComponent<GameHUDTextComponent>(
				GameHUDKeys::INSTRUCTIONS_ID,
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
		RegisterComponent(CreateHUDComponent<GameHUDTextComponent>(
			GameHUDKeys::TITLE_ID,
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
		RegisterComponent(CreateHUDComponent<GameHUDTextComponent>(
			GameHUDKeys::TITLE_ID,
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
		RegisterComponent(CreateHUDComponent<GameHUDScoreComponent>(GameHUDKeys::SCORE_ID));
		return true;
	}

}; // namespace chaos
