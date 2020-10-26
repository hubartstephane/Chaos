#pragma once

#include "Ludum44PowerUp.h"

#include <chaos/Chaos.h>

#include <death/Game.h>
#include <death/GameInstance.h>
#include <death/GameFramework.h>

// =========================================================
// LudumGameInstance
// =========================================================

class LudumGameInstance : public death::GameInstance
{

public:

	friend class GameHUDHealthBarComponent;
	friend class GameHUDPowerUpComponent;	
	friend class PowerUpTrigger;	

	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	CHAOS_OBJECT_DECLARE_CLASS2(LudumGameInstance, death::GameInstance);

	LudumGameInstance();

    chaos::ParticleAccessor<ParticleFire> FireProjectile(char const* bitmap_name, chaos::box2& ref_box, float ratio_to_box, int count, char const* sound_name, float offset_rotation, float delta_rotation, float velocity, float damage, bool trample, bool player_ownership);

	void FireExplosion(chaos::box2 const & ref_box);

protected:

	virtual void OnLevelChanged(death::Level * new_level, death::Level * old_level, death::LevelInstance * new_level_instance) override;

	virtual void OnPlayerEntered(death::Player * player) override;

	void OnPowerUpZone(death::Player * player, bool enter, PowerUpTrigger* powerup_trigger);

protected:

	chaos::shared_ptr<LudumPowerUp> current_power_up;

	chaos::shared_ptr<PowerUpTrigger> current_powerup_trigger;
};
