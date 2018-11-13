#include <death/GameHUD.h>
#include <death/Game.h>

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

	bool GameHUD::Initialize(chaos::ParticleManager * in_particle_manager, bool in_external_manager)
	{
		// ensure not already initialized or in use
		assert(particle_manager == nullptr);
		assert(in_particle_manager != nullptr);
		assert(particle_allocations.size() == 0);

		// copy the data
		particle_manager = in_particle_manager;
		external_manager = in_external_manager;
		return true;
	}

	bool GameHUD::Initialize(chaos::BitmapAtlas::TextureArrayAtlas * in_texture_atlas)
	{
		// ensure not already initialized or in use
		assert(particle_manager == nullptr);
		assert(in_texture_atlas != nullptr);
		assert(particle_allocations.size() == 0);

		// create a texture manager
		particle_manager = new chaos::ParticleManager;
		if (particle_manager == nullptr)
			return false;
		particle_manager->SetTextureAtlas(in_texture_atlas);

		return true;
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
		particle_allocations.clear();
	}

	bool GameHUD::DoTick(double delta_time)
	{
		if (!external_manager && particle_manager != nullptr)
			particle_manager->Tick(delta_time);
		return true;
	}

	int GameHUD::DoDisplay(chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const
	{
		int result = 0; 
		if (!external_manager && particle_manager != nullptr)
			result += particle_manager->Display(uniform_provider, render_params);
		return result;
	}


	bool GameHUD::InitializeHUD()
	{
		// create the particle manager from the game texture atlas
		if (!Initialize(game->GetTextureAtlas()))
			return false;

		// create some layers
		int render_order = 0;
		particle_manager->AddLayer<chaos::ParticleDefault::ParticleTrait> (++render_order, death::GameHUDKeys::TEXT_LAYER_ID, "text");

		return true;
	}

	bool GameHUD::FillHUDContent()
	{
		if (!InitializeHUD())
			return false;
		return true;
	}

		// =============================================
		// MainMenuHUD
		// =============================================

	bool MainMenuHUD::FillHUDContent()
	{
		// call super method
		if (!GameHUD::FillHUDContent())
			return false;
		// populate the HUD
		if (game->GetGameName() != nullptr)
			RegisterParticles(GameHUDKeys::TITLE_ID, game->CreateTitle(game->GetGameName(), false, GetParticleManager(), GameHUDKeys::TEXT_LAYER_ID));

		if (game->GetBestScore() > 0)
		{
			chaos::ParticleTextGenerator::GeneratorParams params;
			params.line_height = 50;
			params.hotpoint_type = chaos::Hotpoint::CENTER;
			params.position.x = 0.0f;
			params.position.y = -130.0f;

			params.font_info_name = "normal";

			std::string str = chaos::StringTools::Printf("Best score : %d", game->GetBestScore());
			RegisterParticles(GameHUDKeys::BEST_SCORE_ID, game->CreateTextParticles(str.c_str(), params, GetParticleManager(), death::GameHUDKeys::TEXT_LAYER_ID));
		}
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
		// populate the HUD
		RegisterParticles(GameHUDKeys::TITLE_ID, game->CreateTitle("Pause", true, GetParticleManager()));
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
		// populate the HUD
		RegisterParticles(GameHUDKeys::TITLE_ID, game->CreateTitle("Game Over", true, GetParticleManager()));
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
		// populate the HUD

		return true;
	}

	bool PlayingHUD::DoTick(double delta_time)
	{
		// call super method
		GameHUD::DoTick(delta_time);
		// populate the HUD
		int current_score = game->GetCurrentScore();
		if (current_score != cached_score_value)
		{
			if (current_score < 0)
				UnregisterParticles(GameHUDKeys::SCORE_ID);
			else
				RegisterParticles(GameHUDKeys::SCORE_ID, game->CreateScoringText("Score : %d", current_score, 20.0f, particle_manager.get()));

			cached_score_value = current_score;
		}
		return true;
	}

}; // namespace death
