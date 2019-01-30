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
		if (!external_manager && particle_manager != nullptr)
			particle_manager->Tick(delta_time);
		return true;
	}

	int GameHUD::DoDisplay(chaos::Renderer * renderer, chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const
	{
		int result = 0; 
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
		// populate the HUD
		char const * game_name = game->GetGameName();
		if (game_name != nullptr)
			RegisterParticles(GameHUDKeys::TITLE_ID, GetGameParticleCreator().CreateTitle(game_name, false, GameHUDKeys::TEXT_LAYER_ID));

		if (game->GetBestScore() > 0)
		{
			chaos::ParticleTextGenerator::GeneratorParams params;
			params.line_height = 50;
			params.hotpoint_type = chaos::Hotpoint::CENTER;
			params.position.x = 0.0f;
			params.position.y = -130.0f;

			params.font_info_name = "normal";

			std::string str = chaos::StringTools::Printf("Best score : %d", game->GetBestScore());
			RegisterParticles(GameHUDKeys::BEST_SCORE_ID, GetGameParticleCreator().CreateTextParticles(str.c_str(), params, death::GameHUDKeys::TEXT_LAYER_ID));
		}
		//
		char const * game_instructions = game->GetGameInstructions();
		if (game_instructions != nullptr)
		{
			chaos::ParticleTextGenerator::GeneratorParams params;
			params.line_height = 40;
			params.hotpoint_type = chaos::Hotpoint::HMIDDLE | chaos::Hotpoint::TOP;
			params.position.x = 0.0f;
			params.position.y = -200.0f;

			params.font_info_name = "normal";

			RegisterParticles(GameHUDKeys::INSTRUCTIONS_ID, GetGameParticleCreator().CreateTextParticles(game_instructions, params, death::GameHUDKeys::TEXT_LAYER_ID));
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
		RegisterParticles(GameHUDKeys::TITLE_ID, GetGameParticleCreator().CreateTitle("Pause", true, death::GameHUDKeys::TEXT_LAYER_ID));
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
		RegisterParticles(GameHUDKeys::TITLE_ID, GetGameParticleCreator().CreateTitle("Game Over", true, death::GameHUDKeys::TEXT_LAYER_ID));
		return true;
	}

		// =============================================
		// PlayingHUD
		// =============================================

	bool PlayingHUD::DoTick(double delta_time)
	{
		// call super method
		GameHUD::DoTick(delta_time);
		// populate the HUD
		UpdateScoreParticles();
		return true;
	}

	void PlayingHUD::UpdateScoreParticles()
	{
		Player * player = game->GetPlayer(0);
		if (player == nullptr)
			return;

		int current_score = player->GetScore();
		if (current_score == cached_score_value)
			return;

		if (current_score < 0)
			UnregisterParticles(GameHUDKeys::SCORE_ID);
		else
			RegisterParticles(GameHUDKeys::SCORE_ID, GetGameParticleCreator().CreateScoringText("Score : %d", current_score, 20.0f, game->GetViewBox(), death::GameHUDKeys::TEXT_LAYER_ID));
		cached_score_value = current_score;
	}

}; // namespace death
