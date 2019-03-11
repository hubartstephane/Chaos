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







	void GameHUD::RegisterParticles(chaos::TagType key, chaos::ParticleAllocation * allocation, bool remove_previous)
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

	chaos::ParticleAllocation * GameHUD::FindParticleAllocation(chaos::TagType key)
	{
		auto it = particle_allocations.find(key);
		if (it == particle_allocations.end())
			return nullptr;
		return it->second.get();
	}
	chaos::ParticleAllocation const * GameHUD::FindParticleAllocation(chaos::TagType key) const
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

	int GameHUD::DoDisplay(chaos::Renderer * renderer, chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const
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
			RegisterComponent(GameHUDKeys::TITLE_ID, new GameHUDTitleComponent(), game_name, false, GameHUDKeys::TEXT_LAYER_ID);
		// the best score
		if (game->GetBestScore() > 0)
			RegisterComponent(GameHUDKeys::BEST_SCORE_ID, new GameHUDBestScoreComponent(), game->GetBestScore());
		// the instructions
		char const * game_instructions = game->GetGameInstructions();
		if (game_instructions != nullptr)
			RegisterComponent(GameHUDKeys::INSTRUCTIONS_ID, new GameHUDInstructionComponent(), game_instructions);

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
		RegisterComponent(GameHUDKeys::TITLE_ID, new GameHUDTitleComponent(), "Pause", true, GameHUDKeys::TEXT_LAYER_ID);
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
		RegisterComponent(GameHUDKeys::TITLE_ID, new GameHUDTitleComponent(), "Game Over", true, GameHUDKeys::TEXT_LAYER_ID);
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
