#pragma once

#include "Ludum44Player.h"

#include <chaos/StandardHeaders.h> 
#include <chaos/ReferencedObject.h>

#include <death/Game.h>
#include <death/GameFramework.h>

// =================================================
// LudumPowerUp
// =================================================

class LudumPowerUp : public chaos::ReferencedObject
{
public:

	virtual bool ApplyPowerUp(LudumGame * game, LudumPlayer * player) const;

	virtual bool CanPowerUp(LudumGame * game, LudumPlayer * player) const;

	virtual char const * GetPowerUpTitle() const { return nullptr; }

	virtual bool InitializeFromConfiguration(char const * json_name, nlohmann::json const & config, boost::filesystem::path const & config_path);


	float GetLifeCost() const { return life_cost; }

protected:

	float life_cost = 1.0f;
};

// =================================================
// LudumPowerUp
// =================================================

class LudumSpeedUp : public LudumPowerUp
{
public:

	virtual bool ApplyPowerUp(LudumGame * game, LudumPlayer * player) const override;

	virtual bool CanPowerUp(LudumGame * game, LudumPlayer * player) const override;

	virtual char const * GetPowerUpTitle() const override;
};

// =================================================
// LudumDamageUp
// =================================================

class LudumDamageUp : public LudumPowerUp
{
public:

	virtual bool ApplyPowerUp(LudumGame * game, LudumPlayer * player) const override;

	virtual bool CanPowerUp(LudumGame * game, LudumPlayer * player) const override;

	virtual char const * GetPowerUpTitle() const override;
};

// =================================================
// LudumFireRateUp
// =================================================

class LudumFireRateUp : public LudumPowerUp
{
public:

	virtual bool ApplyPowerUp(LudumGame * game, LudumPlayer * player) const override;

	virtual bool CanPowerUp(LudumGame * game, LudumPlayer * player) const override;

	virtual char const * GetPowerUpTitle() const override;
};

