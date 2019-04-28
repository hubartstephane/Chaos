#pragma once

#include "Ludum44PowerUp.h"

#include <chaos/StandardHeaders.h> 

#include <death/Game.h>
#include <death/GameInstance.h>
#include <death/GameFramework.h>

// =========================================================
// LudumGameInstance
// =========================================================

class LudumGameInstance : public death::GameInstance
{

public:

	friend class GameHUDLifeBarComponent;
	friend class GameHUDPowerUpComponent;	
	friend class PowerUpTriggerSurfaceObject;	

	DEATH_GAMEFRAMEWORK_DECLARE_GAMEINSTANCE(Ludum);

	/** constructor */
	LudumGameInstance(death::Game * in_game);

protected:

	virtual death::Player * DoCreatePlayer() override;

	virtual bool DoCheckGameOverCondition() override;

	virtual void OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance) override;

	virtual bool DoTick(double delta_time) override;

	virtual void OnPlayerEntered(death::Player * player) override;

	void OnPowerUpZone(death::Player * player, bool enter, death::TiledMap::TriggerSurfaceObject * surface);

	

protected:

	chaos::shared_ptr<LudumPowerUp> current_power_up;

	chaos::shared_ptr<death::TiledMap::TriggerSurfaceObject> current_power_up_surface;
};
