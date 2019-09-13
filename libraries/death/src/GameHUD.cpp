#include <death/GameHUD.h>
#include <death/Game.h>
#include <death/GameInstance.h>

namespace death
{
	// =============================================
	// GameHUD
	// =============================================

	GameHUD::GameHUD(Game * in_game) :
		game(in_game)
	{
		assert(in_game != nullptr);
	}

	GameInstance * GameHUD::GetGameInstance()
	{ 
		return game->GetGameInstance(); 
	}

	GameInstance const * GameHUD::GetGameInstance() const 
	{ 
		return game->GetGameInstance(); 
	}

	GameLevel * GameHUD::GetLevel()
	{
		return game->GetLevel();
	}

	GameLevel const * GameHUD::GetLevel() const
	{
		return game->GetLevel();
	}

	GameLevelInstance * GameHUD::GetLevelInstance()
	{
		return game->GetLevelInstance();
	}

	GameLevelInstance const * GameHUD::GetLevelInstance() const
	{
		return game->GetLevelInstance();
	}

	Player * GameHUD::GetPlayer(int player_index)
	{
		return game->GetPlayer(player_index);
	}

	Player const * GameHUD::GetPlayer(int player_index) const
	{
		return game->GetPlayer(player_index);
	}

	void GameHUD::InitializeComponentFromConfiguration(chaos::TagType key, GameHUDComponent * component)
	{
		assert(component != nullptr);

		chaos::Application * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return;

		nlohmann::json const & config = application->GetConfiguration();
		// get the hud config
		nlohmann::json const * hud_config = chaos::JSONTools::GetStructure(config, "hud");
		if (hud_config == nullptr)
			return;
		// get the component config (if existing)
		nlohmann::json const * component_config = chaos::JSONTools::GetStructure(*hud_config, (char const *)key);
		if (component_config == nullptr)
			return;
		// initialize the component from JSON
		component->InitializeFromConfiguration(*component_config, application->GetConfigurationPath());
	}

	bool GameHUD::InitializeHUD()
	{
		// create the particle manager from the game texture atlas
		if (!CreateInternalData(nullptr, game->GetTextGenerator(), game->GetTextureAtlas()))
			return false;
		// Create the layers
		if (!CreateHUDLayers())
			return false;
		// create the particles
		if (!FillHUDContent())
			return false;

		return true;
	}

	bool GameHUD::CreateInternalData(chaos::ParticleManager * in_particle_manager, chaos::ParticleTextGenerator::Generator * in_particle_text_generator, chaos::BitmapAtlas::TextureArrayAtlas * in_texture_atlas)
	{
		assert((in_particle_manager != nullptr) ^ (in_texture_atlas != nullptr)); // cannot have both creation protocole

		// create the particle manager
		if (in_particle_manager != nullptr)
		{
			particle_manager = in_particle_manager;
			external_manager = true;
		}
		else if (in_texture_atlas != nullptr)
		{
			particle_manager = in_particle_manager = new chaos::ParticleManager;
			if (particle_manager == nullptr)
				return false;
			particle_manager->SetTextureAtlas(in_texture_atlas);
			external_manager = false;
		}
		// initialize the particle creator
		particle_creator.Initialize(in_particle_manager, in_particle_text_generator, in_texture_atlas);

		return true;
	}
		
	bool GameHUD::CreateHUDLayers()
	{
		int render_order = 0;
		particle_manager->AddLayer<chaos::ParticleDefault::ParticleTrait>(render_order, death::GameHUDKeys::TEXT_LAYER_ID, "text");

		return true;
	}

	bool GameHUD::FillHUDContent()
	{
#if _DEBUG
		RegisterComponent(GameHUDKeys::FPS_ID, new GameHUDFramerateComponent());
		RegisterComponent(GameHUDKeys::FREECAMERA_ID, new GameHUDFreeCameraComponent(), "FreeCam Mode");
#endif
		return true;
	}

	void GameHUD::UnregisterComponent(chaos::TagType key)
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

	GameHUDComponent * GameHUD::FindComponent(chaos::TagType key)
	{
		auto it = components.find(key);
		if (it == components.end())
			return nullptr;
		return it->second.get();
	}

	GameHUDComponent const * GameHUD::FindComponent(chaos::TagType key) const
	{
		auto it = components.find(key);
		if (it == components.end())
			return nullptr;
		return it->second.get();
	}

	void GameHUD::RegisterParticles(chaos::TagType key, chaos::ParticleAllocationBase * allocation, bool remove_previous)
	{
		if (remove_previous)
			UnregisterParticles(key);
		particle_allocations.insert(std::make_pair(key, allocation));
	}
	
	void GameHUD::UnregisterParticles(chaos::TagType key)
	{
		particle_allocations.erase(key);
	}
		
	void GameHUD::Clear()
	{
		components.clear();
		particle_allocations.clear();
	}

	chaos::ParticleAllocationBase * GameHUD::FindParticleAllocation(chaos::TagType key)
	{
		auto it = particle_allocations.find(key);
		if (it == particle_allocations.end())
			return nullptr;
		return it->second.get();
	}
	chaos::ParticleAllocationBase const * GameHUD::FindParticleAllocation(chaos::TagType key) const
	{
		auto it = particle_allocations.find(key);
		if (it == particle_allocations.end())
			return nullptr;
		return it->second.get();
	}

	bool GameHUD::DoTick(double delta_time)
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

	int GameHUD::DoDisplay(chaos::GPURenderer * renderer, chaos::GPUProgramProviderBase const * uniform_provider, chaos::GPURenderParams const & render_params) const
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
				chaos::ParticleTextGenerator::GeneratorParams("title", 150.0f, glm::vec2(0.0f, 0.0f), chaos::Hotpoint::CENTER), GameHUDKeys::TEXT_LAYER_ID), game_name);
		// the best score
		if (game->GetBestScore() > 0)
		{
			std::string best_score = chaos::StringTools::Printf("Best score: %d", game->GetBestScore());
			RegisterComponent(GameHUDKeys::BEST_SCORE_ID, new GameHUDTextComponent(
				chaos::ParticleTextGenerator::GeneratorParams("normal", 60.0f, glm::vec2(0.0f, -110.0f), chaos::Hotpoint::CENTER), GameHUDKeys::TEXT_LAYER_ID), best_score.c_str());
		}
		// the instructions
		char const * game_instructions = game->GetGameInstructions();
		if (game_instructions != nullptr)
			RegisterComponent(GameHUDKeys::INSTRUCTIONS_ID, new GameHUDTextComponent(
				chaos::ParticleTextGenerator::GeneratorParams("normal", 40.0f, glm::vec2(0.0f, 40.0f), chaos::Hotpoint::HMIDDLE | chaos::Hotpoint::BOTTOM), GameHUDKeys::TEXT_LAYER_ID), game_instructions);

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
			chaos::ParticleTextGenerator::GeneratorParams("title", 150.0f, glm::vec2(0.0f, 0.0f), chaos::Hotpoint::CENTER), GameHUDKeys::TEXT_LAYER_ID), "Pause");
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
			chaos::ParticleTextGenerator::GeneratorParams("title", 150.0f, glm::vec2(0.0f, 0.0f), chaos::Hotpoint::CENTER), GameHUDKeys::TEXT_LAYER_ID), "Game Over");
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
	
}; // namespace death
