#include "Ludum46Game.h"
#include "Ludum46Particles.h"
#include "Ludum46Level.h"
#include "Ludum46LevelInstance.h"
#include "Ludum46HUD.h"
#include "Ludum46Player.h"
#include "Ludum46GameInstance.h"

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
	// 	Don't let the Flames of Hell die

	game_name = "Burnaouf Paradise\nVII";
	game_instructions = R"INSTRUCTIONS(
	[ButtonA] or [KEYBOARD SPACE] : Jump
	[ButtonB] or [KEYBOARD CTRL]  : Attack
	[RightTrigger] or [KEYBOARD SHIFT] : Run)INSTRUCTIONS";

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
	chaos::ClassTools::DeclareClass<ParticleAnimated, ParticleBase>("ParticleAnimated");
	chaos::ClassTools::DeclareClass<ParticlePlayer, ParticleAnimated>("ParticlePlayer");
	chaos::ClassTools::DeclareClass<ParticleFire, ParticleBase>("ParticleFire");
	chaos::ClassTools::DeclareClass<ParticleSoul, ParticleBase>("ParticleSoul");
	chaos::ClassTools::DeclareClass<ParticleBlood, ParticleAnimated>("ParticleBlood");
	chaos::ClassTools::DeclareClass<ParticleBurnedSoul, ParticleBlood>("ParticleBurnedSoul");
	
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

death::GameInstance * LudumGame::DoCreateGameInstance()
{
	return new LudumGameInstance(this);
}

void LudumGame::DoDisplayGame(chaos::GPURenderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::GPURenderParams const & render_params)
{
	death::Game::DoDisplayGame(renderer, uniform_provider, render_params);


	// Fadeout

#if 1

	LudumLevelInstance const* ludum_level_instance = GetLevelInstance();
	if (ludum_level_instance != nullptr)
	{
		if (ludum_level_instance->completion_timer > 0.0f && ludum_level_instance->completion_delay > 0.0f)
		{
			chaos::GPUResourceManager* resource_manager = chaos::MyGLFW::SingleWindowApplication::GetGPUResourceManagerInstance();
			if (resource_manager == nullptr)
				return;

			chaos::GPURenderMaterial* blackscreen_material = resource_manager->FindRenderMaterial("blackscreen");
			if (blackscreen_material != nullptr)
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				chaos::GPUProgramProviderChain blackscreen_provider(uniform_provider);
				blackscreen_provider.AddVariableValue("fade_ratio", 1.0f - (ludum_level_instance->completion_timer / ludum_level_instance->completion_delay));

				renderer->DrawFullscreenQuad(blackscreen_material, &blackscreen_provider, render_params);

				glDisable(GL_BLEND);
			}
		}
	}
#endif
}




















