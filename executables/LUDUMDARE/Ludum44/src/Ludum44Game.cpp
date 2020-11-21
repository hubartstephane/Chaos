#include <chaos/Chaos.h>

#include "Ludum44Game.h"
#include "Ludum44Particles.h"
#include "Ludum44Level.h"
#include "Ludum44LevelInstance.h"
#include "Ludum44HUD.h"
#include "Ludum44Player.h"
#include "Ludum44GameInstance.h"

LudumGame::LudumGame()
{		
	game_name = "Kill Paouf V";
	game_instructions = R"INSTRUCTIONS(
	[ButtonA] or [KEYBOARD SPACE] : Fire
	[ButtonB] or [KEYBOARD CTRL]  : Charged fire
	[ButtonY] or [KEYBOARD ALT]   : Trade POWER-UP and LIFE)INSTRUCTIONS";

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

template<typename T>
static bool InitializeGameValueVector(char const * json_name, nlohmann::json const & config, boost::filesystem::path const & config_path, std::vector<T> & result)
{
	chaos::JSONTools::GetAttribute(config, json_name, result);
	return (result.size() > 0);
}

bool LudumGame::InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path, bool hot_reload)
{
	if (!chaos::Game::InitializeGameValues(config, config_path, hot_reload))
		return false;
	if (!InitializeGameValueVector("player_speeds", config, config_path, player_speeds))
		return false;
	if (!InitializeGameValueVector("player_damages", config, config_path, player_damages))
		return false;
	if (!InitializeGameValueVector("player_charged_damages", config, config_path, player_charged_damages))
		return false;
	if (!InitializeGameValueVector("player_fire_rates", config, config_path, player_fire_rates))
		return false;
	
	CHAOS_JSON_ATTRIBUTE(config, min_player_max_health);
	CHAOS_JSON_ATTRIBUTE(config, player_speed_factor);
	CHAOS_JSON_ATTRIBUTE(config, buy_upgrade_time);
	CHAOS_JSON_ATTRIBUTE(config, charged_fire_time);
	CHAOS_JSON_ATTRIBUTE(config, normal_fire_time);	
	CHAOS_JSON_ATTRIBUTE(config, scroll_factor);
	CHAOS_JSON_ATTRIBUTE(config, fire_velocity);
	CHAOS_JSON_ATTRIBUTE(config, enemy_fire_velocity);
	CHAOS_JSON_ATTRIBUTE(config, enemy_fire_damage);
	CHAOS_JSON_ATTRIBUTE(config, enemy_fire_rate);

	CHAOS_JSON_ATTRIBUTE(config, meteor_health);
	CHAOS_JSON_ATTRIBUTE(config, alien_health);
	CHAOS_JSON_ATTRIBUTE(config, turret_health);

	CHAOS_JSON_ATTRIBUTE(config, meteor_damage_for_player);
	CHAOS_JSON_ATTRIBUTE(config, alien_damage_for_player);
	CHAOS_JSON_ATTRIBUTE(config, turret_damage_for_player);
			
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
	if (!PopulatePowerUps(config, config_path))
		return false;

	return true;
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
