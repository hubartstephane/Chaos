#pragma once

#include <death/Game.h>
#include <death/GameHUD.h>
#include <death/GameFramework.h>

#include "Ludum44PowerUp.h"

namespace death
{
	namespace GameHUDKeys
	{
		CHAOS_DECLARE_TAG(POWER_UP_ID);
		CHAOS_DECLARE_TAG(LEVEL_TITLE_ID);
	};
};



// ====================================================================
// GameHUDLifeBarComponent
// ====================================================================

class GameHUDLifeBarComponent : public death::GameHUDSingleAllocationComponent
{
protected:

	/** override */
	virtual bool DoTick(double delta_time) override;
};

// ====================================================================
// GameHUDPowerUpComponent
// ====================================================================

class GameHUDPowerUpComponent : public death::GameHUDSingleAllocationComponent
{
	friend class death::GameHUD;

protected:

	virtual bool DoTick(double delta_time) override;

protected:

	chaos::weak_ptr<LudumPowerUp> cached_power_up;
};

// ====================================================================
// GameHUDLevelTitleComponent
// ====================================================================

class GameHUDLevelTitleComponent : public death::GameHUDSingleAllocationComponent
{
	friend class death::GameHUD;

protected:

	/** override */
	virtual bool DoTick(double delta_time) override;

protected:

	std::string cached_level_title;
};

// ====================================================================
// LudumPlayingHUD
// ====================================================================

class LudumPlayingHUD : public death::PlayingHUD
{
public:

	DEATH_GAMEFRAMEWORK_DECLARE_HUD(Ludum);

	/** constructor */
	LudumPlayingHUD(death::Game * in_game) :
		death::PlayingHUD(in_game) {}

protected:

	/** override */
	virtual bool FillHUDContent() override;
	/** override */
	virtual bool CreateHUDLayers() override;
};
