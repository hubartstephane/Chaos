#include <chaos/Chaos.h>

#include "Ludum49Game.h"
#include "Ludum49Particles.h"
#include "Ludum49Level.h"
#include "Ludum49LevelInstance.h"
#include "Ludum49HUD.h"
#include "Ludum49Player.h"
#include "Ludum49GameInstance.h"

LudumGame::LudumGame()
{		
	// 	Don't let the Flames of Hell die
	game_name = "No Game Today\n(but have a look)";
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

void LudumGame::DoDisplayGame(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params)
{
	GPUProgramProviderChain update_provider(uniform_provider);

	LudumLevelInstance const* ludum_level_instance = GetLevelInstance();
	if (ludum_level_instance != nullptr)
	{
	


	}
	

	Game::DoDisplayGame(renderer, &update_provider, render_params);
}




















