#include <chaos/Chaos.h>

#include "Ludum47Game.h"
#include "Ludum47Particles.h"
#include "Ludum47Level.h"
#include "Ludum47LevelInstance.h"
#include "Ludum47HUD.h"
#include "Ludum47Player.h"
#include "Ludum47GameInstance.h"

LudumGame::LudumGame()
{		
	// 	Don't let the Flames of Hell die
	game_name = "Aouf Run";
	game_instructions = R"INSTRUCTIONS(
[ButtonA] or [KEYBOARD SPACE]       : Accelerate
[ButtonB] or [KEYBOARD CTRL]        : Break
[ButtonX] or [KEYBOARD LEFT SHIFT] : Special)INSTRUCTIONS";

	looping_levels = false;

	game_instance_class = LudumGameInstance::GetStaticClass();
}

bool LudumGame::OnEnterGame(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad)
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

void LudumGame::DoDisplayGame(chaos::GPURenderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::GPURenderParams const & render_params)
{
	chaos::GPUProgramProviderChain update_provider(uniform_provider);

	LudumLevelInstance const* ludum_level_instance = GetLevelInstance();
	if (ludum_level_instance != nullptr)
	{
	


	}
	

	chaos::Game::DoDisplayGame(renderer, &update_provider, render_params);
}




















