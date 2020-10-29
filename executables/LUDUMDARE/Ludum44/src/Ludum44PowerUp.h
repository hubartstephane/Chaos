#pragma once

#include <chaos/Chaos.h>

#include "Ludum44Player.h"

// =================================================
// LudumPowerUp
// =================================================

class LudumPowerUp : public chaos::Object
{
public:

	LudumPowerUp(char const * in_sound_name) : sound_name(in_sound_name){}

	virtual bool ApplyPowerUp(LudumGame * game, LudumPlayer * player, bool decreasing_power_up) const;

	virtual bool CanPowerUp(LudumGame * game, LudumPlayer * player, bool decreasing_power_up) const;

	virtual char const * GetPowerUpTitle() const { return nullptr; }

	virtual bool InitializeFromConfiguration(char const * json_name, nlohmann::json const & config, boost::filesystem::path const & config_path);


	float GetLifeCost() const { return life_cost; }

protected:

	float life_cost = 1.0f;

	std::string sound_name;
};

// =================================================
// LudumPowerUp
// =================================================

class LudumSpeedUp : public LudumPowerUp
{
public:

	using LudumPowerUp::LudumPowerUp;

	virtual bool ApplyPowerUp(LudumGame * game, LudumPlayer * player, bool decreasing_power_up) const override;

	virtual bool CanPowerUp(LudumGame * game, LudumPlayer * player, bool decreasing_power_up) const override;

	virtual char const * GetPowerUpTitle() const override;
};

// =================================================
// LudumDamageUp
// =================================================

class LudumDamageUp : public LudumPowerUp
{
public:

	LudumDamageUp(char const * in_sound_name, bool in_charged_fire) : LudumPowerUp(in_sound_name), charged_fire(in_charged_fire){}

	virtual bool ApplyPowerUp(LudumGame * game, LudumPlayer * player, bool decreasing_power_up) const override;

	virtual bool CanPowerUp(LudumGame * game, LudumPlayer * player, bool decreasing_power_up) const override;

	virtual char const * GetPowerUpTitle() const override;

protected:

	bool charged_fire = false;
};

// =================================================
// LudumFireRateUp
// =================================================

class LudumFireRateUp : public LudumPowerUp
{
public:

	using LudumPowerUp::LudumPowerUp;

	virtual bool ApplyPowerUp(LudumGame * game, LudumPlayer * player, bool decreasing_power_up) const override;

	virtual bool CanPowerUp(LudumGame * game, LudumPlayer * player, bool decreasing_power_up) const override;

	virtual char const * GetPowerUpTitle() const override;
};

