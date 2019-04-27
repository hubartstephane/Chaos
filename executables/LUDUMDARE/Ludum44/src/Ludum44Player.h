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

	DEATH_GAMEFRAMEWORK_DECLARE_PLAYER(Ludum);

	/** constructor */
	LudumPlayer(death::GameInstance * in_game_instance);

	/** get the player particle */
	ParticlePlayer * GetPlayerParticle();
	ParticlePlayer const * GetPlayerParticle() const;

	/** called whenever the level is being changed */
	void OnLevelChanged();

	/** override */
	virtual void SetPlayerAllocation(chaos::ParticleAllocationBase * in_allocation) override;



	float GetCurrentLife() const { return current_life; }
	float GetCurrentMaxLife() const { return current_max_life; }
	float GetBuyTimer() const { return buy_timer; }

protected:

	/** override */
	virtual void TickPlayerDisplacement(double delta_time) override;
	/** override */
	virtual void InternalHandleGamepadInputs(double delta_time, chaos::MyGLFW::GamepadData const * gpd) override;
	/** override */
	virtual void HandleKeyboardInputs(double delta_time) override;

	/** update player displacement with the stick values that are stored */
	virtual void UpdatePlayerAcceleration(double delta_time);
	
	/** test whether the player is firing */
	void UpdatePlayerFire(double delta_time);
	/** test whether the player wants to by an item */
	void UpdatePlayerBuyingItem(double delta_time);

	/** check whether some inputs are pressed */
	bool CheckButtonPressed(int const * keyboard_buttons, int gamepad_button);

	/** fire a charged projectile */
	ParticleFire * FireChargedProjectile();	
	/** fire a normal projectile */
	ParticleFire *FireNormalProjectile();		
	/** fire generic projectile */
	ParticleFire * FireProjectile(chaos::BitmapAtlas::BitmapLayout const & layout);

protected:

	float current_life = 1.0f;
	float current_max_life = 1.0f;

	float current_speed = 1.0f;
	float current_damage = 1.0f;
	float current_fire_rate = 1.0f;

	float buy_timer = 0.0f;
	float fire_timer = 0.0f; // the fire_timer is a COOLDOWN !
	float charged_fire_timer = 0.0f;

	chaos::shared_ptr<chaos::ParticleAllocationBase> fire_allocation;

	chaos::BitmapAtlas::BitmapLayout fire_bitmap_layout;

	chaos::BitmapAtlas::BitmapLayout charged_fire_bitmap_layout;

};
