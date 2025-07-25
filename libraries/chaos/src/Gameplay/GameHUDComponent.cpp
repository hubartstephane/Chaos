#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// ====================================================================
	// GameHUDComponent
	// ====================================================================

	GPUDevice * GameHUDComponent::GetGPUDevice()
	{
		return WindowApplication::GetGPUDeviceInstance();
	}

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

	bool GameHUDComponent::Initialize()
	{
		if (!ReadConfigurableProperties(ReadConfigurablePropertiesContext::Initialization, false))
			return false;
		return OnInitialize(GetJSONReadConfiguration());
	}

	void GameHUDComponent::OnInsertedInHUD()
	{
	}

	void GameHUDComponent::OnRemovedFromHUD()
	{
	}

	glm::vec2 GameHUDComponent::GetCanvasBoxCorner(box2 const & canvas_box, Hotpoint hotpoint)
	{
		std::pair<glm::vec2, glm::vec2> corners = GetBoxCorners(canvas_box);

		glm::vec2 result;
		// search the X position
		if (int(hotpoint) & int(Hotpoint::LEFT))
			result.x = corners.first.x;
		else if (int(hotpoint) & int(Hotpoint::RIGHT))
			result.x = corners.second.x;
		else
			result.x = canvas_box.position.x;
		// search the Y position
		if (int(hotpoint) & int(Hotpoint::BOTTOM))
			result.y = corners.first.y;
		else if (int(hotpoint) & int(Hotpoint::TOP))
			result.y = corners.second.y;
		else
			result.y = canvas_box.position.y;

		return result;
	}

	bool GameHUDComponent::OnInitialize(JSONReadConfiguration config)
	{
		bool visible = true;
		JSONTools::GetAttribute(config, "visible", visible);
		if (!visible)
			Show(false);
		return true;
	}

	// ====================================================================
	// Standalone functions
	// ====================================================================

	static void TweakTextHotpointWithCanvas(box2 const & canvas_box, ParticleTextGenerator::GeneratorParams& final_params)
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
		glm::vec2 corner = GameHUDComponent::GetCanvasBoxCorner(canvas_box, final_params.hotpoint);
		final_params.position += corner;
	}

	// ====================================================================
	// GameHUDMeshComponent
	// ====================================================================

	void GameHUDMeshComponent::OnRemovedFromHUD()
	{
		mesh = nullptr;
	}

	int GameHUDMeshComponent::DoDisplay(GPURenderContext* render_context, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params)
	{
		int result = GameHUDComponent::DoDisplay(render_context, uniform_provider, render_params);
		if (mesh != nullptr)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			result += mesh->Display(render_context, uniform_provider, render_params);
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
		}
		return result;
	}

	// ====================================================================
	// GameHUDTextComponent
	// ====================================================================

	GameHUDTextComponent::GameHUDTextComponent(char const * in_text, ParticleTextGenerator::GeneratorParams const & in_generator_params):
		text(in_text),
		generator_params(in_generator_params)
	{
	}

	bool GameHUDTextComponent::OnInitialize(JSONReadConfiguration config)
	{
		if (!GameHUDMeshComponent::OnInitialize(config))
			return true;
		JSONTools::GetAttribute(config, "format", text);
		JSONTools::GetAttribute(config, "generator_params", generator_params);
		return true;
	}

	void GameHUDTextComponent::OnInsertedInHUD()
	{
		UpdateMesh();
	}

	void GameHUDTextComponent::UpdateMesh()
	{
		SetText(text.c_str());
	}

	void GameHUDTextComponent::TweakTextGeneratorParams(ParticleTextGenerator::GeneratorParams & final_params) const
	{
		TweakTextHotpointWithCanvas(GetGame()->GetCanvasBox(), final_params);
	}

	void GameHUDTextComponent::SetText(char const * in_text)
	{
		if (StringTools::IsEmpty(in_text))
			mesh = nullptr;
		else
		{
			ParticleTextGenerator::GeneratorParams other_params = generator_params;
			TweakTextGeneratorParams(other_params);

			GPUDrawInterface<VertexDefault> DI(GetGPUDevice(), nullptr);
			DrawText(DI, in_text, other_params);
			mesh = DI.GetDynamicMesh();
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
		GameHUDTextComponent("", in_params)
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
			SetText(in_message);
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
	// GameHUDBestScoreComponent
	// ====================================================================

	GameHUDBestScoreComponent::GameHUDBestScoreComponent(char const* in_text) :
		GameHUDCacheValueTextComponent<int>(in_text)
	{
		generator_params.line_height = 60.0f;
		generator_params.font_info_name = "normal";
		generator_params.position = glm::vec2(0.0f, -110.0f);
		generator_params.hotpoint = Hotpoint::CENTER;
	}

	bool GameHUDBestScoreComponent::QueryValue(int & result) const
	{
		if (Game const * game = GetGame())
		{
			result = game->GetBestScore();
			return true;
		}
		return false;
	}

	// ====================================================================
	// GameHUDScoreComponent
	// ====================================================================

	GameHUDScoreComponent::GameHUDScoreComponent(char const* in_text) :
		GameHUDCacheValueTextComponent<int>(in_text)
	{
		generator_params.line_height = 60.0f;
		generator_params.font_info_name = "normal";
		generator_params.position = glm::vec2(20.0f, -20.0f);
		generator_params.hotpoint = Hotpoint::TOP_LEFT;
	}

	bool GameHUDScoreComponent::QueryValue(int & result) const
	{
		Player const* player = GetPlayer(0);
		if (player == nullptr)
			return false;
		result = player->GetScore();
		return true;
	}

	// ====================================================================
	// GameHUDFramerateComponent
	// ====================================================================

	namespace GlobalVariables
	{
#if !_DEBUG
		CHAOS_GLOBAL_VARIABLE(bool, ShowFPS, false);
		CHAOS_GLOBAL_VARIABLE(bool, ShowPerfs, false);
#endif
		CHAOS_GLOBAL_VARIABLE(bool, HideFPS, false);
		CHAOS_GLOBAL_VARIABLE(bool, HidePerfs, false);
	}

	GameHUDFramerateComponent::GameHUDFramerateComponent(char const * in_text):
		GameHUDCacheValueTextComponent<float>(in_text)
	{
		generator_params.line_height = 60.0f;
		generator_params.font_info_name = "normal";
		generator_params.position = glm::vec2(-20.0f, -20.0f);
		generator_params.hotpoint = Hotpoint::TOP_RIGHT;
	}

	int GameHUDFramerateComponent::DoDisplay(GPURenderContext* render_context, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params)
	{
#if !_DEBUG
		if (!GlobalVariables::ShowFPS.Get())
			return 0;
#endif
		if (GlobalVariables::HideFPS.Get())
			return 0;

		GPURenderContextStats const & stats = render_context->GetStats();

		average_framerate = stats.GetAverageFrameRate();
		return GameHUDCacheValueTextComponent<float>::DoDisplay(render_context, uniform_provider, render_params);
	}

	bool GameHUDFramerateComponent::QueryValue(float & result) const
	{
		if (average_framerate <= 0.0f)
			return false;
		result = std::ceil(average_framerate * 100.0f) / 100.0f;
		return true;
	}


	// ====================================================================
	// GameHUDPerfsComponent
	// ====================================================================

	GameHUDPerfsComponent::GameHUDPerfsComponent(char const* in_text) :
		GameHUDCacheValueTextComponent<std::pair<int, int>>(in_text)
	{
		generator_params.line_height = 30.0f;
		generator_params.font_info_name = "normal";
		generator_params.position = glm::vec2(-20.0f, -80.0f);
		generator_params.hotpoint = Hotpoint::TOP_RIGHT;
	}

	int GameHUDPerfsComponent::DoDisplay(GPURenderContext* render_context, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params)
	{
#if !_DEBUG
		if (!GlobalVariables::ShowPerfs.Get())
			return 0;
#endif
		if (GlobalVariables::HidePerfs.Get())
			return 0;

		GPURenderContextStats const & stats = render_context->GetStats();

		average_drawcall = stats.GetAverageDrawCalls();
		average_vertices = stats.GetAverageVertices();
		average_framerate = stats.GetAverageFrameRate();
		return GameHUDCacheValueTextComponent<std::pair<int, int>>::DoDisplay(render_context, uniform_provider, render_params);
	}

	bool GameHUDPerfsComponent::QueryValue(std::pair<int, int>& result) const
	{
		float rate = (average_framerate == 0.0f) ? 1.0f : average_framerate;
		result = {
			int(float(average_drawcall) / rate),
			int(float(average_vertices) / rate)
		};
		return true;
	}

	void GameHUDPerfsComponent::UpdateMesh()
	{
		SetText(StringTools::Printf(text.c_str(), cached_value.first, cached_value.second).c_str());
	}

	// ====================================================================
	// GameHUDTimeoutComponent
	// ====================================================================

	GameHUDTimeoutComponent::GameHUDTimeoutComponent(char const * in_text) :
		GameHUDCacheValueTextComponent<float>(in_text)
	{
		generator_params.line_height = 60.0f;
		generator_params.font_info_name = "normal";
		generator_params.position = glm::vec2(0.0f, -20.0f);
		generator_params.hotpoint = Hotpoint::TOP;
	}

	bool GameHUDTimeoutComponent::QueryValue(float & result) const
	{
		// early exit
		LevelInstance const * level_instance = GetLevelInstance();
		if (level_instance == nullptr)
			return false;
		// level without timer, hide it
		float level_timeout = level_instance->GetLevelTimeout();
		if (level_timeout < 0.0f)
			return false;
		// compute the timer
		int dc = std::max(digit_count, 0);
		float div = std::pow(10.0f, (float)dc);
		result = std::ceil(level_timeout * div) / div;
		return true;
	}

	void GameHUDTimeoutComponent::TweakTextGeneratorParams(ParticleTextGenerator::GeneratorParams & final_params) const
	{
		GameHUDCacheValueTextComponent<float>::TweakTextGeneratorParams(final_params);
		final_params.default_color = (cached_value >= 10.0f) ? normal_color : final_color;
	}

	bool GameHUDTimeoutComponent::OnInitialize(JSONReadConfiguration config)
	{
		if (!GameHUDCacheValueTextComponent<float>::OnInitialize(config))
			return true;
		JSONTools::GetAttribute(config, "digit_count", digit_count);
		JSONTools::GetAttribute(config, "normal_color", normal_color);
		JSONTools::GetAttribute(config, "final_color", final_color);
		return true;
	}

	// ====================================================================
	// GameHUDLifeComponent
	// ====================================================================

	bool GameHUDLifeComponent::OnInitialize(JSONReadConfiguration config)
	{
		if (!GameHUDMeshComponent::OnInitialize(config))
			return true;

		JSONTools::GetAttribute(config, "hotpoint", hotpoint);
		JSONTools::GetAttribute(config, "position", position);
		JSONTools::GetAttribute(config, "particle_size", particle_size);
		JSONTools::GetAttribute(config, "particle_offset", particle_offset);

		JSONTools::GetAttribute(config, "particle_name", particle_name);
		JSONTools::GetAttribute(config, "heart_beat_sound", heart_beat_sound);
		JSONTools::GetAttribute(config, "heart_beat_frequency", heart_beat_frequency);

		JSONTools::GetAttribute(config, "fadeout_warning_base", fadeout_warning_base);

		return true;
	}

	bool GameHUDLifeComponent::DoTick(float delta_time)
	{
		GameHUDMeshComponent::DoTick(delta_time);
		TickHeartBeat(delta_time);
		return true;
	}

	void GameHUDLifeComponent::TickHeartBeat(float delta_time)
	{
		// early exit
		if (heart_beat_frequency <= 0.0f || heart_beat_sound.empty())
			return;
		// get game instances
		Game * game = GetGame();
		if (game == nullptr || !game->IsPlaying())
			return;
		// update sound and blinking
		if (cached_value == 1)
		{
			warning_value -= delta_time / heart_beat_frequency;
			if (warning_value <= 0.0f)
			{
				game->PlaySound(heart_beat_sound.c_str(), false, false, 0.0f, SoundContext::GAME);

				float fractionnal_part, integer_part;
				fractionnal_part = modf(warning_value, &integer_part);

				warning_value = (1.0f + fractionnal_part);
			}
			InvalidateMesh();
		}
		else
			warning_value = 1.0f;
	}

	bool GameHUDLifeComponent::QueryValue(int& result) const
	{
		// get the player
		Player const* player = GetPlayer(0);
		if (player == nullptr)
			return false;
		// get player life
		result = player->GetLifeCount();
		return true;
	}

	void GameHUDLifeComponent::UpdateMesh()
	{
		GPUDrawInterface<VertexDefault> DI(GetGPUDevice(), nullptr);

		AtlasBitmapInfo const* bitmap_info = DI.FindBitmapInfo(particle_name.c_str());
		if (bitmap_info != nullptr)
		{
			// compute the final size of the particle
			glm::vec2 particle_final_size = bitmap_info->ApplyRatioToSize(particle_size);

			// compute the size of the whole sprites with their offset
			glm::vec2 whole_particle_size =
				particle_final_size +
				glm::abs(particle_offset) * (float)(cached_value - 1);
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

			QuadPrimitive<VertexDefault> quads = DI.AddQuads(cached_value);
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
			mesh = DI.GetDynamicMesh();
		}
	}

	// ====================================================================
	// GameHUDLevelTitleComponent
	// ====================================================================

	GameHUDLevelTitleComponent::GameHUDLevelTitleComponent() :
		GameHUDCacheValueTextComponent<Level const*>("%s")
	{
		generator_params.line_height = 80.0f;
		generator_params.font_info_name = "normal";
		generator_params.position = glm::vec2(-40.0f, 100.0f);
		generator_params.hotpoint = Hotpoint::BOTTOM_RIGHT;
	}


	bool GameHUDLevelTitleComponent::QueryValue(Level const* & result) const
	{
		// get the level
		Level const* level = GetLevel();
		if (level == nullptr)
			return false;
		// hide title after 4.0 seconds (considere level is now null)
		LevelInstance const* level_instance = GetLevelInstance();
		if (level_instance != nullptr && level_instance->GetLevelClockTime() > 4.0)
			return false;
		result = level;
		return true;
	}

	void GameHUDLevelTitleComponent::UpdateMesh()
	{
		// display title
		std::string const& level_title = cached_value->GetLevelTitle();
		if (!level_title.empty())
		{
			SetText(level_title.c_str());
		}
		else
		{
			std::string title = StringTools::Printf("Level %d", cached_value->GetLevelIndex());
			SetText(title.c_str());
		}
	}

	// ====================================================================
	// GameHUDFreeCameraComponent
	// ====================================================================

	GameHUDFreeCameraComponent::GameHUDFreeCameraComponent(char const * in_text):
		GameHUDTextComponent(in_text)
	{
		generator_params.line_height = 60.0f;
		generator_params.font_info_name = "normal";
		generator_params.position = glm::vec2(-20.0f, -80.0f);
		generator_params.hotpoint = Hotpoint::TOP_RIGHT;
		generator_params.default_color = glm::vec4(0.0f, 0.45f, 1.0f, 1.0f); // light blue
	}

	GameHUDFreeCameraComponent::GameHUDFreeCameraComponent(char const* in_text, ParticleTextGenerator::GeneratorParams const & in_params):
		GameHUDTextComponent(in_text, in_params)
	{
	}

	bool GameHUDFreeCameraComponent::DoTick(float delta_time)
	{
		if (mesh != nullptr)
			if (Game * game = GetGame())
				mesh->Show(game->IsFreeCameraMode());
		return true;
	}

	// ====================================================================
	// GameHUDDebugValuesComponent
	// ====================================================================

#if _DEBUG

	GameHUDDebugValuesComponent::GameHUDDebugValuesComponent()
	{
		generator_params.line_height = 30.0f;
		generator_params.font_info_name = "normal";
		generator_params.position = glm::vec2(40.0f, -100.0f);
		generator_params.hotpoint = Hotpoint::TOP_LEFT;
	}

	void GameHUDDebugValuesComponent::AddValue(char const* title, char const* value, float life_time)
	{
		entries.push_back({ title, value, life_time });
		should_update_mesh = true;
	}

	bool GameHUDDebugValuesComponent::DoTick(float delta_time)
	{
		// XXX : do not apply delta_time to entries here because we want entries to be displayed at least once
		last_delta_time = delta_time;
		return true;
	}

	bool GameHUDDebugValuesComponent::DoUpdateGPUResources(GPURenderContext* render_context)
	{
		if (should_update_mesh)
		{
			if (entries.size() == 0)
				mesh = nullptr;
			else
			{
				GPUDrawInterface<VertexDefault> DI(GetGPUDevice(), nullptr);

				size_t largest_title = 0;
				for (Entry const& entry : entries)
					largest_title = std::max(largest_title, entry.title.length());

				std::stringstream stream;
				for (Entry const& entry : entries)
					stream << entry.title << StreamTools::Whitespaces(largest_title - entry.title.length()) << ": " << entry.value << "\n";

				ParticleTextGenerator::GeneratorParams other_params = generator_params;
				TweakTextHotpointWithCanvas(GetGame()->GetCanvasBox(), other_params);
				DrawText(DI, stream.str().c_str(), other_params);

				mesh = DI.GetDynamicMesh();
			}

			// decrease time for all entries
			for (Entry& entry : entries)
				entry.life_time -= last_delta_time;
			// remove timed out entries
			size_t count1 = entries.size();
			entries.erase(std::remove_if(entries.begin(), entries.end(), [](Entry& entry) { return entry.life_time < 0.0f; }), entries.end());
			size_t count2 = entries.size();
			should_update_mesh = (count1 != count2);
		}
		return true;
	}

	void GameHUDDebugValuesComponent::OnInsertedInHUD()
	{
		should_update_mesh = true;
	}

	bool GameHUDDebugValuesComponent::OnInitialize(JSONReadConfiguration config)
	{
		if (!GameHUDMeshComponent::OnInitialize(config))
			return true;
		JSONTools::GetAttribute(config, "generator_params", generator_params);
		return true;
	}

	void DebugValue(char const* title, char const* value, float life_time)
	{
		if (GameApplication* game_application = Application::GetInstance())
			if (Game* game = game_application->GetGame())
				if (GameHUD* hud = game->GetCurrentHUD())
					if (GameHUDDebugValuesComponent* debug_component = hud->FindComponentByClass<GameHUDDebugValuesComponent>())
						debug_component->AddValue(title, value, life_time);
	}

#endif // #if _DEBUG

	// ====================================================================
	// GameHUDDebugDrawComponent
	// ====================================================================

#if _DEBUG

	GameHUDDebugDrawComponent::~GameHUDDebugDrawComponent()
	{
		delete draw_interface;
	}

	int GameHUDDebugDrawComponent::DoDisplay(GPURenderContext* render_context, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params)
	{
		int result = 0;
		if (draw_interface != nullptr)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);

			// shu49
			glPointSize(5.0f);
			glLineWidth(3.0f);

			result = draw_interface->Display(render_context, uniform_provider, render_params);

			glPointSize(1.0f);
			glLineWidth(1.0f);

			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);

			draw_interface->Flush();
		}
		return result;
	}

	GPUDrawInterface<VertexDefault>* GameHUDDebugDrawComponent::GetDebugDrawInterface()
	{
		if (draw_interface == nullptr)
			draw_interface = new GPUDrawInterface<VertexDefault>(GetGPUDevice(), DefaultParticleProgram::GetMaterial());
		return draw_interface;
	}

	GPUDrawInterface<VertexDefault> * GetDebugDrawInterface()
	{
		if (GameApplication* game_application = Application::GetInstance())
			if (Game* game = game_application->GetGame())
				if (GameHUD* hud = game->GetCurrentHUD())
					if (GameHUDDebugDrawComponent* debug_component = hud->FindComponentByClass<GameHUDDebugDrawComponent>())
						return debug_component->GetDebugDrawInterface();
		return nullptr;
	}

#endif // #if _DEBUG


}; // namespace chaos
