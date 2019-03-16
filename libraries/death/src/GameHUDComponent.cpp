#include <death/GameHUDComponent.h>
#include <death/GameHUD.h>
#include <death/Player.h>

namespace death
{
	Game * GameHUDComponent::GetGame()
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetGame();
	}

	Game const * GameHUDComponent::GetGame() const
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetGame();
	}

	GameInstance * GameHUDComponent::GetGameInstance()
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetGameInstance();
	}

	GameInstance const * GameHUDComponent::GetGameInstance() const
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetGameInstance();
	}

	GameLevel * GameHUDComponent::GetLevel()
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetLevel();
	}

	GameLevel const * GameHUDComponent::GetLevel() const
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetLevel();
	}

	GameLevelInstance * GameHUDComponent::GetLevelInstance()
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetLevelInstance();
	}

	GameLevelInstance const * GameHUDComponent::GetLevelInstance() const
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetLevelInstance();
	}

	Player * GameHUDComponent::GetPlayer(int player_index)
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetPlayer(player_index);
	}

	Player const * GameHUDComponent::GetPlayer(int player_index) const
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetPlayer(player_index);
	}

	void GameHUDComponent::OnInsertedInHUD() // XXX: this function is a "PLACEHOLDER". It does not deserve to be overriden. It is called in a template function 
	{                                        // Arguments can be changed to anything
	}

	void GameHUDComponent::OnRemovedFromHUD()
	{
	}




	void GameHUDSingleAllocationComponent::OnRemovedFromHUD()
	{
		allocations = nullptr;
	}

	void GameHUDTitleComponent::OnInsertedInHUD(char const * game_name, bool normal, chaos::TagType layer_id)
	{
		allocations = hud->GetGameParticleCreator().CreateTitle(game_name, normal, layer_id);
	}

	void GameHUDBestScoreComponent::OnInsertedInHUD(int score)
	{
		chaos::ParticleTextGenerator::GeneratorParams params;
		params.line_height = 50;
		params.hotpoint_type = chaos::Hotpoint::CENTER;
		params.position.x = 0.0f;
		params.position.y = -130.0f;
		params.font_info_name = "normal";

		std::string str = chaos::StringTools::Printf("Best score : %d", score);
		allocations = hud->GetGameParticleCreator().CreateTextParticles(str.c_str(), params, death::GameHUDKeys::TEXT_LAYER_ID);
	}

	void GameHUDInstructionComponent::OnInsertedInHUD(char const * instructions)
	{
		chaos::ParticleTextGenerator::GeneratorParams params;
		params.line_height = 40;
		params.hotpoint_type = chaos::Hotpoint::HMIDDLE | chaos::Hotpoint::TOP;
		params.position.x = 0.0f;
		params.position.y = -200.0f;
		params.font_info_name = "normal";

		allocations = hud->GetGameParticleCreator().CreateTextParticles(instructions, params, death::GameHUDKeys::TEXT_LAYER_ID);
	}

	bool GameHUDScoreComponent::DoTick(double delta_time)
	{
		GameHUDSingleAllocationComponent::DoTick(delta_time);

		Player * player = GetPlayer(0);
		if (player != nullptr)
		{
			int current_score = player->GetScore();
			if (current_score != cached_value)
			{
				if (current_score < 0)
					allocations = nullptr;
				else
					allocations = hud->GetGameParticleCreator().CreateScoringText("Score : %d", current_score, 20.0f, GetGame()->GetViewBox(), death::GameHUDKeys::TEXT_LAYER_ID);
				current_score = cached_value;
			}
		}
		return true;
	}


	int GameHUDFramerateComponent::DoDisplay(chaos::Renderer * renderer, chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const
	{
		framerate = renderer->GetFrameRate();
		return 0;
	}

	bool GameHUDFramerateComponent::DoTick(double delta_time)
	{
		GameHUDSingleAllocationComponent::DoTick(delta_time);

		// test for cache
		if (fabsf(framerate - cached_value) < 0.01f)
			return true;

		// get box
		chaos::box2 view_box = GetGame()->GetViewBox();

		std::pair<glm::vec2, glm::vec2> corners = view_box.GetCorners();

		// format text and create particles
		chaos::ParticleTextGenerator::GeneratorParams params;
		params.line_height = 60;

		params.hotpoint_type = chaos::Hotpoint::RIGHT | chaos::Hotpoint::TOP;
		params.position.x = corners.second.x - 20.0f;
		params.position.y = corners.second.y - 20.0f;
		params.font_info_name = "normal";

		// generate the allocation
		std::string str = chaos::StringTools::Printf("%02.01f FPS", framerate);
		allocations = hud->GetGameParticleCreator().CreateTextParticles(str.c_str(), params, death::GameHUDKeys::TEXT_LAYER_ID);

		cached_value = framerate;

		return true;
	}

}; // namespace death
