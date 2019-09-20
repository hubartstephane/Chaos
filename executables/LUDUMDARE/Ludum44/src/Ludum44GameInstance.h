#pragma once

#include "Ludum44PowerUp.h"
#include "Ludum44GameCheckpoint.h"

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

	ParticleFire * FireProjectile(chaos::ParticleAllocationBase * allocation, chaos::box2 const & ref_box, chaos::BitmapAtlas::BitmapLayout const & layout, float ratio_to_box, int count, char const * sound_name, float delta_rotation, bool player_ownership, float velocity, float offset_rotation);

	ParticleExplosion * FireExplosion(chaos::box2 const & ref_box);

protected:

	virtual death::Player * DoCreatePlayer() override;

	virtual bool DoCheckGameOverCondition() override;

	virtual void OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance) override;

	virtual void OnPlayerEntered(death::Player * player) override;

	virtual bool OnRestartedFromCheckpoint(death::Player * player) override;

	void OnPowerUpZone(death::Player * player, bool enter, death::TiledMap::TriggerSurfaceObject * surface, bool decreasing_power_up);

	
	virtual bool DoSaveIntoCheckpoint(death::GameCheckpoint * checkpoint) const override;
	
	virtual bool DoLoadFromCheckpoint(death::GameCheckpoint const * checkpoint) override;
	

protected:

	chaos::shared_ptr<LudumPowerUp> current_power_up;

	chaos::shared_ptr<death::TiledMap::TriggerSurfaceObject> current_power_up_surface;
};
