#include "Ludum43Game.h"
#include "Ludum43Particles.h"
#include "Ludum43Level.h"
#include "Ludum43HUD.h"

#include <chaos/JSONTools.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/FileTools.h>
#include <chaos/WinTools.h>
#include <chaos/Application.h>
#include <chaos/MathTools.h>
#include <chaos/InputMode.h>
#include <chaos/GeometryFramework.h>
#include <chaos/CollisionFramework.h>
#include <chaos/GPUFramebufferGenerator.h>
#include <chaos/GLTools.h>

#include <death/GameParticles.h>

LudumGame::LudumGame()
{		
	game_name = "Quantic Paouf IV";
	game_instructions = R"INSTRUCTIONS(
Save particles to the level exit.
Black holes and Shadows are dangerous.
[ButtonA] Turbo
[ButtonB] Ejects particles
		)INSTRUCTIONS";
	camera_safe_zone = glm::vec2(0.2f, 0.2f);
}

bool LudumGame::OnEnterGame(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad)
{
	if (!death::Game::OnEnterGame(in_physical_gamepad))
		return false;
	PlaySound("start", false, false);
	return true;
}

void LudumGame::DoDisplay(chaos::Renderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::RenderParams const & render_params)
{
	// clear the color buffers
	glm::vec4 clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

	// clear the depth buffers
	float far_plane = 1000.0f;
	glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

	// some states
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// try to capture the "WorldBounds layer"
	// XXX : in menu, there is no current_level_instance, so we can test the pointer to exaclty know what to do
	//
	death::TiledMap::LayerInstance * worldlimits = nullptr;

	death::TiledMap::LevelInstance * ludum_level_instance = dynamic_cast<death::TiledMap::LevelInstance*>(current_level_instance.get());
	if (ludum_level_instance != nullptr)
	{
		if (GenerateFramebuffer(render_params.screen_size, framebuffer_other) && GenerateFramebuffer(render_params.screen_size, framebuffer_worldlimits))
		{
			// RENDER TARGET 1 : SPECIAL WorldLimits (on red channel), Enlarged enemies (on blue channel)
			{
				framebuffer_worldlimits->BeginRendering();

				glViewport(0, 0, render_params.screen_size.x, render_params.screen_size.y);

				glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

				// World limits on RED
				{
					chaos::DisableReferenceCount<chaos::ParticleLayerFilterList> filter;
					filter.name_filter.enable_names.push_back("WorldLimits");

					chaos::RenderParams other_render_params = render_params;
					other_render_params.object_filter = &filter;

					glColorMask(true, false, false, true);
					ludum_level_instance->Display(renderer, uniform_provider, other_render_params);
				}

				// (enlarged) Enemies on GREEN  (position_blend_ratio => enlarged for enemies)
				//  BLACK => greatest deformation, white => smallest deformation
				{
					chaos::DisableReferenceCount<chaos::ParticleLayerFilterList> filter;
					filter.name_filter.enable_names.push_back("Enemies");

					chaos::RenderParams other_render_params = render_params;
					other_render_params.object_filter = &filter;

					chaos::GPUProgramProviderChain enlarged_provider(uniform_provider);
					enlarged_provider.AddVariableValue("position_blend_ratio", 0.0f);

					glColorMask(false, true, false, true);
					ludum_level_instance->Display(renderer, &enlarged_provider, other_render_params);
					glColorMask(true, true, true, true);
				}

				framebuffer_worldlimits->EndRendering();
			}

			// RENDER TARGET 2 : all objects that are to be deformed (except Enemies and Player and atoms)
			{
				framebuffer_other->BeginRendering();
				glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

				chaos::DisableReferenceCount<chaos::ParticleLayerFilterList> filter;
				filter.name_filter.forbidden_names.push_back("Enemies");
				filter.name_filter.forbidden_names.push_back("Atoms");
				filter.name_filter.forbidden_names.push_back("PlayerAndCamera");
				filter.name_filter.forbidden_names.push_back("WorldLimits");

#if 0
				filter.name_filter.forbidden_names.push_back("BackgroundObject1");
				filter.name_filter.forbidden_names.push_back("BackgroundObject2");
				filter.name_filter.forbidden_names.push_back("BackgroundObject3");
				filter.name_filter.forbidden_names.push_back("BackgroundObject4");
				filter.name_filter.forbidden_names.push_back("BackgroundObject5");
#endif

				chaos::RenderParams other_render_params = render_params;
				other_render_params.object_filter = &filter;

				// draw particle system (the background)
				if (particle_manager != nullptr)
					particle_manager->Display(renderer, uniform_provider, other_render_params);
				current_level_instance->Display(renderer, uniform_provider, other_render_params);

				framebuffer_other->EndRendering();
			}

			// COMBINE STEP 1 & STEP 2 (blend_backgrounds = 1 for default rendering, 0 for texture combining)
			{
				chaos::GLTools::SetViewport(render_params.viewport);

				chaos::GPUProgramProviderChain main_provider(uniform_provider);

				chaos::GPUFramebufferAttachmentInfo const * attachment_worldlimits = framebuffer_worldlimits->GetColorAttachment(0);
				if (framebuffer_worldlimits != nullptr)
				{
					chaos::GPUTexture * texture = attachment_worldlimits->texture.get();
					if (texture != nullptr)
						main_provider.AddVariableValue("extra_background", texture);
				}

				chaos::GPUFramebufferAttachmentInfo const * attachment_other = framebuffer_other->GetColorAttachment(0);
				if (attachment_other != nullptr)
				{
					chaos::GPUTexture * texture = attachment_other->texture.get();
					if (texture != nullptr)
						main_provider.AddVariableValue("background", texture);
				}

				main_provider.AddVariableValue("blend_backgrounds", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

				if (particle_manager != nullptr)
					particle_manager->Display(renderer, &main_provider, render_params);
			}


			// simply render player and ennemies
			{
				chaos::DisableReferenceCount<chaos::ParticleLayerFilterList> filter;
				filter.name_filter.enable_names.push_back("Enemies");
				filter.name_filter.enable_names.push_back("Atoms");
				filter.name_filter.enable_names.push_back("PlayerAndCamera");
#if 0
				filter.name_filter.forbidden_names.push_back("BackgroundObject1");
				filter.name_filter.forbidden_names.push_back("BackgroundObject2");
				filter.name_filter.forbidden_names.push_back("BackgroundObject3");
				filter.name_filter.forbidden_names.push_back("BackgroundObject4");
				filter.name_filter.forbidden_names.push_back("BackgroundObject5");
#endif

				chaos::RenderParams other_rendering_params = render_params;
				other_rendering_params.object_filter = &filter;

				// draw particle system (the background)
				current_level_instance->Display(renderer, uniform_provider, other_rendering_params);
			}
		}
	}
	else
	{
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		// draw particle system (the background)
		if (particle_manager != nullptr)
			particle_manager->Display(renderer, uniform_provider, render_params);

	}

	// finally draw the hud
	if (hud != nullptr)
		hud->Display(renderer, uniform_provider, render_params);
}

void LudumGame::OnInputModeChanged(int new_mode, int old_mode)
{

}

void LudumGame::ResetGameVariables()
{
	death::Game::ResetGameVariables();
	current_cooldown  = 0.0f;

	current_dash_cooldown = 0.0f;
	current_dash_duration = 0.0f;
	
	previous_frame_life = 0.0;

	waken_up_particle_count = 0;
	current_score = 0;
	heart_beat_time = 0.0f;
	level_timeout = 0.0f;

}

bool LudumGame::CheckGameOverCondition()
{
	ParticlePlayer const * player_particle = GetPlayerParticle(0);
	if (player_particle == nullptr || level_timeout <= 0.0f)
	{
		RequireGameOver();
		return true;
	}
	return false;
}

bool LudumGame::TickGameLoop(double delta_time)
{
	// super call
	if (!death::Game::TickGameLoop(delta_time))
		return false;
	// displace the player
	UpdatePlayerAcceleration(delta_time);
	// dash values update
	TickDashValues(delta_time);
	// cooldown
	TickCooldown(delta_time);
	// tick sound for heart beat
	TickHeartBeat(delta_time);
	// update time out
	if (!GetCheatMode())
	{
		level_timeout -= (float)delta_time;
		if (level_timeout < 0.0f)
			level_timeout = 0.0f;
	}
	return true;
}

void LudumGame::TickHeartBeat(double delta_time) 
{
	ParticlePlayer const * particle_player = GetPlayerParticle(0);
	if (particle_player == nullptr)
		return;

	float limit_value = 1.4f;

	if (particle_player->life < previous_frame_life)
		limit_value = 0.4f;
	previous_frame_life = particle_player->life;
	

	heart_beat_time += (float)delta_time;
	if (heart_beat_time >= limit_value)
	{
		heart_beat_time = 0.0f;
		PlaySound("heartbeat", false, false);	
	}
}

death::GameHUD * LudumGame::DoCreatePlayingHUD()
{
	return new LudumPlayingHUD(this);
}

chaos::SM::StateMachine * LudumGame::DoCreateGameStateMachine()
{
	return new LudumStateMachine(this);
}

bool LudumGame::DeclareParticleClasses()
{
	if (!Game::DeclareParticleClasses())
		return false;

	chaos::ClassTools::DeclareClass<ParticlePlayer, ParticleBase>("ParticlePlayer");
	chaos::ClassTools::DeclareClass<ParticleEnemy, ParticleBase>("ParticleEnemy");
	chaos::ClassTools::DeclareClass<ParticleAtom, ParticleBase>("ParticleAtom");
	return true;
}

bool LudumGame::InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path, bool hot_reload)
{
	if (!death::Game::InitializeGameValues(config, config_path, hot_reload))
		return false;

	DEATHGAME_JSON_ATTRIBUTE(initial_player_life);
	DEATHGAME_JSON_ATTRIBUTE(initial_particle_life);

	DEATHGAME_JSON_ATTRIBUTE(dash_duration);
	DEATHGAME_JSON_ATTRIBUTE(dash_cooldown);
	DEATHGAME_JSON_ATTRIBUTE(dash_velocity);
	DEATHGAME_JSON_ATTRIBUTE(cooldown);

	DEATHGAME_JSON_ATTRIBUTE(player_attraction_minradius);
	DEATHGAME_JSON_ATTRIBUTE(player_attraction_maxradius);
	DEATHGAME_JSON_ATTRIBUTE(player_tangent_force);		
	DEATHGAME_JSON_ATTRIBUTE(player_attraction_force);	
	DEATHGAME_JSON_ATTRIBUTE(player_repulsion_force);	
	DEATHGAME_JSON_ATTRIBUTE(player_slowing_factor);		
	DEATHGAME_JSON_ATTRIBUTE(player_max_velocity);
	DEATHGAME_JSON_ATTRIBUTE(player_acceleration);

	DEATHGAME_JSON_ATTRIBUTE(worldlimits_attraction_maxradius_offset);
	DEATHGAME_JSON_ATTRIBUTE(worldlimits_attraction_minradius_offset);
	DEATHGAME_JSON_ATTRIBUTE(enemy_attraction_minradius_offset);
	DEATHGAME_JSON_ATTRIBUTE(enemy_attraction_maxradius_offset);

	DEATHGAME_JSON_ATTRIBUTE(worldlimits_attraction_maxradius_factor);
	DEATHGAME_JSON_ATTRIBUTE(worldlimits_attraction_minradius_factor);
	DEATHGAME_JSON_ATTRIBUTE(enemy_attraction_minradius_factor);
	DEATHGAME_JSON_ATTRIBUTE(enemy_attraction_maxradius_factor);

	DEATHGAME_JSON_ATTRIBUTE(enemy_tangent_force);		
	DEATHGAME_JSON_ATTRIBUTE(enemy_attraction_force);	
	DEATHGAME_JSON_ATTRIBUTE(enemy_repulsion_force);	

	DEATHGAME_JSON_ATTRIBUTE(particle_slowing_factor);		
	DEATHGAME_JSON_ATTRIBUTE(particle_min_radius_factor);
	DEATHGAME_JSON_ATTRIBUTE(particle_max_radius_factor);
	
	DEATHGAME_JSON_ATTRIBUTE(particle_max_velocity);

	DEATHGAME_JSON_ATTRIBUTE(world_clamp_radius);

	return true;
}

death::TiledMap::Level * LudumGame::CreateTiledMapLevel()
{
	return new LudumLevel();
}

int LudumGame::AddParticleLayers()
{
	return death::Game::AddParticleLayers();
}

bool LudumGame::InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	if (!death::Game::InitializeFromConfiguration(config, config_path))
		return false;

	return true;
}

void LudumGame::OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance, death::GameLevelInstance * old_level_instance)
{
	static float DEFAULT_LEVEL_TIMEOUT = 50.0f;
	static int   DEFAULT_LEVEL_PARTICLE_REQUIREMENT = 10;

	// super method
	death::Game::OnLevelChanged(new_level, old_level, new_level_instance, old_level_instance);

	// internal
	current_cooldown = 0.0f;
	current_dash_cooldown = 0.0f;
	current_dash_duration = 0.0f;
	previous_frame_life = 0.0f;
	current_score += waken_up_particle_count;
	waken_up_particle_count = 0;
	level_timeout = DEFAULT_LEVEL_TIMEOUT;
	level_particle_requirement = DEFAULT_LEVEL_PARTICLE_REQUIREMENT;

	// change the background image and the level time
	std::string const * background_name = nullptr;
	if (new_level_instance != nullptr)
	{		
		death::TiledMap::Level const * level = dynamic_cast<death::TiledMap::Level const *>(new_level_instance->GetLevel());
		if (level != nullptr)
			background_name = level->GetTiledMap()->FindPropertyString("BACKGROUND_NAME");	

		level_timeout = level->GetTiledMap()->FindPropertyFloat("LEVEL_TIME", DEFAULT_LEVEL_TIMEOUT);
		level_particle_requirement = level->GetTiledMap()->FindPropertyInt("LEVEL_PARTICLE_REQUIREMENT", DEFAULT_LEVEL_PARTICLE_REQUIREMENT);
	}
	CreateBackgroundImage(nullptr, (background_name == nullptr)? nullptr : background_name->c_str());

	// play a sound
	if (new_level != nullptr && old_level != nullptr)
		PlaySound("next_level", false, false);


	
}

void LudumGame::UpdatePlayerAcceleration(double delta_time)
{
	death::Player * player = GetPlayer(0);
	if (player == nullptr)
		return;

	ParticlePlayer * player_particle = GetPlayerParticle(0);
	if (player_particle == nullptr)
		return;
	player_particle->acceleration = glm::vec2(0.0f, 0.0f);

	float left_length_2  = glm::length2(player->left_stick_position);
	float right_length_2 = glm::length2(player->right_stick_position);
	if (left_length_2 > 0.0f || right_length_2 > 0.0f)
	{
		glm::vec2 acceleration = (left_length_2 > right_length_2)?
			player->left_stick_position / chaos::MathTools::Sqrt(left_length_2):
			player->right_stick_position / chaos::MathTools::Sqrt(right_length_2);

		player_particle->acceleration = player_acceleration * glm::vec2(1.0f, -1.0f) * acceleration; // axis Y reversed
	}
}

void LudumGame::TickCooldown(double delta_time)
{
	current_cooldown = chaos::MathTools::Maximum(0.0f, current_cooldown - (float)delta_time);
}

void LudumGame::HandlePlayerGamepadInput(double delta_time, chaos::MyGLFW::GamepadData & in_gamepad_data)
{
	death::Game::HandlePlayerGamepadInput(delta_time, in_gamepad_data);

	if (chaos::Application::GetApplicationInputMode() == chaos::InputMode::Gamepad)
	{
		bool dash = in_gamepad_data.IsButtonPressed(chaos::MyGLFW::XBOX_BUTTON_A, false); 
		SetPlayerDashMode(dash);

		bool reversed_mode = in_gamepad_data.IsButtonPressed(chaos::MyGLFW::XBOX_BUTTON_B, false);
		SetPlayerReverseMode(reversed_mode);
	}
}

void LudumGame::HandleKeyboardInputs()
{
	death::Game::HandleKeyboardInputs();
	

	if (chaos::Application::GetApplicationInputMode() == chaos::InputMode::Keyboard)
	{
		bool dash_mode = (glfwGetKey(glfw_window, GLFW_KEY_SPACE) != GLFW_RELEASE);
		SetPlayerDashMode(dash_mode);
		
		bool reversed_mode = (glfwGetKey(glfw_window, GLFW_KEY_RIGHT_CONTROL) != GLFW_RELEASE);
		SetPlayerReverseMode(reversed_mode);
	}
}

void LudumGame::SetPlayerReverseMode(bool reversed_mode)
{
	if (GetLevelClockTime() < 3.0) // because the player start could cause a repulsion
		return;

	ParticlePlayer * player_particle = GetPlayerParticle(0);
	if (player_particle != nullptr)
	{
		if (player_particle->reversed == reversed_mode) // no change, ignore
			return;
	
		if (reversed_mode) // only trigger a 'pulse' if no cooldown
		{
			if (current_cooldown > 0.0f)
				return;
			current_cooldown = cooldown;
			PlaySound("collision", false, false);
		}	
		player_particle->reversed = reversed_mode;
	}
}

void LudumGame::SetPlayerDashMode(bool dash)
{
	if (GetLevelClockTime() < 3.0) // because the player start dash
		return;

	ParticlePlayer * player_particle = GetPlayerParticle(0);
	if (player_particle != nullptr)
	{
		if (player_particle->dash == dash) // no change, ignore
			return;

		if (dash) // only trigger a 'pulse' if no cooldown
		{
			if (current_dash_cooldown > 0.0f)
				return;
			current_dash_cooldown = dash_cooldown;
			current_dash_duration = dash_duration;
			PlaySound("thrust", false, false);
		}	
		player_particle->dash = dash;
	}
}

void LudumGame::TickDashValues(double delta_time)
{
	// cooldow in progress
	if (current_dash_cooldown > 0.0f)
		current_dash_cooldown = chaos::MathTools::Maximum(0.0f, current_dash_cooldown - (float)delta_time);
	if (current_dash_duration > 0.0f)
	{
		current_dash_duration = chaos::MathTools::Maximum(0.0f, current_dash_duration - (float)delta_time);
		if (current_dash_duration == 0.0f)
			SetPlayerDashMode(false);	
	}
}


void LudumGame::RegisterEnemiesInRange(glm::vec2 const & center, float radius, std::vector<ParticleEnemy> & enemy_particles, char const * layer_name, bool take_all)
{
	// capture all Enemies in range
	LudumLevelInstance const * level_instance = dynamic_cast<LudumLevelInstance const *>(GetCurrentLevelInstance());
	if (level_instance != nullptr)
	{
		death::TiledMap::LayerInstance const * layer_instance = level_instance->FindLayerInstance(layer_name);
		if (layer_instance)
		{
			chaos::ParticleLayer const * particle_layer = layer_instance->GetParticleLayer();
			if (particle_layer != nullptr)
			{
				float square_radius = radius * radius;

				size_t count = particle_layer->GetAllocationCount();
				for (size_t i = 0 ; i < count ; ++i)
				{
					chaos::ParticleAllocation const * allocation = particle_layer->GetAllocation(i);
					if (allocation == nullptr)
						continue;

					chaos::ParticleConstAccessor<ParticleEnemy> enemies = allocation->GetParticleAccessor<ParticleEnemy>();

					size_t enemies_count = enemies.GetCount();
					for (size_t j = 0 ; j < enemies_count ; ++j)
					{
						ParticleEnemy const & enemy = enemies[j];
						if (!take_all)
						{
							float l2 = glm::length2(enemy.bounding_box.position - center);
							if (l2 > square_radius)
								continue;						
						}
						enemy_particles.push_back(enemy);									
					}				
				}
			}		
		}	
	}
}

void LudumGame::NotifyAtomCountChange(int delta)
{
	if (delta > 0)
		PlaySound("particle_add", false, false);
	else if (delta < 0)
		PlaySound("particle_removed", false, false);
	waken_up_particle_count += delta;
}

float LudumGame::GetPlayerLife(int player_index) const
{
	ParticlePlayer const * player_particle = GetPlayerParticle(player_index);
	if (player_particle == nullptr)
		return 0.0f;
	return player_particle->life;
}

ParticlePlayer * LudumGame::GetPlayerParticle(int player_index)
{
	chaos::ParticleAllocation * player_allocations = GetPlayerAllocation(player_index);
	if (player_allocations == nullptr)
		return nullptr;
	chaos::ParticleAccessor<ParticlePlayer> player_particles = player_allocations->GetParticleAccessor<ParticlePlayer>();
	if (player_particles.GetCount() == 0)
		return nullptr;
	return &player_particles[0];
}

ParticlePlayer const * LudumGame::GetPlayerParticle(int player_index) const
{
	chaos::ParticleAllocation const * player_allocations = GetPlayerAllocation(player_index);
	if (player_allocations == nullptr)
		return nullptr;
	chaos::ParticleConstAccessor<ParticlePlayer> player_particles = player_allocations->GetParticleAccessor<ParticlePlayer>();
	if (player_particles.GetCount() == 0)
		return nullptr;
	return &player_particles[0];
}

chaos::box2 LudumGame::GetWorldBox() const
{
	if (current_level_instance != nullptr)
		 return current_level_instance->GetBoundingBox();
	return chaos::box2();
}


bool LudumGame::GenerateFramebuffer(glm::ivec2 const & size, chaos::shared_ptr<chaos::GPUFramebuffer> & in_framebuffer)
{
	if (in_framebuffer != nullptr)
	{
		if (size == in_framebuffer->GetSize())
			return true;	
	}

	chaos::GPUFramebufferGenerator framebuffer_generator;
	framebuffer_generator.AddColorAttachment(0, chaos::PixelFormat::GetPixelFormat<chaos::PixelBGRA>(), glm::ivec2(0, 0), "scene");

	in_framebuffer = framebuffer_generator.GenerateFramebuffer(size);

	if (in_framebuffer == nullptr)
		return false;
	if (!in_framebuffer->CheckCompletionStatus())
		return false;

	return true;
}

void LudumGame::SetPlayerAllocation(int player_index, chaos::ParticleAllocation * in_allocation) 
{ 
	death::Game::SetPlayerAllocation(player_index, in_allocation);
	if (in_allocation != nullptr)
	{
		chaos::ParticleAccessor<ParticlePlayer> player_particles = in_allocation->GetParticleAccessor<ParticlePlayer>();
		if (player_particles.GetCount() > 0)
			player_particles[0].life = initial_player_life;
	}
}