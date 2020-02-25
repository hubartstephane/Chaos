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
#include <death/SoundContext.h>

DEATH_GAMEFRAMEWORK_IMPLEMENT_GAME(Ludum);

LudumGame::LudumGame()
{		
	game_name = "Kill Paouf V";
	game_instructions = R"INSTRUCTIONS(
	[ButtonA] or [KEYBOARD SPACE] : Fire
	[ButtonB] or [KEYBOARD CTRL]  : Charged fire
	[ButtonY] or [KEYBOARD ALT]   : Trade POWER-UP and LIFE)INSTRUCTIONS";

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
	chaos::ClassTools::DeclareClass<ParticlePowerUpZone, chaos::ParticleDefault::Particle>("ParticlePowerUpZone");
	chaos::ClassTools::DeclareClass<ParticlePlayer, ParticleBase>("ParticlePlayer");
	chaos::ClassTools::DeclareClass<ParticleEnemy, ParticleBase>("ParticleEnemy");
	chaos::ClassTools::DeclareClass<ParticleFire, ParticleBase>("ParticleFire");
	chaos::ClassTools::DeclareClass<ParticleLife, chaos::ParticleDefault::Particle>("ParticleLife");
	chaos::ClassTools::DeclareClass<ParticleExplosion, chaos::ParticleDefault::Particle>("ParticleExplosion");
	return true;
}

template<typename T>
static bool InitializeGameValueVector(char const * json_name, nlohmann::json const & config, boost::filesystem::path const & config_path, std::vector<T> & result)
{
	chaos::JSONTools::GetAttribute(config, json_name, result);
	return (result.size() > 0);
}

bool LudumGame::InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path, bool hot_reload)
{
	if (!death::Game::InitializeGameValues(config, config_path, hot_reload))
		return false;
	if (!InitializeGameValueVector("player_speeds", config, config_path, player_speeds))
		return false;
	if (!InitializeGameValueVector("player_damages", config, config_path, player_damages))
		return false;
	if (!InitializeGameValueVector("player_charged_damages", config, config_path, player_charged_damages))
		return false;
	if (!InitializeGameValueVector("player_fire_rates", config, config_path, player_fire_rates))
		return false;
	
	DEATHGAME_JSON_ATTRIBUTE(min_player_max_health);
	DEATHGAME_JSON_ATTRIBUTE(player_speed_factor);
	DEATHGAME_JSON_ATTRIBUTE(buy_upgrade_time);
	DEATHGAME_JSON_ATTRIBUTE(charged_fire_time);
	DEATHGAME_JSON_ATTRIBUTE(normal_fire_time);	
	DEATHGAME_JSON_ATTRIBUTE(scroll_factor);
	DEATHGAME_JSON_ATTRIBUTE(fire_velocity);
	DEATHGAME_JSON_ATTRIBUTE(enemy_fire_velocity);
	DEATHGAME_JSON_ATTRIBUTE(enemy_fire_damage);
	DEATHGAME_JSON_ATTRIBUTE(enemy_fire_rate);

	DEATHGAME_JSON_ATTRIBUTE(meteor_health);
	DEATHGAME_JSON_ATTRIBUTE(alien_health);
	DEATHGAME_JSON_ATTRIBUTE(turret_health);

	DEATHGAME_JSON_ATTRIBUTE(meteor_damage_for_player);
	DEATHGAME_JSON_ATTRIBUTE(alien_damage_for_player);
	DEATHGAME_JSON_ATTRIBUTE(turret_damage_for_player);
			
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

void LudumGame::OnLevelChanged(death::Level * new_level, death::Level * old_level, death::LevelInstance * new_level_instance)
{
	// super method
	death::Game::OnLevelChanged(new_level, old_level, new_level_instance);

	// play a sound
	if (new_level != nullptr && old_level != nullptr)
        PlaySound("next_level", false, false, 0.0f, death::SoundContext::GAME);
}

ParticlePlayer * LudumGame::GetPlayerParticle(size_t player_index)
{
	LudumPlayer * player = GetLudumPlayer(player_index);
	if (player == nullptr)
		return nullptr;
	return player->GetPlayerParticle();
}

ParticlePlayer const * LudumGame::GetPlayerParticle(size_t player_index) const
{
	LudumPlayer const * player = GetLudumPlayer(player_index);
	if (player == nullptr)
		return nullptr;
	return player->GetPlayerParticle();
}

chaos::box2 LudumGame::GetWorldBox() const
{
	if (level_instance != nullptr)
		 return level_instance->GetBoundingBox();
	return chaos::box2();
}

death::GameInstance * LudumGame::CreateGameInstance()
{
	return new LudumGameInstance(this);
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
	if (!PopulatePowerOneUp(new LudumSpeedUp("speed_up"), "speed_up", config, config_path))
		return false;
	if (!PopulatePowerOneUp(new LudumDamageUp("power_up", false), "damage_up", config, config_path))
		return false;
	//if (!PopulatePowerOneUp(new LudumDamageUp("blast_improved", true), "charged_damage_up", config, config_path))
	//	return false;
	if (!PopulatePowerOneUp(new LudumFireRateUp("augmented_fire"), "fire_rate_up", config, config_path))
		return false;
	return true;
}
