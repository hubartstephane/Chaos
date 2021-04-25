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

	game_name = "Burnaouf Paradise\n";
	game_instructions = R"INSTRUCTIONS(
	[ButtonA] or [KEYBOARD SPACE] : Jump
	[ButtonB] or [KEYBOARD CTRL]  : Attack
	[RightTrigger] or [KEYBOARD SHIFT] : Run)INSTRUCTIONS";

	looping_levels = false;

	game_instance_class = LudumGameInstance::GetStaticClass();
}

bool LudumGame::OnEnterGame(chaos::PhysicalGamepad * in_physical_gamepad)
{
	if (!chaos::Game::OnEnterGame(in_physical_gamepad))
		return false;
    PlaySound("start", false, false, 0.0f, chaos::SoundContext::GAME);
	return true;
}


chaos::GameHUD * LudumGame::DoCreatePlayingHUD()
{
	return new LudumPlayingHUD();
}

bool LudumGame::InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path, bool hot_reload)
{
	if (!chaos::Game::InitializeGameValues(config, config_path, hot_reload))
		return false;
						
	return true;
}

chaos::TMLevel * LudumGame::CreateTMLevel()
{
	return new LudumLevel();
}

bool LudumGame::InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	if (!chaos::Game::InitializeFromConfiguration(config, config_path))
		return false;





	return true;
}


void LudumGame::DoDisplayGame(chaos::GPURenderer* renderer, chaos::GPUProgramProviderBase const* uniform_provider, chaos::GPURenderParams const& render_params)
{
	chaos::GPUProgramProviderChain update_provider(uniform_provider);

	chaos::Game::DoDisplayGame(renderer, &update_provider, render_params);


	// Win Fadeout to white
	LudumPlayer* player = GetPlayer(0);
	if (player != nullptr)
	{

		if (player->cheater_farid)
		{
			chaos::ParticleTextGenerator::GeneratorParams params;
			params.line_height = 80.0f;
			params.hotpoint = chaos::Hotpoint::BOTTOM;
			params.position.y = -300.0f;

			chaos::GPUDrawInterface<chaos::VertexDefault> DI(nullptr);
			chaos::DrawText(DI, "[KISS] Cheater Farid [KISS]", params);


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

void LudumGame::SetFadeEffect(chaos::GPURenderer* renderer, chaos::GPUProgramProviderBase const* uniform_provider, chaos::GPURenderParams const& render_params, bool fade_to_black, float ratio)
{
	chaos::GPUResourceManager* resource_manager = chaos::WindowApplication::GetGPUResourceManagerInstance();
	if (resource_manager == nullptr)
		return;

	chaos::GPURenderMaterial* fade_material = resource_manager->FindRenderMaterial(fade_to_black? "blackscreen" : "whitescreen");
	if (fade_material != nullptr)
	{
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		chaos::GPUProgramProviderChain fade_provider(uniform_provider);
		fade_provider.AddVariable("fade_ratio", ratio);

		renderer->DrawFullscreenQuad(fade_material, &fade_provider, render_params);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
}




























