#include "Ludum43Game.h"
#include "Ludum43Particles.h"
#include "Ludum43Level.h"
#include "Ludum43LevelInstance.h"
#include "Ludum43HUD.h"
#include "Ludum43Player.h"
#include "Ludum43GameInstance.h"

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
#include <chaos/GPURenderParams.h>

#include <death/GameParticles.h>
#include <death/SoundContext.h>

DEATH_GAMEFRAMEWORK_IMPLEMENT_GAME(Ludum);

LudumGame::LudumGame()
{
	game_name = "Quantic Paouf IV";
	game_instructions = R"INSTRUCTIONS(
Save particles to the level exit.
Black holes and Shadows are dangerous.
[ButtonA] Turbo
[ButtonB] Ejects particles)INSTRUCTIONS";
}

bool LudumGame::OnEnterGame(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad)
{
	if (!death::Game::OnEnterGame(in_physical_gamepad))
		return false;
	Play("start", false, false, 0.0f, death::SoundContext::GAME);
	return true;
}

void LudumGame::DoDisplayGame(chaos::GPURenderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::GPURenderParams const & render_params)
{
	death::TiledMap::LevelInstance * ludum_level_instance = GetLudumLevelInstance();

	// -------------------------------------
	// I/ no level rendering like main menu (the background)
	// -------------------------------------

	if (ludum_level_instance == nullptr)
	{
		if (particle_manager != nullptr)
			particle_manager->Display(renderer, uniform_provider, render_params);
		return;
	}

	// -------------------------------------
	// II/ offscreen rendering + composition
	// -------------------------------------

	chaos::GPUResourceManager * resource_manager = chaos::MyGLFW::SingleWindowApplication::GetGPUResourceManagerInstance();
	if (resource_manager == nullptr)
		return;

	chaos::GPURenderMaterial * postprocess_material = resource_manager->FindRenderMaterial("postprocess");
	if (postprocess_material == nullptr)
		return;

	// create the render targets or reuse olds
	glm::ivec2 viewport_size = chaos::GLMTools::RecastVector<glm::ivec2>(render_params.viewport.half_size * 2.0f);
	if (!GenerateFramebuffer(viewport_size, framebuffer_deformed) || !GenerateFramebuffer(viewport_size, framebuffer_worldlimits))
		return;

	// clear the color buffers
	glm::vec4 clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	// try to capture the "WorldBounds layer"
	// XXX : in menu, there is no current_level_instance, so we can test the pointer to exaclty know what to do
	//

	// RENDER TARGET 1 : SPECIAL WorldLimits (on red channel), Enlarged enemies (on blue channel)
	{

		renderer->PushFramebufferRenderContext(framebuffer_worldlimits.get(), true);

		glViewport(0, 0, viewport_size.x, viewport_size.y);

		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		// ---------------------------------------------
		// World limits on RED
		// ---------------------------------------------
		{
			chaos::GPURenderParams other_render_params = render_params;
			other_render_params.renderpass_name = "WORLD_LIMITS_ONLY";

#if 0
			chaos::DisableReferenceCount<chaos::GPUParticleLayerFilterList> filter;
			filter.name_filter.enable_names.push_back("WorldLimits");
			other_render_params.object_filter = &filter;
#endif

			glColorMask(true, false, false, true);
			ludum_level_instance->Display(renderer, uniform_provider, other_render_params);
		}

		// ---------------------------------------------
		// (enlarged) Enemies on GREEN  (position_blend_ratio => enlarged for enemies)
		//  BLACK => greatest deformation, white => smallest deformation
		// ---------------------------------------------
		{
			chaos::GPURenderParams other_render_params = render_params;
			other_render_params.renderpass_name = "ENEMIES_ONLY";

#if 0
			chaos::DisableReferenceCount<chaos::GPUParticleLayerFilterList> filter;
			filter.name_filter.enable_names.push_back("Enemies");
			other_render_params.object_filter = &filter;
#endif
			
			chaos::GPUProgramProviderChain enlarged_provider(uniform_provider);
			enlarged_provider.AddVariableValue("position_blend_ratio", 0.0f);

			glColorMask(false, true, false, true);
			ludum_level_instance->Display(renderer, &enlarged_provider, other_render_params);
			glColorMask(true, true, true, true);
		}

		renderer->PopFramebufferRenderContext();
	}

	// ---------------------------------------------
	// RENDER TARGET 2 : all objects that are to be deformed (except Enemies and Player and atoms)
	// ---------------------------------------------
	{
		renderer->PushFramebufferRenderContext(framebuffer_deformed.get(), true);

		glViewport(0, 0, viewport_size.x, viewport_size.y);

		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		chaos::GPURenderParams other_render_params = render_params;
		other_render_params.renderpass_name = "DEFORMED_OBJECT";

#if 0
		chaos::DisableReferenceCount<chaos::GPUParticleLayerFilterList> filter;
		filter.name_filter.forbidden_names.push_back("Enemies");
		filter.name_filter.forbidden_names.push_back("Atoms");
		filter.name_filter.forbidden_names.push_back("PlayerAndCamera");
		filter.name_filter.forbidden_names.push_back("WorldLimits");
		filter.name_filter.forbidden_names.push_back("Texts");
		other_render_params.object_filter = &filter;
#endif

		// draw particle system (the background)
		if (particle_manager != nullptr)
			particle_manager->Display(renderer, uniform_provider, other_render_params);
		current_level_instance->Display(renderer, uniform_provider, other_render_params);

		renderer->PopFramebufferRenderContext();
	}

	// ---------------------------------------------
	// COMBINE STEP 1 & STEP 2 (blend_backgrounds = 1 for default rendering, 0 for texture combining)
	// ---------------------------------------------
	{
		chaos::GPURenderParams other_rendering_params = render_params;

		chaos::GLTools::SetViewport(other_rendering_params.viewport);

		chaos::GPUProgramProviderChain main_provider(uniform_provider);

		chaos::GPUFramebufferAttachmentInfo const * attachment_worldlimits = framebuffer_worldlimits->GetColorAttachment(0);
		if (framebuffer_worldlimits != nullptr)
		{
			chaos::GPUTexture * texture = attachment_worldlimits->texture.get();
			if (texture != nullptr)
				main_provider.AddVariableValue("worldlimits", texture);
		}

		chaos::GPUFramebufferAttachmentInfo const * attachment_deformed = framebuffer_deformed->GetColorAttachment(0);
		if (attachment_deformed != nullptr)
		{
			chaos::GPUTexture * texture = attachment_deformed->texture.get();
			if (texture != nullptr)
				main_provider.AddVariableValue("deformed", texture);
		}

		renderer->DrawFullscreenQuad(postprocess_material, &main_provider, other_rendering_params);
	}

	// ---------------------------------------------
	// simply render player and ennemies
	// ---------------------------------------------
	{
		chaos::GPURenderParams other_rendering_params = render_params;
		other_rendering_params.renderpass_name = "UNDEFORMED_OBJECT";

#if 0
		chaos::DisableReferenceCount<chaos::GPUParticleLayerFilterList> filter;
		filter.name_filter.enable_names.push_back("Enemies");
		filter.name_filter.enable_names.push_back("Atoms");
		filter.name_filter.enable_names.push_back("PlayerAndCamera");
		filter.name_filter.enable_names.push_back("Texts");
		other_rendering_params.object_filter = &filter;
#endif

		// draw particle system (the background)
		current_level_instance->Display(renderer, uniform_provider, other_rendering_params);
	}
}

void LudumGame::OnInputModeChanged(int new_mode, int old_mode)
{

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

void LudumGame::RegisterEnemiesInRange(glm::vec2 const & center, float radius, std::vector<ParticleEnemy> & enemy_particles, char const * layer_name, bool take_all)
{
	// capture all Enemies in range
	LudumLevelInstance const * level_instance = GetLudumLevelInstance();
	if (level_instance != nullptr)
	{
		death::TiledMap::LayerInstance const * layer_instance = level_instance->FindLayerInstance(layer_name);
		if (layer_instance)
		{
			chaos::ParticleLayerBase const * particle_layer = layer_instance->GetParticleLayer();
			if (particle_layer != nullptr)
			{
				float square_radius = radius * radius;

				size_t count = particle_layer->GetAllocationCount();
				for (size_t i = 0; i < count; ++i)
				{
					chaos::ParticleAllocationBase const * allocation = particle_layer->GetAllocation(i);
					if (allocation == nullptr)
						continue;

					chaos::ParticleConstAccessor<ParticleEnemy> enemies = allocation->GetParticleAccessor<ParticleEnemy>();

					size_t enemies_count = enemies.GetCount();
					for (size_t j = 0; j < enemies_count; ++j)
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

void LudumGame::OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance)
{
	// super method
	death::Game::OnLevelChanged(new_level, old_level, new_level_instance);

	// play a sound
	if (new_level != nullptr && old_level != nullptr)
		Play("next_level", false, false, 0.0f, death::SoundContext::GAME);
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
	LudumPlayer * player = GetLudumPlayer(player_index);
	if (player == nullptr)
		return nullptr;
	return player->GetPlayerParticle();
}

ParticlePlayer const * LudumGame::GetPlayerParticle(int player_index) const
{
	LudumPlayer const * player = GetLudumPlayer(player_index);
	if (player == nullptr)
		return nullptr;
	return player->GetPlayerParticle();
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

death::GameInstance * LudumGame::CreateGameInstance()
{
	return new LudumGameInstance(this);
}
