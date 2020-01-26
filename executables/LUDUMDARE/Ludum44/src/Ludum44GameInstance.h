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
	friend class PowerUpTriggerObject;	

	DEATH_GAMEFRAMEWORK_DECLARE_GAMEINSTANCE(Ludum);

	/** constructor */
	LudumGameInstance(death::Game * in_game);

    chaos::ParticleAccessor<ParticleFire> FireProjectile(char const* bitmap_name, chaos::box2& ref_box, float ratio_to_box, int count, char const* sound_name, float offset_rotation, float delta_rotation, float velocity, float damage, bool trample, bool player_ownership);

	void FireExplosion(chaos::box2 const & ref_box);

protected:

	virtual death::Player * DoCreatePlayer() override;

	virtual bool DoCheckGameOverCondition() override;

	virtual void OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance) override;

	virtual void OnPlayerEntered(death::Player * player) override;

	void OnPowerUpZone(death::Player * player, bool enter, death::TiledMap::TriggerObject * surface, bool decreasing_power_up);

	
	virtual bool DoSaveIntoCheckpoint(death::GameCheckpoint * checkpoint) const override;
	
	virtual bool DoLoadFromCheckpoint(death::GameCheckpoint const * checkpoint) override;
	

protected:

	chaos::shared_ptr<LudumPowerUp> current_power_up;

	chaos::shared_ptr<death::TiledMap::TriggerObject> current_power_up_surface;
};
