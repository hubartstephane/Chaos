#include "LudumTestGame.h"
#include "LudumTestParticles.h"
#include "LudumTestLevel.h"
#include "LudumTestLevelInstance.h"
#include "LudumTestHUD.h"
#include "LudumTestPlayer.h"
#include "LudumTestGameInstance.h"

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

LudumGame::LudumGame(GLFWwindow* in_glfw_window) :
	death::Game(in_glfw_window)
{		
	game_name = "Hello World";
	game_instructions = R"INSTRUCTIONS(
	[ButtonA] or [KEYBOARD SPACE] : Fire
	[ButtonB] or [KEYBOARD CTRL]  : Dash
	[ButtonY] or [KEYBOARD ALT]   : Bright side of life)INSTRUCTIONS";

	looping_levels = false;
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
	chaos::ClassTools::DeclareClass<ParticlePlayer,ParticleBase>("ParticlePlayer");
	
	return true;
}

bool LudumGame::InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path, bool hot_reload)
{
	if (!death::Game::InitializeGameValues(config, config_path, hot_reload))
		return false;
						
	return true;
}

death::TiledMapLevel * LudumGame::CreateTiledMapLevel()
{
	return new LudumLevel();
}

bool LudumGame::InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	if (!death::Game::InitializeFromConfiguration(config, config_path))
		return false;





	return true;
}

ParticlePlayer * LudumGame::GetPlayerParticle(size_t player_index)
{
	LudumPlayer * player = GetPlayer(player_index);
	if (player == nullptr)
		return nullptr;
	return player->GetPlayerParticle();
}

ParticlePlayer const * LudumGame::GetPlayerParticle(size_t player_index) const
{
	LudumPlayer const * player = GetPlayer(player_index);
	if (player == nullptr)
		return nullptr;
	return player->GetPlayerParticle();
}

death::GameInstance * LudumGame::DoCreateGameInstance()
{
	return new LudumGameInstance(this);
}

void LudumGame::DoDisplayGame(chaos::GPURenderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::GPURenderParams const & render_params)
{
	death::Game::DoDisplayGame(renderer, uniform_provider, render_params);

}




















