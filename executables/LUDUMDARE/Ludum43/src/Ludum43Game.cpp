#include "Ludum43PCH.h"
#include "Ludum43Game.h"
#include "Ludum43Particles.h"
#include "Ludum43Level.h"
#include "Ludum43LevelInstance.h"
#include "Ludum43HUD.h"
#include "Ludum43Player.h"
#include "Ludum43GameInstance.h"

LudumGame::LudumGame()
{
	game_name = "Quantic Paouf IV";
	game_instructions = R"INSTRUCTIONS(
Save particles to the level exit.
Black holes and Shadows are dangerous.
[ButtonA] Turbo
[ButtonB] Ejects particles)INSTRUCTIONS";

	game_instance_class = LudumGameInstance::GetStaticClass();
}

bool LudumGame::OnEnterGame(chaos::PhysicalGamepad * in_physical_gamepad)
{
	if (!chaos::Game::OnEnterGame(in_physical_gamepad))
		return false;
    PlaySound("start", false, false, 0.0f, chaos::SoundContext::GAME);
	return true;
}

void LudumGame::DoDisplayGame(chaos::GPURenderContext * render_context, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::GPURenderParams const & render_params)
{
	chaos::TMLevelInstance * ludum_level_instance = GetLevelInstance();

	// -------------------------------------
	// I/ no level rendering like main menu (the background)
	// -------------------------------------

	if (ludum_level_instance == nullptr)
	{
		DoDisplayBackground(render_context, uniform_provider, render_params);
		if (particle_manager != nullptr)
			particle_manager->Display(render_context, uniform_provider, render_params);
		return;
	}

	// -------------------------------------
	// II/ offscreen rendering + composition
	// -------------------------------------

	chaos::GPUResourceManager * resource_manager = chaos::WindowApplication::GetGPUResourceManagerInstance();
	if (resource_manager == nullptr)
		return;

	chaos::GPURenderMaterial * postprocess_material = resource_manager->FindRenderMaterial("postprocess");
	if (postprocess_material == nullptr)
		return;

	// create the render targets or reuse olds
	glm::ivec2 viewport_size = render_params.viewport.size;
	if (!GenerateFramebuffer(render_context, viewport_size, framebuffer_deformed) || !GenerateFramebuffer(render_context, viewport_size, framebuffer_worldlimits))
		return;

	// clear the color buffers
	glm::vec4 clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	// try to capture the "WorldBounds layer"
	// XXX : in menu, there is no level_instance, so we can test the pointer to exaclty know what to do
	//

	// RENDER TARGET 1 : SPECIAL WorldLimits (on red channel), Enlarged enemies (on blue channel)
	render_context->RenderIntoFramebuffer(framebuffer_worldlimits.get(), true, [this, &clear_color, &render_params, uniform_provider, &render_context, ludum_level_instance]()
	{
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		// ---------------------------------------------
		// World limits on RED
		// ---------------------------------------------
		{
			chaos::GPURenderParams other_render_params = render_params;
			other_render_params.renderpass_name = "WORLD_LIMITS_ONLY";

			glColorMask(true, false, false, true);
			ludum_level_instance->Display(render_context, uniform_provider, other_render_params);
		}

		// ---------------------------------------------
		// (enlarged) Enemies on GREEN  (enlarge_enemy => enlarged for enemies)
		//  BLACK => greatest deformation, white => smallest deformation
		// ---------------------------------------------
		{
			chaos::GPURenderParams other_render_params = render_params;
			other_render_params.renderpass_name = "ENEMIES_ONLY";

			chaos::GPUProgramProviderChain enlarged_provider(uniform_provider);
			enlarged_provider.AddVariable("enlarge_enemy", 1.0f);

			glColorMask(false, true, false, true);
			ludum_level_instance->Display(render_context, &enlarged_provider, other_render_params);
			glColorMask(true, true, true, true);
		}

		return true;
	});

	// ---------------------------------------------
	// RENDER TARGET 2 : all objects that are to be deformed (except Enemies and Player and atoms)
	// ---------------------------------------------

	render_context->RenderIntoFramebuffer(framebuffer_deformed.get(), true, [this, &clear_color, &render_params, uniform_provider, &render_context]()
	{
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		chaos::GPURenderParams other_render_params = render_params;
		other_render_params.renderpass_name = "DEFORMED_OBJECT";

		// display the background
		DoDisplayBackground(render_context, uniform_provider, render_params);
		// draw particle system
		//if (particle_manager != nullptr)
		//	particle_manager->Display(render_context, uniform_provider, other_render_params);
		level_instance->Display(render_context, uniform_provider, other_render_params);

		return true;
	});

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
				main_provider.AddVariable("worldlimits", texture);
		}

		chaos::GPUFramebufferAttachmentInfo const * attachment_deformed = framebuffer_deformed->GetColorAttachment(0);
		if (attachment_deformed != nullptr)
		{
			chaos::GPUTexture * texture = attachment_deformed->texture.get();
			if (texture != nullptr)
				main_provider.AddVariable("deformed", texture);
		}

		render_context->DrawFullscreenQuad(postprocess_material, &main_provider, other_rendering_params);
	}

	// ---------------------------------------------
	// simply render player and ennemies
	// ---------------------------------------------
	{
		chaos::GPURenderParams other_rendering_params = render_params;
		other_rendering_params.renderpass_name = "UNDEFORMED_OBJECT";

		chaos::GPUProgramProviderChain enlarged_provider(uniform_provider);
		enlarged_provider.AddVariable("enlarge_enemy", 0.0f);


		// draw particle system (the background)
		level_instance->Display(render_context, &enlarged_provider, other_rendering_params);
	}
}

chaos::GameHUD * LudumGame::DoCreatePlayingHUD()
{
	return new LudumPlayingHUD();
}

bool LudumGame::OnReadConfigurableProperties(chaos::JSONReadConfiguration config, chaos::ReadConfigurablePropertiesContext context)
{
	if (!chaos::Game::OnReadConfigurableProperties(config, context))
		return false;

	CHAOS_JSON_ATTRIBUTE(config, initial_particle_health);

	CHAOS_JSON_ATTRIBUTE(config, dash_duration);
	CHAOS_JSON_ATTRIBUTE(config, dash_cooldown);
	CHAOS_JSON_ATTRIBUTE(config, dash_velocity);
	CHAOS_JSON_ATTRIBUTE(config, cooldown);

	CHAOS_JSON_ATTRIBUTE(config, player_attraction_minradius);
	CHAOS_JSON_ATTRIBUTE(config, player_attraction_maxradius);
	CHAOS_JSON_ATTRIBUTE(config, player_tangent_force);
	CHAOS_JSON_ATTRIBUTE(config, player_attraction_force);
	CHAOS_JSON_ATTRIBUTE(config, player_repulsion_force);
	CHAOS_JSON_ATTRIBUTE(config, player_slowing_factor);
	CHAOS_JSON_ATTRIBUTE(config, player_max_velocity);
	CHAOS_JSON_ATTRIBUTE(config, player_acceleration);

	CHAOS_JSON_ATTRIBUTE(config, worldlimits_attraction_maxradius_offset);
	CHAOS_JSON_ATTRIBUTE(config, worldlimits_attraction_minradius_offset);
	CHAOS_JSON_ATTRIBUTE(config, enemy_attraction_minradius_offset);
	CHAOS_JSON_ATTRIBUTE(config, enemy_attraction_maxradius_offset);

	CHAOS_JSON_ATTRIBUTE(config, worldlimits_attraction_maxradius_factor);
	CHAOS_JSON_ATTRIBUTE(config, worldlimits_attraction_minradius_factor);
	CHAOS_JSON_ATTRIBUTE(config, enemy_attraction_minradius_factor);
	CHAOS_JSON_ATTRIBUTE(config, enemy_attraction_maxradius_factor);

	CHAOS_JSON_ATTRIBUTE(config, enemy_tangent_force);
	CHAOS_JSON_ATTRIBUTE(config, enemy_attraction_force);
	CHAOS_JSON_ATTRIBUTE(config, enemy_repulsion_force);

	CHAOS_JSON_ATTRIBUTE(config, particle_slowing_factor);
	CHAOS_JSON_ATTRIBUTE(config, particle_min_radius_factor);
	CHAOS_JSON_ATTRIBUTE(config, particle_max_radius_factor);

	CHAOS_JSON_ATTRIBUTE(config, particle_max_velocity);

	CHAOS_JSON_ATTRIBUTE(config, world_clamp_radius);

	return true;
}

chaos::TMLevel * LudumGame::CreateTMLevel()
{
	return new LudumLevel();
}

void LudumGame::RegisterEnemiesInRange(glm::vec2 const & center, float radius, std::vector<ParticleEnemy> & enemy_particles, char const * layer_name, bool take_all)
{
	// capture all Enemies in range
	LudumLevelInstance const * level_instance = GetLevelInstance();
	if (level_instance != nullptr)
	{
		chaos::TMLayerInstance const * layer_instance = level_instance->FindLayerInstance(layer_name);
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

					chaos::ParticleConstAccessor<ParticleEnemy> enemies = allocation->GetParticleAccessor();

					size_t enemies_count = enemies.GetDataCount();
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

bool LudumGame::GenerateFramebuffer(chaos::GPURenderContext * in_gpu_render_context, glm::ivec2 const & size, chaos::shared_ptr<chaos::GPUFramebuffer> & in_framebuffer)
{
	if (in_framebuffer != nullptr)
	{
		if (size == in_framebuffer->GetSize())
			return true;
	}

	chaos::GPUFramebufferGenerator framebuffer_generator(in_gpu_render_context);
	framebuffer_generator.AddColorAttachment(0, chaos::PixelFormat::BGRA, glm::ivec2(0, 0), "scene");

	in_framebuffer = framebuffer_generator.GenerateFramebuffer(size);

	if (in_framebuffer == nullptr)
		return false;
	if (!in_framebuffer->CheckCompletionStatus())
		return false;

	return true;
}
