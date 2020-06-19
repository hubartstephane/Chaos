#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/BitmapAtlas.h>

#include <death/Game.h>
#include <death/GameInstance.h>
#include <death/Player.h>
#include <death/GameFramework.h>



#include "Ludum44Particles.h"

// =========================================================
// LudumPlayer
// =========================================================

class LudumPlayer : public death::Player
{

public:

	friend class LudumPowerUp;
	friend class LudumSpeedUp;
	friend class LudumDamageUp;
	friend class LudumFireRateUp;

	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	float GetBuyTimer() const { return buy_timer; }

protected:

	/** override */
	virtual death::PlayerCheckpoint * DoCreateCheckpoint() const override;
	/** override */
	virtual bool DoLoadFromCheckpoint(death::PlayerCheckpoint const * checkpoint) override;
	/** override */
	virtual bool DoSaveIntoCheckpoint(death::PlayerCheckpoint * checkpoint) const override;

	/** override */
	virtual void TickInternal(float delta_time) override;
	/** override */
	virtual void TickPlayerDisplacement(float delta_time) override;

	/** update player displacement with the stick values that are stored */
	virtual void UpdatePlayerAcceleration(float delta_time);
	
	/** test whether the player is firing */
	void UpdatePlayerFire(float delta_time);
	/** test whether the player wants to by an item */
	void UpdatePlayerBuyingItem(float delta_time);

	/** fire a charged projectile */
	void FireChargedProjectile();	
	/** fire a normal projectile */
	void FireNormalProjectile();		

protected:

	size_t current_speed_index = 0;
	size_t current_damage_index = 0;
	size_t current_charged_damage_index = 0;
	size_t current_fire_rate_index = 0;

	float buy_timer = 0.0f;
	float fire_timer = 0.0f; // the fire_timer is a COOLDOWN !
	float charged_fire_timer = 0.0f;

	bool buylocked = false;
};
