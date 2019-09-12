#include <chaos/ParticleTextGenerator.h>

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

	glm::vec2 GameHUDComponent::GetCanvasBoxCorner(chaos::box2 const & canvas_box, int hotpoint)
	{
		std::pair<glm::vec2, glm::vec2> corners = GetBoxExtremums(canvas_box);

		glm::vec2 result;
		// search the X position
		if (hotpoint & chaos::Hotpoint::LEFT)
			result.x = corners.first.x;
		else if (hotpoint & chaos::Hotpoint::RIGHT)
			result.x = corners.second.x;
		else
			result.x = canvas_box.position.x;
		// search the Y position
		if (hotpoint & chaos::Hotpoint::BOTTOM)
			result.y = corners.first.y;
		else if (hotpoint & chaos::Hotpoint::TOP)
			result.y = corners.second.y;
		else
			result.y = canvas_box.position.y;

		return result;
	}

	bool GameHUDComponent::InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		return true;
	}

	// ====================================================================
	// GameHUDSingleAllocationComponent
	// ====================================================================

	void GameHUDSingleAllocationComponent::OnRemovedFromHUD()
	{
		allocations = nullptr;
	}


	// ====================================================================
	// GameHUDTextAllocationComponent
	// ====================================================================

	GameHUDTextComponent::GameHUDTextComponent(chaos::TagType in_layer_id):
		layer_id(in_layer_id)
	{
	}

	GameHUDTextComponent::GameHUDTextComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_generator_params, chaos::TagType in_layer_id):
		layer_id(in_layer_id),
		generator_params(in_generator_params)
	{
		
	}

	bool GameHUDTextComponent::InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		if (!GameHUDSingleAllocationComponent::InitializeFromConfiguration(json, config_path))
			return true;

		chaos::JSONTools::GetAttribute(json, "generator_params", generator_params);

	//	chaos::JSONTools::GetAttribute(json, "layer_id", layer_id);
		//LoadFromJSON(json, generator_params);

		return true;
	}

	void GameHUDTextComponent::OnInsertedInHUD(char const * in_text)
	{
		UpdateTextAllocation(in_text);
	}

	void GameHUDTextComponent::TweakTextGeneratorParams(chaos::ParticleTextGenerator::GeneratorParams & final_params) const
	{
		// XXX : for text generation, we use a HOTPOINT that describe where is the origin of the set of particles (LEFT, RIGHT, TOP, BOTTOM, CENTER)
		//       It makes sence to use the same HOTPOINT to give the position on SCREEN as a reference
		//    0
		//     +-------------------+
		//     | 0                 |
		//     |  +------+         |     for example, the relative coordinate of a TOP-LEFT text would be the
		//     |  |TEXT  |         |     TOP-LEFT corner of the screen
		//     |  +------+         |
		//     |                   |
		//
		//      As a side effect the params.position is just a relative offset
		//

		chaos::box2 canvas_box = GetGame()->GetCanvasBox();
		glm::vec2 corner = GetCanvasBoxCorner(canvas_box, final_params.hotpoint_type);
		final_params.position += corner;
	}

	void GameHUDTextComponent::UpdateTextAllocation(char const * in_text)
	{
		if (in_text == nullptr)
			allocations = nullptr;
		else
		{
			chaos::ParticleTextGenerator::GeneratorParams other_params = generator_params;
			TweakTextGeneratorParams(other_params);
			allocations = hud->GetGameParticleCreator().CreateTextParticles(in_text, other_params, layer_id);
		}
	}

	// ====================================================================
	// GameHUDScoreComponent
	// ====================================================================

	GameHUDScoreComponent::GameHUDScoreComponent(chaos::TagType in_layer_id) :
		GameHUDCacheValueComponent<int>("Score: %d", -1, in_layer_id) 
	{
		generator_params.line_height = 60.0f;
		generator_params.font_info_name = "normal";
		generator_params.position = glm::vec2(20.0f, -20.0f);
		generator_params.hotpoint_type = chaos::Hotpoint::TOP_LEFT;
	}

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

	// ====================================================================
	// GameHUDFramerateComponent
	// ====================================================================

	GameHUDFramerateComponent::GameHUDFramerateComponent(chaos::TagType in_layer_id):
		GameHUDCacheValueComponent<float>("%02.01f FPS", -1.0f, in_layer_id) 
	{
		generator_params.line_height = 60.0f;
		generator_params.font_info_name = "normal";
		generator_params.position = glm::vec2(-20.0f, -20.0f);
		generator_params.hotpoint_type = chaos::Hotpoint::TOP_RIGHT;
	}

	int GameHUDFramerateComponent::DoDisplay(chaos::GPURenderer * renderer, chaos::GPUProgramProviderBase const * uniform_provider, chaos::GPURenderParams const & render_params) const
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

	// ====================================================================
	// GameHUDTimeoutComponent
	// ====================================================================

	GameHUDTimeoutComponent::GameHUDTimeoutComponent(chaos::TagType in_layer_id) :
		GameHUDCacheValueComponent<float>("%02.01f", -1.0f, in_layer_id) 
	{
		generator_params.line_height = 60.0f;
		generator_params.font_info_name = "normal";
		generator_params.position = glm::vec2(0.0f, -20.0f);
		generator_params.hotpoint_type = chaos::Hotpoint::TOP;
	}

	bool GameHUDTimeoutComponent::UpdateCachedValue(bool & destroy_allocation)
	{
		GameLevelInstance * level_instance = GetLevelInstance();
		if (level_instance != nullptr)
		{
			float level_timeout = level_instance->GetLevelTimeout();
			// level without timer, hide it
			if (level_timeout < 0.0f)
			{
				destroy_allocation = true;
			}
			else if (fabsf(level_timeout - cached_value) > 0.1f)
			{
				cached_value = level_timeout;
				return true;
			}
		}
		return false;
	}

	void GameHUDTimeoutComponent::TweakTextGeneratorParams(chaos::ParticleTextGenerator::GeneratorParams & final_params) const
	{
		GameHUDCacheValueComponent<float>::TweakTextGeneratorParams(final_params);
		final_params.default_color = (cached_value >= 10.0f) ? glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) : glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);	
	}

	// ====================================================================
	// GameHUDLifeComponent
	// ====================================================================

	bool GameHUDLifeComponent::DoTick(double delta_time)
	{
		GameHUDSingleAllocationComponent::DoTick(delta_time);
		TickHeartBeat(delta_time);
		UpdateLifeParticles(delta_time);
		return true;
	}

	void GameHUDLifeComponent::TickHeartBeat(double delta_time)
	{
		Game * game = GetGame();
		if (!game->IsPlaying())
			return;

		Player const * player = game->GetPlayer(0);
		if (player == nullptr)
			return;

		int current_life = player->GetLifeCount();
		if (current_life == 1)
		{
			heart_warning -= heart_beat_speed * (float)delta_time;
			if (heart_warning <= 0.0f)
			{
				game->Play("heartbeat", false, false);

				float fractionnal_part, integer_part;
				fractionnal_part = modf(heart_warning, &integer_part);

				heart_warning = (1.0f + fractionnal_part);
			}
		}
		else
			heart_warning = 1.0f;
	}

	void GameHUDLifeComponent::UpdateLifeParticles(double delta_time)
	{
		// get the player
		Player const * player = GetGame()->GetPlayer(0);
		if (player == nullptr)
			return;
		// get player life, destroy the allocation if no more life
		int current_life = player->GetLifeCount();
		if (current_life <= 0)
		{
			allocations = nullptr;
			return;
		}
		// create/ resize the allocation
		if (allocations == nullptr)
		{
			allocations = hud->GetGameParticleCreator().CreateParticles("life", current_life, GameHUDKeys::LIFE_LAYER_ID);
			if (allocations == nullptr)
				return;
		}
		else
		{
			allocations->Resize(current_life);
			if (current_life > cached_value)
				hud->GetGameParticleCreator().InitializeParticles(allocations.get(), "life", current_life - cached_value);
		}

		// set the color
		chaos::ParticleAccessor<chaos::ParticleDefault::Particle> particles = allocations->GetParticleAccessor<chaos::ParticleDefault::Particle>();

		glm::vec2 corner = GetCanvasBoxCorner(GetGame()->GetCanvasBox(), chaos::Hotpoint::BOTTOM_LEFT);

		glm::vec2 particle_size;
		particle_size.x = 35.0f;
		particle_size.y = 20.0f;

		for (size_t i = 0; i < (size_t)current_life; ++i)
		{
			glm::vec2 position;
			position.x = corner.x + 20.0f + (particle_size.x + 5.0f) * (float)i;
			position.y = corner.y + 20.0f;

			particles[i].bounding_box.position = chaos::Hotpoint::Convert(position, particle_size, chaos::Hotpoint::BOTTOM_LEFT, chaos::Hotpoint::CENTER);
			particles[i].bounding_box.half_size = 0.5f * particle_size;

			float blend_warning = 1.0f;
			if (heart_warning < 0.5f)
				blend_warning = 0.4f + 0.6f * heart_warning / 0.5f;

			particles[i].color = blend_warning * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		cached_value = current_life;
	}

	// ====================================================================
	// GameHUDLevelTitleComponent
	// ====================================================================

	GameHUDLevelTitleComponent::GameHUDLevelTitleComponent(chaos::TagType in_layer_id) :
		GameHUDCacheValueComponent<std::string>("%s", std::string(), in_layer_id) 
	{
		generator_params.line_height = 80.0f;
		generator_params.font_info_name = "normal";
		generator_params.position = glm::vec2(-40.0f, 100.0f);
		generator_params.hotpoint_type = chaos::Hotpoint::BOTTOM_RIGHT;
	}

	bool GameHUDLevelTitleComponent::UpdateCachedValue(bool & destroy_allocation) 
	{ 
		// ensure we got a level/level instance
		GameLevel * level = GetLevel();
		GameLevelInstance * level_instance = GetLevelInstance();

		if (level == nullptr || level_instance == nullptr)
		{
			destroy_allocation = true;
			cached_value = std::string();
			return true;
		}

		// dont let the allocation more the 4 seconds visible
		double clock_time = level_instance->GetLevelClockTime();
		if (clock_time > 4.0)
		{
			destroy_allocation = true;
			cached_value = std::string();
			return true;
		}

		// try to find a title
		std::string const * lt = nullptr;
		std::string placeholder_level_title;

		std::string const & level_title = level->GetLevelTitle();
		if (level_title.empty())
		{
			placeholder_level_title = chaos::StringTools::Printf("Level %d", level_instance->GetLevel()->GetLevelIndex());
			lt = &placeholder_level_title;	
		}
		else
			lt = &level_title;

		// ensure we do not have already cached this title
		if (cached_value == *lt)
			return false;
		cached_value = *lt;
		return true; 
	}

	std::string GameHUDLevelTitleComponent::FormatText() const 
	{
		return cached_value;	
	}

}; // namespace death
