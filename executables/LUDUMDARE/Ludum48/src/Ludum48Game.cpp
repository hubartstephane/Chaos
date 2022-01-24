#include <chaos/Chaos.h>

#include "Ludum48Game.h"
#include "Ludum48Particles.h"
#include "Ludum48Level.h"
#include "Ludum48LevelInstance.h"
#include "Ludum48HUD.h"
#include "Ludum48Player.h"
#include "Ludum48GameInstance.h"

LudumGame::LudumGame()
{		
	// 	Don't let the Flames of Hell die

	game_name = "Bouldaouf Dash\n";
	game_instructions = R"INSTRUCTIONS(
Move:                 Dpad or Arrows
Move camera:         Right Stick
Fake displacement: [ButtonA] or Control
Suicide:            [ButtonY] or Space)INSTRUCTIONS";

	looping_levels = false;

	game_instance_class = LudumGameInstance::GetStaticClass();
}

bool LudumGame::OnEnterGame(PhysicalGamepad * in_physical_gamepad)
{
	if (!Game::OnEnterGame(in_physical_gamepad))
		return false;
    PlaySound("start", false, false, 0.0f, SoundContext::GAME);
	return true;
}


GameHUD * LudumGame::DoCreatePlayingHUD()
{
	return new LudumPlayingHUD();
}

bool LudumGame::InitializeGameValues(nlohmann::json const & config, bool hot_reload)
{
	if (!Game::InitializeGameValues(config, hot_reload))
		return false;
						
	return true;
}

TMLevel * LudumGame::CreateTMLevel()
{
	return new LudumLevel();
}

bool LudumGame::InitializeFromConfiguration(nlohmann::json const & config)
{
	if (!Game::InitializeFromConfiguration(config))
		return false;





	return true;
}


void LudumGame::DoDisplayGame(GPURenderer* renderer, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params)
{
	GPUProgramProviderChain update_provider(uniform_provider);

	Game::DoDisplayGame(renderer, &update_provider, render_params);


	// Win Fadeout to white
	LudumPlayer* player = GetPlayer(0);
	if (player != nullptr)
	{

		if (player->cheater_farid)
		{
			ParticleTextGenerator::GeneratorParams params;
			params.line_height = 80.0f;
			params.hotpoint = Hotpoint::BOTTOM;
			params.position.y = -300.0f;

			GPUDrawInterface<VertexDefault> DI(nullptr);
			DrawText(DI, "[KISS] Cheater Farid [KISS]", params);


			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			DI.Display(renderer, uniform_provider, render_params);
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);

			
		}

		LudumLevelInstance const * li = GetLevelInstance();






		

		if (player->death_timer >= 0.0f)
		{
			SetFadeEffect(renderer, uniform_provider, render_params, false, 1.0f - (player->death_timer / player->max_death_timer));
		}
		else if (player->suicidal_timer >= 0.0f)
		{
			SetFadeEffect(renderer, uniform_provider, render_params, false, (player->suicidal_timer / player->max_suicidal_timer));
		}
		else if (li != nullptr && li->level_complete)
		{
			SetFadeEffect(renderer, uniform_provider, render_params, true, 1.0f - (li->completion_timer / li->completion_delay));

		}


	}
}

void LudumGame::SetFadeEffect(GPURenderer* renderer, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params, bool fade_to_black, float ratio)
{
	GPUResourceManager* resource_manager = WindowApplication::GetGPUResourceManagerInstance();
	if (resource_manager == nullptr)
		return;

	GPURenderMaterial* fade_material = resource_manager->FindRenderMaterial(fade_to_black? "blackscreen" : "whitescreen");
	if (fade_material != nullptr)
	{
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		GPUProgramProviderChain fade_provider(uniform_provider);
		fade_provider.AddVariable("fade_ratio", ratio);

		renderer->DrawFullscreenQuad(fade_material, &fade_provider, render_params);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
}




























