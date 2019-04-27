#pragma once

#include <chaos/StandardHeaders.h> 

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

protected:

	/** override */
	virtual void TickPlayerDisplacement(double delta_time) override;
	/** override */
	virtual void InternalHandleGamepadInputs(double delta_time, chaos::MyGLFW::GamepadData const * gpd) override;
	/** override */
	virtual void HandleKeyboardInputs(double delta_time) override;

	/** update player displacement with the stick values that are stored */
	virtual void UpdatePlayerAcceleration(double delta_time);
	
protected:

	float current_life = 1.0f;
	float current_speed = 1.0f;
	float current_damage = 1.0f;
	float current_fire_rate = 1.0f;
};
