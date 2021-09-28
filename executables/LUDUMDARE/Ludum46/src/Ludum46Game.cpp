#include <chaos/Chaos.h>

#include "Ludum46Game.h"
#include "Ludum46Particles.h"
#include "Ludum46Level.h"
#include "Ludum46LevelInstance.h"
#include "Ludum46HUD.h"
#include "Ludum46Player.h"
#include "Ludum46GameInstance.h"

LudumGame::LudumGame()
{		
	// 	Don't let the Flames of Hell die

	game_name = "Burnaouf Paradise\nVII";
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

bool LudumGame::InitializeGameValues(nlohmann::json const & config, bool hot_reload)
{
	if (!chaos::Game::InitializeGameValues(config, hot_reload))
		return false;
						
	return true;
}

chaos::TMLevel * LudumGame::CreateTMLevel()
{
	return new LudumLevel();
}

bool LudumGame::InitializeFromConfiguration(nlohmann::json const & config)
{
	if (!chaos::Game::InitializeFromConfiguration(config))
		return false;





	return true;
}

void LudumGame::DoDisplayGame(chaos::GPURenderer * renderer, chaos::GPUProgramProviderBase const * uniform_provider, chaos::GPURenderParams const & render_params)
{
	chaos::GPUProgramProviderChain update_provider(uniform_provider);

	LudumLevelInstance const* ludum_level_instance = GetLevelInstance();
	if (ludum_level_instance != nullptr)
	{
		float health = ludum_level_instance->flame_health;
		float max_health = ludum_level_instance->flame_initial_health;

		int burning_decrease_step = (int)(12.0 * (max_health - health) / max_health);

		update_provider.AddVariable("burning_decrease_step", burning_decrease_step);
	}
	

	chaos::Game::DoDisplayGame(renderer, &update_provider, render_params);


	// Win Fadeout to white
	if (ludum_level_instance != nullptr)
	{
		if (ludum_level_instance->completion_timer > 0.0f && ludum_level_instance->completion_delay > 0.0f)
		{
			chaos::GPUResourceManager* resource_manager = chaos::WindowApplication::GetGPUResourceManagerInstance();
			if (resource_manager == nullptr)
				return;

			chaos::GPURenderMaterial* blackscreen_material = resource_manager->FindRenderMaterial("blackscreen");
			if (blackscreen_material != nullptr)
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				chaos::GPUProgramProviderChain blackscreen_provider(uniform_provider);
				blackscreen_provider.AddVariable("fade_ratio", 1.0f - (ludum_level_instance->completion_timer / ludum_level_instance->completion_delay));

				renderer->DrawFullscreenQuad(blackscreen_material, &blackscreen_provider, render_params);

				glDisable(GL_BLEND);
			}
		}
	}
}




















