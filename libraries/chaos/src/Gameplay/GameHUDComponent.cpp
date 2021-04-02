#include <chaos/Chaos.h>

namespace chaos
{
	// ====================================================================
	// GameHUDComponent
	// ====================================================================

	AutoCastable<Game> GameHUDComponent::GetGame()
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetGame();
	}

	AutoConstCastable<Game> GameHUDComponent::GetGame() const
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetGame();
	}

	AutoCastable<GameInstance> GameHUDComponent::GetGameInstance()
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetGameInstance();
	}

	AutoConstCastable<GameInstance> GameHUDComponent::GetGameInstance() const
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetGameInstance();
	}

	AutoCastable<Level> GameHUDComponent::GetLevel()
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetLevel();
	}

	AutoConstCastable<Level> GameHUDComponent::GetLevel() const
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetLevel();
	}

	AutoCastable<LevelInstance> GameHUDComponent::GetLevelInstance()
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetLevelInstance();
	}

	AutoConstCastable<LevelInstance> GameHUDComponent::GetLevelInstance() const
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetLevelInstance();
	}

	AutoCastable<Player> GameHUDComponent::GetPlayer(size_t player_index)
	{
		if (hud == nullptr)
			return nullptr;
		return hud->GetPlayer(player_index);
	}

	AutoConstCastable<Player> GameHUDComponent::GetPlayer(size_t player_index) const
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

	glm::vec2 GameHUDComponent::GetCanvasBoxCorner(box2 const & canvas_box, Hotpoint hotpoint)
	{
		std::pair<glm::vec2, glm::vec2> corners = GetBoxCorners(canvas_box);

		glm::vec2 result;
		// search the X position
		if ((int)hotpoint & (int)Hotpoint::LEFT)
			result.x = corners.first.x;
		else if ((int)hotpoint & (int)Hotpoint::RIGHT)
			result.x = corners.second.x;
		else
			result.x = canvas_box.position.x;
		// search the Y position
		if ((int)hotpoint & (int)Hotpoint::BOTTOM)
			result.y = corners.first.y;
		else if ((int)hotpoint & (int)Hotpoint::TOP)
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
	// GameHUDMeshComponent
	// ====================================================================

	void GameHUDMeshComponent::ShowComponent(bool in_show)
	{
		if (mesh != nullptr)
			mesh->Show(in_show);
	}

	void GameHUDMeshComponent::OnRemovedFromHUD()
	{
		mesh = nullptr;
	}

	int GameHUDMeshComponent::DoDisplay(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params)
	{
		int result = GameHUDComponent::DoDisplay(renderer, uniform_provider, render_params);
		if (mesh != nullptr)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			result += mesh->Display(renderer, uniform_provider, render_params);
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
		}
		return result;
	}

	// ====================================================================
	// GameHUDTextComponent
	// ====================================================================

	GameHUDTextComponent::GameHUDTextComponent(ParticleTextGenerator::GeneratorParams const & in_generator_params):
		generator_params(in_generator_params)
	{
	}

	bool GameHUDTextComponent::InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		if (!GameHUDMeshComponent::InitializeFromConfiguration(json, config_path))
			return true;
		JSONTools::GetAttribute(json, "generator_params", generator_params);
		return true;
	}

	void GameHUDTextComponent::OnInsertedInHUD(char const * in_text)
	{
		UpdateTextMesh(in_text);
	}

	void GameHUDTextComponent::TweakTextGeneratorParams(ParticleTextGenerator::GeneratorParams & final_params) const
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

		box2 canvas_box = GetGame()->GetCanvasBox();
		glm::vec2 corner = GetCanvasBoxCorner(canvas_box, final_params.hotpoint);
		final_params.position += corner;
	}

	void GameHUDTextComponent::UpdateTextMesh(char const * in_text)
	{
		if (StringTools::IsEmpty(in_text))
			mesh = nullptr;
		else
		{
			ParticleTextGenerator::GeneratorParams other_params = generator_params;
			TweakTextGeneratorParams(other_params);

			GPUDrawInterface<VertexDefault> DI(nullptr);
			DI.AddText(in_text, other_params);
			mesh = DI.ExtractMesh();
		}
	}

	// ====================================================================
	// GameHUDNotificationComponent
	// ====================================================================

	GameHUDNotificationComponent::GameHUDNotificationComponent()
	{
		generator_params.line_height = 80.0f;
		generator_params.font_info_name = "normal";
		generator_params.position = glm::vec2(-40.0f, 100.0f);
		generator_params.hotpoint = Hotpoint::BOTTOM_RIGHT;
	}

	GameHUDNotificationComponent::GameHUDNotificationComponent(ParticleTextGenerator::GeneratorParams const & in_params):
		GameHUDTextComponent(in_params)
	{
	}

	void GameHUDNotificationComponent::ShowNotification(char const * in_message, float in_lifetime)
	{
		if (StringTools::IsEmpty(in_message))
			HideNotification();
		else
		{
			current_time = 0.0f;
			lifetime = in_lifetime;
			UpdateTextMesh(in_message);
		}
	}

	void GameHUDNotificationComponent::HideNotification()
	{
		current_time = 0.0f;
		lifetime = 0.0f;
		mesh = nullptr;
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

	GameHUDScoreComponent::GameHUDScoreComponent() :
		GameHUDCacheValueComponent<int>("Score: %d", -1) 
	{
		generator_params.line_height = 60.0f;
		generator_params.font_info_name = "normal";
		generator_params.position = glm::vec2(20.0f, -20.0f);
		generator_params.hotpoint = Hotpoint::TOP_LEFT;
	}

	bool GameHUDScoreComponent::UpdateCachedValue(bool & destroy_mesh)
	{
		Player * player = GetPlayer(0);
		if (player != nullptr)
		{
			int current_score = player->GetScore();
			if (current_score < 0)
				destroy_mesh = true;
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

	GameHUDFramerateComponent::GameHUDFramerateComponent():
		GameHUDCacheValueComponent<float>("%02.01f FPS", -1.0f) 
	{
		generator_params.line_height = 60.0f;
		generator_params.font_info_name = "normal";
		generator_params.position = glm::vec2(-20.0f, -20.0f);
		generator_params.hotpoint = Hotpoint::TOP_RIGHT;
	}

	int GameHUDFramerateComponent::DoDisplay(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params)
	{
		framerate = renderer->GetFrameRate();
		return GameHUDCacheValueComponent<float>::DoDisplay(renderer, uniform_provider, render_params);
	}

	bool GameHUDFramerateComponent::UpdateCachedValue(bool & destroy_mesh)
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

	GameHUDTimeoutComponent::GameHUDTimeoutComponent() :
		GameHUDCacheValueComponent<float>("%02.01f", -1.0f) 
	{
		generator_params.line_height = 60.0f;
		generator_params.font_info_name = "normal";
		generator_params.position = glm::vec2(0.0f, -20.0f);
		generator_params.hotpoint = Hotpoint::TOP;
	}

	bool GameHUDTimeoutComponent::UpdateCachedValue(bool & destroy_mesh)
	{
		LevelInstance * level_instance = GetLevelInstance();
		if (level_instance != nullptr)
		{
			float level_timeout = level_instance->GetLevelTimeout();
			// level without timer, hide it
			if (level_timeout < 0.0f)
			{
				destroy_mesh = true;
			}
			else if (fabsf(level_timeout - cached_value) > 0.1f)
			{
				cached_value = level_timeout;
				return true;
			}
		}
		return false;
	}

	void GameHUDTimeoutComponent::TweakTextGeneratorParams(ParticleTextGenerator::GeneratorParams & final_params) const
	{
		GameHUDCacheValueComponent<float>::TweakTextGeneratorParams(final_params);
		final_params.default_color = (cached_value >= 10.0f) ? glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) : glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);	
	}

	// ====================================================================
	// GameHUDLifeComponent
	// ====================================================================

	bool GameHUDLifeComponent::InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		if (!GameHUDMeshComponent::InitializeFromConfiguration(json, config_path))
			return true;
		
		JSONTools::GetAttribute(json, "hotpoint", hotpoint);
		JSONTools::GetAttribute(json, "position", position);
		JSONTools::GetAttribute(json, "particle_size", particle_size);
		JSONTools::GetAttribute(json, "particle_offset", particle_offset);

		JSONTools::GetAttribute(json, "particle_name", particle_name);
		JSONTools::GetAttribute(json, "heart_beat_sound", heart_beat_sound);
		JSONTools::GetAttribute(json, "heart_beat_frequency", heart_beat_frequency);

		JSONTools::GetAttribute(json, "fadeout_warning_base", fadeout_warning_base);
		
		return true;
	}

	bool GameHUDLifeComponent::DoTick(float delta_time)
	{
		GameHUDMeshComponent::DoTick(delta_time);
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
		// get player life
		return player->GetLifeCount();
	}

	void GameHUDLifeComponent::UpdateLifeParticles(float delta_time)
	{
		// get the number of life
		int count = (get_life_count_func) ?
			get_life_count_func() :
			GetLifeCount();
		if (count < 0) 
			return;
		// destroy the mesh if no more life
		if (count <= 0)
		{
			mesh = nullptr;
			return;
		}
		// update the mesh if necessary
		if (count != cached_value || count == 1) // count == 1 for blinking  
		{
			GPUDrawInterface<VertexDefault> DI(nullptr);

			BitmapAtlas::BitmapInfo const* bitmap_info  = DI.FindBitmapInfo(particle_name.c_str());
			if (bitmap_info != nullptr)
			{
				// compute the final size of the particle
				//
				// XXX: explanation of 'particle_size' member usage
				//      -if .x AND .y are 0     => use the particle size in the atlas
				//      -if .x AND .y are not 0 => override particle size in the atlas
				//      -if .x OR  .y is  0     => use the particle effective ratio to compute the 0 member value
				glm::vec2 particle_final_size = particle_size;
				if (particle_final_size.x <= 0.0f || particle_final_size.y <= 0.0f)
				{
					if (particle_final_size.x <= 0.0f && particle_final_size.y <= 0.0f) // both are invalid
						particle_final_size = glm::vec2(bitmap_info->width, bitmap_info->height);
					else if (particle_final_size.x <= 0.0f)
						particle_final_size.x = particle_final_size.y * bitmap_info->width / bitmap_info->height;
					else
						particle_final_size.y = particle_final_size.x * bitmap_info->height / bitmap_info->width;
				}

				// compute the size of the whole sprites with their offset
				glm::vec2 whole_particle_size =
					particle_final_size +
					glm::abs(particle_offset) * (float)(count - 1);
				// compute the reference point relative to the screen
				glm::vec2 screen_ref = GetCanvasBoxCorner(GetGame()->GetCanvasBox(), hotpoint);
				// compute the bottom-left corner of the whole sprite rectangle
				glm::vec2 whole_particle_ref = ConvertHotpoint(screen_ref + position, whole_particle_size, hotpoint, Hotpoint::BOTTOM_LEFT);
				// update the particles members
				glm::vec2 particle_position = whole_particle_ref;
				// get the color
				float fadeout = 1.0f;
				if (warning_value < 0.5f)
					fadeout = fadeout_warning_base + (1.0f - fadeout_warning_base) * warning_value / 0.5f;

				QuadPrimitive<VertexDefault> quads = DI.AddQuads(count);
				while (quads.GetVerticesCount() > 0)
				{
					ParticleDefault particle;
					particle.bounding_box.position = ConvertHotpoint(particle_position, particle_final_size, Hotpoint::BOTTOM_LEFT, Hotpoint::CENTER);
					particle.bounding_box.half_size = 0.5f * particle_final_size;
					particle.texcoords = bitmap_info->GetTexcoords();
					particle.color = { 1.0f, 1.0f, 1.0f, fadeout };

					ParticleToPrimitive(particle, quads);

					particle_position += glm::abs(particle_offset);
					++quads; // next quad
				}

				mesh = DI.ExtractMesh();
			}
			cached_value = count;
		}
	}

	// ====================================================================
	// GameHUDLevelTitleComponent
	// ====================================================================

	GameHUDLevelTitleComponent::GameHUDLevelTitleComponent() :
		GameHUDCacheValueComponent<std::string>("%s", std::string()) 
	{
		generator_params.line_height = 80.0f;
		generator_params.font_info_name = "normal";
		generator_params.position = glm::vec2(-40.0f, 100.0f);
		generator_params.hotpoint = Hotpoint::BOTTOM_RIGHT;
	}

	bool GameHUDLevelTitleComponent::UpdateCachedValue(bool & destroy_mesh)
	{ 
		// ensure we got a level/level instance
		Level * level = GetLevel();
		LevelInstance * level_instance = GetLevelInstance();

		if (level == nullptr || level_instance == nullptr)
		{
			destroy_mesh = true;
			cached_value = std::string();
			return true;
		}

		// dont let the mesh more the 4 seconds visible
		double clock_time = level_instance->GetLevelClockTime();
		if (clock_time > 4.0)
		{
			destroy_mesh = true;
			cached_value = std::string();
			return true;
		}

		// try to find a title
		std::string const * lt = nullptr;
		std::string placeholder_level_title;

		std::string const & level_title = level->GetLevelTitle();
		if (level_title.empty())
		{
			placeholder_level_title = StringTools::Printf("Level %d", level_instance->GetLevel()->GetLevelIndex());
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

	GameHUDFreeCameraComponent::GameHUDFreeCameraComponent()
	{
		generator_params.line_height = 60.0f;
		generator_params.font_info_name = "normal";
		generator_params.position = glm::vec2(-20.0f, -80.0f);
		generator_params.hotpoint = Hotpoint::TOP_RIGHT;
		generator_params.default_color = glm::vec4(0.0f, 0.45f, 1.0f, 1.0f); // light blue
	}

	GameHUDFreeCameraComponent::GameHUDFreeCameraComponent(ParticleTextGenerator::GeneratorParams const & in_params):
		GameHUDTextComponent(in_params)
	{
	}

	bool GameHUDFreeCameraComponent::DoTick(float delta_time)
	{
		Game * game = GetGame();
		if (game != nullptr)
			ShowComponent(game->IsFreeCameraMode());
		return true;
	}

}; // namespace chaos
