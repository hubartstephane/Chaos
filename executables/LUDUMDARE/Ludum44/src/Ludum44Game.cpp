#include "Ludum44Game.h"
#include "Ludum44Particles.h"
#include "Ludum44Level.h"
#include "Ludum44LevelInstance.h"
#include "Ludum44HUD.h"
#include "Ludum44Player.h"
#include "Ludum44GameInstance.h"

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

DEATH_GAMEFRAMEWORK_IMPLEMENT_GAME(Ludum);

LudumGame::LudumGame()
{		
	game_name = "Quantic Paouf V";
	//game_instructions = R"INSTRUCTIONS(
	//)INSTRUCTIONS";
}

bool LudumGame::OnEnterGame(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad)
{
	if (!death::Game::OnEnterGame(in_physical_gamepad))
		return false;
	PlaySound("start", false, false);
	return true;
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
	return true;
}

static GameValue InitializeGameValue(char const * json_name, nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	GameValue result;

	std::string initial_json_name = std::string("initial_") + json_name;
	chaos::JSONTools::GetAttribute(config, initial_json_name.c_str(), result.initial_value);
	std::string max_json_name = std::string("max_") + json_name;
	chaos::JSONTools::GetAttribute(config, max_json_name.c_str(), result.max_value);
	std::string increment_json_name = std::string("increment_") + json_name;
	chaos::JSONTools::GetAttribute(config, increment_json_name.c_str(), result.increment_value);

	return result;
}

bool LudumGame::InitializeParticleTextGenerator()
{
	if (!death::Game::InitializeParticleTextGenerator())
		return false;
	particle_text_generator->AddColor("POWERUP", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	particle_text_generator->AddColor("KEYBOARD", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	particle_text_generator->AddColor("LEVELTITLE", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	return true;
}


bool LudumGame::InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path, bool hot_reload)
{
	if (!death::Game::InitializeGameValues(config, config_path, hot_reload))
		return false;

	player_life = InitializeGameValue("player_life", config, config_path);
	player_speed = InitializeGameValue("player_speed", config, config_path);
	player_damage = InitializeGameValue("player_damage", config, config_path);
	player_charged_damage = InitializeGameValue("player_charged_damage", config, config_path);
	player_fire_rate = InitializeGameValue("player_fire_rate", config, config_path);
	
	DEATHGAME_JSON_ATTRIBUTE(min_player_max_life);
	DEATHGAME_JSON_ATTRIBUTE(player_speed_factor);
	DEATHGAME_JSON_ATTRIBUTE(buy_upgrade_time);
	DEATHGAME_JSON_ATTRIBUTE(charged_fire_time);
	DEATHGAME_JSON_ATTRIBUTE(scroll_factor);
	DEATHGAME_JSON_ATTRIBUTE(fire_velocity);
		
	return true;
}

death::TiledMap::Level * LudumGame::CreateTiledMapLevel()
{
	return new LudumLevel();
}

bool LudumGame::InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	if (!death::Game::InitializeFromConfiguration(config, config_path))
		return false;
	if (!PopulatePowerUps(config, config_path))
		return false;
	return true;
}

void LudumGame::OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance)
{
	// super method
	death::Game::OnLevelChanged(new_level, old_level, new_level_instance);

	// play a sound
	if (new_level != nullptr && old_level != nullptr)
		PlaySound("next_level", false, false);
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

death::GameInstance * LudumGame::CreateGameInstance()
{
	return new LudumGameInstance(this);
}

void LudumGame::DoDisplayGame(chaos::Renderer * renderer, chaos::GPUProgramProvider * uniform_provider, chaos::RenderParams const & render_params)
{

//	chaos::RenderParams other_render_params = render_params;
//	other_render_params.submaterial_name = "titi";
//	death::Game::DoDisplayGame(renderer, uniform_provider, other_render_params);
	death::Game::DoDisplayGame(renderer, uniform_provider, render_params);
}

bool LudumGame::PopulatePowerOneUp(LudumPowerUp * power_up, char const * json_name, nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	if (!power_up->InitializeFromConfiguration(json_name, config, config_path))
	{
		delete(power_up);
		return false;
	}
	power_ups.push_back(power_up);
	return true;
}

bool LudumGame::PopulatePowerUps(nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	if (!PopulatePowerOneUp(new LudumSpeedUp("SpeedUp"), "speed_up", config, config_path))
		return false;
	if (!PopulatePowerOneUp(new LudumDamageUp("PowerUp", false), "damage_up", config, config_path))
		return false;
	if (!PopulatePowerOneUp(new LudumDamageUp("PowerUp", true), "charged_damage_up", config, config_path))
		return false;
	if (!PopulatePowerOneUp(new LudumFireRateUp("FireRateIncreased"), "fire_rate_up", config, config_path))
		return false;
	return true;
}

int LudumGame::AddParticleLayers()
{
	int render_order = death::Game::AddParticleLayers();
	if (render_order < 0)
		return render_order;

	return render_order;
}