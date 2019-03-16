#include <death/GameHUDComponent.h>
#include <death/GameHUD.h>
#include <death/Player.h>

namespace death
{
	// ====================================================================
	// GameHUDComponent
	// ====================================================================

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

	glm::vec2 GameHUDComponent::GetViewBoxCorner(chaos::box2 const & view_box, int hotpoint)
	{
		std::pair<glm::vec2, glm::vec2> corners = view_box.GetCorners();

		glm::vec2 result;
		// search the X position
		if (hotpoint & chaos::Hotpoint::LEFT)
			result.x = corners.first.x;
		else if (hotpoint & chaos::Hotpoint::RIGHT)
			result.x = corners.second.x;
		else
			result.x = view_box.position.x;
		// search the Y position
		if (hotpoint & chaos::Hotpoint::BOTTOM)
			result.y = corners.first.y;
		else if (hotpoint & chaos::Hotpoint::TOP)
			result.y = corners.second.y;
		else
			result.y = view_box.position.y;

		return result;
	}

	// ====================================================================
	// GameHUDSingleAllocationComponent
	// ====================================================================

	void GameHUDSingleAllocationComponent::OnRemovedFromHUD()
	{
		allocations = nullptr;
	}

	// ====================================================================
	// GameHUDTitleComponent
	// ====================================================================

	void GameHUDTitleComponent::OnInsertedInHUD(char const * game_name, bool normal, chaos::TagType layer_id)
	{
		allocations = hud->GetGameParticleCreator().CreateTitle(game_name, normal, layer_id);
	}

	// ====================================================================
	// GameHUDBestScoreComponent
	// ====================================================================

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

	// ====================================================================
	// GameHUDInstructionComponent
	// ====================================================================

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

	// ====================================================================
	// GameHUDScoreComponent
	// ====================================================================

	bool GameHUDScoreComponent::UpdateCachedValue(bool & destroy_allocation)
	{
		Player * player = GetPlayer(0);
		if (player != nullptr)
		{
			int current_score = player->GetScore();
			if (current_score < 0)
				destroy_allocation = true;
			if (current_score != cached_value)
			{
				cached_value = current_score;
				return true;
			}
		}
		return false;
	}

	void GameHUDScoreComponent::TweakTextGeneratorParams(chaos::ParticleTextGenerator::GeneratorParams & params, chaos::box2 const & view_box)
	{		
		int hotpoint = chaos::Hotpoint::TOP_LEFT;

		glm::vec2 corner = GetViewBoxCorner(view_box, hotpoint);
		params.hotpoint_type = hotpoint;
		params.position.x = corner.x + 20.0f;
		params.position.y = corner.y - 20.0f;
	}

	// ====================================================================
	// GameHUDFramerateComponent
	// ====================================================================

	int GameHUDFramerateComponent::DoDisplay(chaos::Renderer * renderer, chaos::GPUProgramProviderBase const * uniform_provider, chaos::RenderParams const & render_params) const
	{
		framerate = renderer->GetFrameRate();
		return GameHUDCacheValueComponent<float>::DoDisplay(renderer, uniform_provider, render_params);
	}

	bool GameHUDFramerateComponent::UpdateCachedValue(bool & destroy_allocation)
	{
		if (fabsf(framerate - cached_value) > 0.01f)
		{
			cached_value = framerate;
			return true;
		}
		return false;
	}
	
	void GameHUDFramerateComponent::TweakTextGeneratorParams(chaos::ParticleTextGenerator::GeneratorParams & params, chaos::box2 const & view_box)
	{
		int hotpoint = chaos::Hotpoint::TOP_RIGHT;

		glm::vec2 corner = GetViewBoxCorner(view_box, hotpoint);
		params.line_height = 60;
		params.hotpoint_type = hotpoint;
		params.position.x = corner.x - 20.0f;
		params.position.y = corner.y - 20.0f;
	}


}; // namespace death
