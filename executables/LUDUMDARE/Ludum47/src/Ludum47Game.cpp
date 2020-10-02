#include "Ludum47Game.h"
#include "Ludum47Particles.h"
#include "Ludum47Level.h"
#include "Ludum47LevelInstance.h"
#include "Ludum47HUD.h"
#include "Ludum47Player.h"
#include "Ludum47GameInstance.h"

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
#include <death/SoundContext.h>

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

bool LudumGame::OnEnterGame(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad)
{
	if (!death::Game::OnEnterGame(in_physical_gamepad))
		return false;
    PlaySound("start", false, false, 0.0f, death::SoundContext::GAME);
	return true;
}


death::GameHUD * LudumGame::DoCreatePlayingHUD()
{
	return new LudumPlayingHUD();
}

bool LudumGame::InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path, bool hot_reload)
{
	if (!death::Game::InitializeGameValues(config, config_path, hot_reload))
		return false;
						
	return true;
}

death::TMLevel * LudumGame::CreateTMLevel()
{
	return new LudumLevel();
}

bool LudumGame::InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	if (!death::Game::InitializeFromConfiguration(config, config_path))
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
	

	death::Game::DoDisplayGame(renderer, &update_provider, render_params);
}




















