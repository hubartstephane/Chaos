#pragma once

#include "Ludum44PCH.h"
#include "Ludum44PowerUp.h"

// =========================================================
// LudumGameInstance
// =========================================================

class LudumGameInstance : public chaos::GameInstance
{

public:

	friend class GameHUDHealthBarComponent;
	friend class GameHUDPowerUpComponent;
	friend class PowerUpTrigger;

	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS(LudumGameInstance, chaos::GameInstance);

	LudumGameInstance();

    chaos::ParticleAccessor<ParticleFire> FireProjectile(char const* bitmap_name, chaos::box2 const& ref_box, float ratio_to_box, int count, char const* sound_name, float offset_rotation, float delta_rotation, float velocity, float damage, bool trample, bool player_ownership);

	void FireExplosion(chaos::box2 const & ref_box);

protected:

	virtual void OnLevelChanged(chaos::Level * new_level, chaos::Level * old_level, chaos::LevelInstance * new_level_instance) override;

	virtual void OnPlayerEntered(chaos::Player * player) override;

	void OnPowerUpZone(chaos::Player * player, bool enter, PowerUpTrigger* powerup_trigger);

protected:

	chaos::shared_ptr<LudumPowerUp> current_power_up;

	chaos::shared_ptr<PowerUpTrigger> current_powerup_trigger;
};
