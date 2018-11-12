#include <death/GameHUD.h>
#include <death/Game.h>

namespace death
{
	// =============================================
	// GameHUD
	// =============================================

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

	bool GameHUD::IsDynamicParticlesRequiringUpdate(chaos::TagType key, int new_value)
	{


		return true;
	}

	bool GameHUD::InitializeHUD(Game * in_game)
	{
		assert(in_game != nullptr);
		// create the particle manager from the game texture atlas
		if (!Initialize(in_game->GetTextureAtlas()))
			return false;

		// create some layers
		int render_order = 0;
		particle_manager->AddLayer<chaos::ParticleDefault::ParticleTrait> (++render_order, death::GameHUDKeys::TEXT_LAYER_ID, "text");

		

		return true;
	}


	bool GameHUD::FillHUDContent(Game * in_game)
	{
		assert(in_game != nullptr);
		if (!InitializeHUD(in_game))
			return false;
		return true;
	}


#if 0
	bool Game::InitializeHUD(death::GameHUD * hud)
	{
		assert(hud != nullptr);
		if (!hud->Initialize(texture_atlas.get()))
			return false;




		//hud->GetParticleManager()->AddLayer<ParticleDefault::PTrait>(++render_order, death::GameHUDKeys::TEXT_LAYER_ID, "text");

		return true;
	}
#endif





	chaos::ParticleAllocation * GameHUD::UpdateDynamicParticles(chaos::TagType key, int new_value)
	{


		return nullptr;
	}

	bool GameHUD::CreateDynamicParticles(chaos::TagType, int initial_value)
	{



		return true;
	}

#if 0
	void GameHUD::CreateDynamicText(TagType key, char const * format, int initial_value)
#endif

		// =============================================
		// MainMenuHUD
		// =============================================

	bool MainMenuHUD::FillHUDContent(Game * in_game)
	{
		// call super method
		if (!GameHUD::FillHUDContent(in_game))
			return false;
		// populate the HUD
		if (in_game->GetGameName() != nullptr)
			RegisterParticles(GameHUDKeys::TITLE_ID, in_game->CreateTitle(in_game->GetGameName(), false, GetParticleManager(), GameHUDKeys::TEXT_LAYER_ID));

		if (in_game->GetBestScore() > 0)
		{
			chaos::ParticleTextGenerator::GeneratorParams params;
			params.line_height = 50;
			params.hotpoint_type = chaos::Hotpoint::CENTER;
			params.position.x = 0.0f;
			params.position.y = -130.0f;

			params.font_info_name = "normal";

			std::string str = chaos::StringTools::Printf("Best score : %d", in_game->GetBestScore());
			RegisterParticles(GameHUDKeys::BEST_SCORE_ID, in_game->CreateTextParticles(str.c_str(), params, GetParticleManager(), death::GameHUDKeys::TEXT_LAYER_ID));
		}
		return true;
	}

	// =============================================
	// PauseMenuHUD
	// =============================================

	bool PauseMenuHUD::FillHUDContent(Game * in_game)
	{
		// call super method
		if (!GameHUD::FillHUDContent(in_game))
			return false;
		// populate the HUD
		RegisterParticles(GameHUDKeys::TITLE_ID, in_game->CreateTitle("Pause", true, GetParticleManager()));
		return true;
	}

	// =============================================
	// GameOverHUD
	// =============================================

	bool GameOverHUD::FillHUDContent(Game * in_game)
	{
		// call super method
		if (!GameHUD::FillHUDContent(in_game))
			return false;
		// populate the HUD
		RegisterParticles(GameHUDKeys::TITLE_ID, in_game->CreateTitle("Game Over", true, GetParticleManager()));
		return true;
	}

		// =============================================
		// PlayingHUD
		// =============================================

	bool PlayingHUD::FillHUDContent(Game * in_game)
	{
		// call super method
		if (!GameHUD::FillHUDContent(in_game))
			return false;
		// populate the HUD


		return true;
	}

	bool PlayingHUD::DoTick(double delta_time)
	{
		// call super method
		GameHUD::DoTick(delta_time);

		return true;
	}



	void PlayingHUD::SetScoreValue(class Game * game, int new_score)
	{
		CacheAndCreateScoreAllocation(game, new_score, "Score : %d", 20.0f, cached_score_value, GameHUDKeys::SCORE_ID);
	}

	void PlayingHUD::CacheAndCreateScoreAllocation(class Game * game, int value, char const * format, float Y,int & cached_value, chaos::TagType key)
	{
		if (value < 0)
			UnregisterParticles(key);
		else if (cached_value != value)
			RegisterParticles(key, game->CreateScoringText(format, value, Y, particle_manager.get()));
		cached_value = value;
	}

}; // namespace death







#if 0



MainMenuHUD * Game::DoCreateMainMenuHUD()
{
	MainMenuHUD * result = new MainMenuHUD;
	if (result == nullptr)
		return nullptr;
	InitializeHUD(result);

	if (game_name != nullptr)
		result->RegisterParticles(GameHUDKeys::TITLE_ID, CreateTitle(game_name, false, result->GetParticleManager(), GameHUDKeys::TEXT_LAYER_ID));

	if (best_score > 0)
	{
		chaos::ParticleTextGenerator::GeneratorParams params;
		params.line_height = 50;
		params.hotpoint_type = chaos::Hotpoint::CENTER;
		params.position.x = 0.0f;
		params.position.y = -130.0f;

		params.font_info_name = "normal";

		std::string str = chaos::StringTools::Printf("Best score : %d", best_score);
		result->RegisterParticles(GameHUDKeys::BEST_SCORE_ID, CreateTextParticles(str.c_str(), params, result->GetParticleManager(), death::GameHUDKeys::TEXT_LAYER_ID));
	}
	return result;
}

PlayingHUD * Game::DoCreatePlayingHUD()
{
	PlayingHUD * result = new PlayingHUD;
	if (result == nullptr)
		return nullptr;
	InitializeHUD(result);
	return result;
}

GameOverHUD * Game::DoCreateGameOverHUD()
{
	GameOverHUD * result = new GameOverHUD;
	if (result == nullptr)
		return nullptr;
	InitializeHUD(result);
	result->RegisterParticles(GameHUDKeys::TITLE_ID, CreateTitle("Game Over", true, result->GetParticleManager()));
	return result;
}

#endif