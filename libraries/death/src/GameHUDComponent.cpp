#include <chaos/ParticleTextGenerator.h>
#include <chaos/Hotpoint.h>

#include <death/GameHUDComponent.h>
#include <death/GameHUD.h>
#include <death/Player.h>
#include <death/SoundContext.h>

namespace death
{
	// ====================================================================
	// GameHUDComponent
	// ====================================================================

	chaos::AutoCastable<Game> GameHUDComponent::GetGame()
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetGame();
	}

	chaos::AutoConstCastable<Game> GameHUDComponent::GetGame() const
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetGame();
	}

	chaos::AutoCastable<GameInstance> GameHUDComponent::GetGameInstance()
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetGameInstance();
	}

	chaos::AutoConstCastable<GameInstance> GameHUDComponent::GetGameInstance() const
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetGameInstance();
	}

	chaos::AutoCastable<Level> GameHUDComponent::GetLevel()
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetLevel();
	}

	chaos::AutoConstCastable<Level> GameHUDComponent::GetLevel() const
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetLevel();
	}

	chaos::AutoCastable<LevelInstance> GameHUDComponent::GetLevelInstance()
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetLevelInstance();
	}

	chaos::AutoConstCastable<LevelInstance> GameHUDComponent::GetLevelInstance() const
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetLevelInstance();
	}

	chaos::AutoCastable<Player> GameHUDComponent::GetPlayer(size_t player_index)
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetPlayer(player_index);
	}

	chaos::AutoConstCastable<Player> GameHUDComponent::GetPlayer(size_t player_index) const
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetPlayer(player_index);
	}

	size_t GameHUDComponent::GetPlayerCount() const
	{
		if (hud == nullptr)
			return 0;
		return hud->GetPlayerCount();
	}

	void GameHUDComponent::OnInsertedInHUD() // XXX: this function is a "PLACEHOLDER". It does not deserve to be overriden. It is called in a template function 
	{                                        // Arguments can be changed to anything
	}

	void GameHUDComponent::OnRemovedFromHUD()
	{
	}

	glm::vec2 GameHUDComponent::GetCanvasBoxCorner(chaos::box2 const & canvas_box, chaos::Hotpoint hotpoint)
	{
		std::pair<glm::vec2, glm::vec2> corners = GetBoxCorners(canvas_box);

		glm::vec2 result;
		// search the X position
		if ((int)hotpoint & (int)chaos::Hotpoint::LEFT)
			result.x = corners.first.x;
		else if ((int)hotpoint & (int)chaos::Hotpoint::RIGHT)
			result.x = corners.second.x;
		else
			result.x = canvas_box.position.x;
		// search the Y position
		if ((int)hotpoint & (int)chaos::Hotpoint::BOTTOM)
			result.y = corners.first.y;
		else if ((int)hotpoint & (int)chaos::Hotpoint::TOP)
			result.y = corners.second.y;
		else
			result.y = canvas_box.position.y;

		return result;
	}

	bool GameHUDComponent::InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		return true;
	}

	void GameHUDComponent::SetHUD(GameHUD * in_hud)
	{
		assert(in_hud != nullptr);
		hud = in_hud;
	}

	// ====================================================================
	// GameHUDSingleAllocationComponent
	// ====================================================================

	void GameHUDSingleAllocationComponent::ShowComponent(bool in_show)
	{
		if (allocations != nullptr)
			allocations->Show(in_show);	
	}

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
		glm::vec2 corner = GetCanvasBoxCorner(canvas_box, final_params.hotpoint);
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
			allocations = hud->GetGameParticleCreator().SpawnTextParticles(layer_id, in_text, other_params);
		}
	}

	// ====================================================================
	// GameHUDNotificationComponent
	// ====================================================================

	GameHUDNotificationComponent::GameHUDNotificationComponent(chaos::TagType in_layer_id) :
		GameHUDTextComponent(in_layer_id)
	{
		generator_params.line_height = 80.0f;
		generator_params.font_info_name = "normal";
		generator_params.position = glm::vec2(-40.0f, 100.0f);
		generator_params.hotpoint = chaos::Hotpoint::BOTTOM_RIGHT;
	}

	GameHUDNotificationComponent::GameHUDNotificationComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_params, chaos::TagType in_layer_id) :
		GameHUDTextComponent(in_params, in_layer_id)
	{
	}

	void GameHUDNotificationComponent::ShowNotification(char const * in_message, float in_lifetime)
	{
		if (chaos::StringTools::IsEmpty(in_message))
			HideNotification();
		else
		{
			current_time = 0.0f;
			lifetime = in_lifetime;
			UpdateTextAllocation(in_message);
		}
	}

	void GameHUDNotificationComponent::HideNotification()
	{
		current_time = 0.0f;
		lifetime = 0.0f;
		allocations = nullptr;
	}

	bool GameHUDNotificationComponent::DoTick(float delta_time)
	{
		GameHUDTextComponent::DoTick(delta_time);
		if (lifetime >= 0.0f && current_time < lifetime)
		{
			current_time += delta_time;
			if (current_time >= lifetime)
				HideNotification();
		}
		return true;
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
		generator_params.hotpoint = chaos::Hotpoint::TOP_LEFT;
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
		generator_params.hotpoint = chaos::Hotpoint::TOP_RIGHT;
	}

	int GameHUDFramerateComponent::DoDisplay(chaos::GPURenderer * renderer, chaos::GPUProgramProviderBase const * uniform_provider, chaos::GPURenderParams const & render_params)
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
		generator_params.hotpoint = chaos::Hotpoint::TOP;
	}

	bool GameHUDTimeoutComponent::UpdateCachedValue(bool & destroy_allocation)
	{
		LevelInstance * level_instance = GetLevelInstance();
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

	GameHUDLifeComponent::GameHUDLifeComponent(chaos::TagType in_layer_id):
		layer_id(in_layer_id)
	{

	}

	bool GameHUDLifeComponent::InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		if (!GameHUDSingleAllocationComponent::InitializeFromConfiguration(json, config_path))
			return true;
		
		chaos::JSONTools::GetAttribute(json, "hotpoint", hotpoint);
		chaos::JSONTools::GetAttribute(json, "position", position);
		chaos::JSONTools::GetAttribute(json, "particle_size", particle_size);
		chaos::JSONTools::GetAttribute(json, "particle_offset", particle_offset);

		chaos::JSONTools::GetAttribute(json, "particle_name", particle_name);
		chaos::JSONTools::GetAttribute(json, "heart_beat_sound", heart_beat_sound);
		chaos::JSONTools::GetAttribute(json, "heart_beat_frequency", heart_beat_frequency);

		chaos::JSONTools::GetAttribute(json, "fadeout_warning_base", fadeout_warning_base);
		
		return true;
	}

	bool GameHUDLifeComponent::DoTick(float delta_time)
	{
		GameHUDSingleAllocationComponent::DoTick(delta_time);
		TickHeartBeat(delta_time);
		UpdateLifeParticles(delta_time);
		return true;
	}

	void GameHUDLifeComponent::TickHeartBeat(float delta_time)
	{
		// early exit
		if (heart_beat_frequency <= 0.0f || heart_beat_sound.empty())
			return;

		// get game instances
		Game * game = GetGame();
		if (!game->IsPlaying())
			return;

		// update sound
		int count = (get_life_count_func)? 
			get_life_count_func() : 
			GetLifeCount();

		if (count == 1)
		{
			warning_value -= delta_time / heart_beat_frequency;
			if (warning_value <= 0.0f)
			{
				game->PlaySound(heart_beat_sound.c_str(), false, false, 0.0f, SoundContext::GAME);

				float fractionnal_part, integer_part;
				fractionnal_part = modf(warning_value, &integer_part);

				warning_value = (1.0f + fractionnal_part);
			}
		}
		else
			warning_value = 1.0f;
	}

	int GameHUDLifeComponent::GetLifeCount() const
	{
		// get the player
		Player const * player = GetGame()->GetPlayer(0);
		if (player == nullptr)
			return -1;
		// get player life, destroy the allocation if no more life
		return player->GetLifeCount();
	}

	void GameHUDLifeComponent::UpdateLifeParticles(float delta_time)
	{
		// early exit
		if (particle_name.empty())
			return;

		// get the number of life
		int count = (get_life_count_func)? 
			get_life_count_func() : 
			GetLifeCount();

		if (count < 0)
			return;

		// destroy the allocation if no more life
		if (count <= 0)
		{
			allocations = nullptr;
			return;
		}
		// create/ resize the allocation
		if (allocations == nullptr || count != cached_value)
		{
			allocations = hud->GetGameParticleCreator().SpawnParticles(layer_id, particle_name.c_str(), count, true);
			if (allocations == nullptr)
				return;
		}

		// compute the final size of the particle
		//
		// XXX: explanation of 'particle_size' member usage
		//      -if .x AND .y are 0     => use the particle size in the atlas
		//      -if .x AND .y are not 0 => override particle size in the atlas
		//      -if .x OR  .y is  0     => use the particle effective ratio to compute the 0 member value




		// shuludum



		glm::vec2 particle_final_size = particle_size;
		if (particle_final_size.x <= 0.0f || particle_final_size.y <= 0.0f)
		{
			chaos::BitmapAtlas::BitmapInfo const * bitmap_info = hud->GetGameParticleCreator().FindBitmapInfo(particle_name.c_str());
			if (bitmap_info != nullptr)
			{
				if (particle_final_size.x <= 0.0f && particle_final_size.y <= 0.0f) // both are invalid
					particle_final_size = glm::vec2(bitmap_info->width, bitmap_info->height);
				else if (particle_final_size.x <= 0.0f)
					particle_final_size.x = particle_final_size.y * bitmap_info->width / bitmap_info->height;
				else
					particle_final_size.y = particle_final_size.x * bitmap_info->height / bitmap_info->width;
			}
		}

		// compute the size of the whole sprites with their offset
		glm::vec2 whole_particle_size =
			particle_final_size +
			glm::abs(particle_offset) * (float)(count - 1);

		// compute the reference point relative to the screen
		glm::vec2 screen_ref = GetCanvasBoxCorner(GetGame()->GetCanvasBox(), hotpoint);

		// compute the bottom-left corner of the whole sprite rectangle
		glm::vec2 whole_particle_ref = chaos::ConvertHotpoint(screen_ref + position, whole_particle_size, hotpoint, chaos::Hotpoint::BOTTOM_LEFT);

		// update the particles members
		glm::vec2 particle_position = whole_particle_ref;

        chaos::ParticleAccessor<chaos::ParticleDefault> accessor = allocations->GetParticleAccessor();
        for (chaos::ParticleDefault & p : accessor)
        {
			p.bounding_box.position = chaos::ConvertHotpoint(particle_position, particle_final_size, chaos::Hotpoint::BOTTOM_LEFT, chaos::Hotpoint::CENTER);
			p.bounding_box.half_size = 0.5f * particle_final_size;

			float fadeout = 1.0f;
			if (warning_value < 0.5f)
				fadeout = fadeout_warning_base + (1.0f - fadeout_warning_base) * warning_value / 0.5f;
			p.color = glm::vec4(1.0f, 1.0f, 1.0f, fadeout);			

			particle_position += glm::abs(particle_offset);
		}

		cached_value = count;
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
		generator_params.hotpoint = chaos::Hotpoint::BOTTOM_RIGHT;
	}

	bool GameHUDLevelTitleComponent::UpdateCachedValue(bool & destroy_allocation) 
	{ 
		// ensure we got a level/level instance
		Level * level = GetLevel();
		LevelInstance * level_instance = GetLevelInstance();

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

	// ====================================================================
	// GameHUDFreeCameraComponent
	// ====================================================================

	GameHUDFreeCameraComponent::GameHUDFreeCameraComponent(chaos::TagType in_layer_id) :
		GameHUDTextComponent(in_layer_id)
	{
		generator_params.line_height = 60.0f;
		generator_params.font_info_name = "normal";
		generator_params.position = glm::vec2(-20.0f, -80.0f);
		generator_params.hotpoint = chaos::Hotpoint::TOP_RIGHT;
		generator_params.default_color = glm::vec4(0.0f, 0.45f, 1.0f, 1.0f); // light blue
	}

	GameHUDFreeCameraComponent::GameHUDFreeCameraComponent(chaos::ParticleTextGenerator::GeneratorParams const & in_params, chaos::TagType in_layer_id) :
		GameHUDTextComponent(in_params, in_layer_id)
	{
	}

	bool GameHUDFreeCameraComponent::DoTick(float delta_time)
	{
		Game * game = GetGame();
		if (game != nullptr)
			ShowComponent(game->IsFreeCameraMode());
		return true;
	}

	// ====================================================================
	// GameHUDTimedComponent
	// ====================================================================

#if 0

	GameHUDTimedComponent::GameHUDTimedComponent(GameHUDComponent * in_child_component, float in_lifetime) :
		child_component(in_child_component),
		lifetime(in_lifetime)
	{
		assert(child_component != nullptr);
		child_component->hud = hud;
	}

	bool GameHUDTimedComponent::DoTick(float delta_time)
	{
		// decrease hud lifetime
		if (lifetime >= 0.0f) // well, child_components do not known their hud yet
		{
			current_time += delta_time;
			if (current_time >= lifetime)
			{
				hud->UnregisterComponent(this);
				return true;
			}
		}
		// tick the child component
		if (child_component != nullptr)
			child_component->Tick(delta_time);
		return true;
	}

	void GameHUDTimedComponent::SetHUD(GameHUD * in_hud)
	{
		GameHUDComponent::SetHUD(in_hud);
		if (child_component != nullptr)
			child_component->SetHUD(in_hud);
	}

	void GameHUDTimedComponent::OnRemovedFromHUD()
	{
		if (child_component != nullptr)
			child_component->OnRemovedFromHUD();
	}

	bool GameHUDTimedComponent::InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		if (child_component != nullptr)
			return child_component->InitializeFromConfiguration(json, config_path);
		return true;
	}
#endif

}; // namespace death
