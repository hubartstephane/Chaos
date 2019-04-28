#include "Ludum44PowerUp.h"
#include "Ludum44Player.h"
#include "Ludum44Game.h"


// =================================================
// LudumPowerUp
// =================================================


bool LudumPowerUp::InitializeFromConfiguration(char const * json_name, nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	std::string cost_json_name = std::string("cost_") + json_name;
	chaos::JSONTools::GetAttribute(config, cost_json_name.c_str(), life_cost);

	return true;
}

bool LudumPowerUp::ApplyPowerUp(LudumGame * game, LudumPlayer * player) const
{
	if (!CanPowerUp(game, player)) // a final test
		return false;
	player->current_max_life -= life_cost;
	if (player->current_life > player->current_max_life)
		player->current_life = player->current_max_life;

	if (!sound_name.empty())
		game->PlaySound(sound_name.c_str(), false, false);
	return true;
}

bool LudumPowerUp::CanPowerUp(LudumGame * game, LudumPlayer * player) const
{
	if (player->current_max_life - life_cost > game->min_player_max_life)
		return true;
	return false;
}

// =================================================
// LudumSpeedUp
// =================================================

bool LudumSpeedUp::ApplyPowerUp(LudumGame * game, LudumPlayer * player) const
{
	if (!LudumPowerUp::ApplyPowerUp(game, player)) // test if still possible to power up (see super method)
		return false;
	++player->current_speed_index;
	return true;
}

bool LudumSpeedUp::CanPowerUp(LudumGame * game, LudumPlayer * player) const
{
	if (!LudumPowerUp::CanPowerUp(game, player))
		return false;
	if (player->current_speed_index >= game->player_speeds.size())
		return false;
	return true;
}

char const * LudumSpeedUp::GetPowerUpTitle() const
{
	return "Speed Up";
}


// =================================================
// LudumDamageUp
// =================================================

bool LudumDamageUp::ApplyPowerUp(LudumGame * game, LudumPlayer * player) const
{
	if (!LudumPowerUp::ApplyPowerUp(game, player)) // test if still possible to power up (see super method)
		return false;

	if (!charged_fire)
		++player->current_damage_index;
	else
		++player->current_charged_damage_index;	
	return true;
}

bool LudumDamageUp::CanPowerUp(LudumGame * game, LudumPlayer * player) const
{
	if (!LudumPowerUp::CanPowerUp(game, player))
		return false;

	if (!charged_fire)
	{
		if (player->current_damage_index >= game->player_damages.size())
			return false;
	}
	else
	{
		if (player->current_charged_damage_index >= game->player_damages.size())
			return false;
	}
	return true;
}

char const * LudumDamageUp::GetPowerUpTitle() const
{
	return "Damage Up";
}

// =================================================
// LudumFireRateUp
// =================================================

bool LudumFireRateUp::ApplyPowerUp(LudumGame * game, LudumPlayer * player) const
{
	if (!LudumPowerUp::ApplyPowerUp(game, player)) // test if still possible to power up (see super method)
		return false;
	++player->current_fire_rate_index;
	return true;
}

bool LudumFireRateUp::CanPowerUp(LudumGame * game, LudumPlayer * player) const
{
	if (!LudumPowerUp::CanPowerUp(game, player))
		return false;
	if (player->current_fire_rate_index >= game->player_fire_rates.size())
		return false;
	return true;
}

char const * LudumFireRateUp::GetPowerUpTitle() const
{
	return "Augmented Fire";
}

