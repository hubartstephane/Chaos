#pragma once

#include <chaos/StandardHeaders.h> 

#include <death/Game.h>
#include <death/GameInstance.h>
#include <death/Player.h>
#include <death/GameFramework.h>

#include "Ludum43Particles.h"

// =========================================================
// LudumPlayer
// =========================================================

class LudumPlayer : public death::Player
{

public:

	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	/** constructor */
	LudumPlayer(death::GameInstance * in_game_instance);

	/** get the player particle */
	ParticlePlayer * GetPlayerParticle();
	ParticlePlayer const * GetPlayerParticle() const;

	/** override */
	virtual void SetPlayerAllocation(chaos::ParticleAllocationBase * in_allocation) override;

protected:

	/** override */
	virtual void OnLevelChanged(death::Level * new_level, death::Level * old_level, death::LevelInstance * new_level_instance) override;
	/** override */
	virtual void TickPlayerDisplacement(float delta_time) override;
	/** override */
	virtual void InternalHandleGamepadInputs(float delta_time, chaos::MyGLFW::GamepadData const * gpd) override;
	/** override */
	virtual void HandleKeyboardInputs(float delta_time) override;

	/** update player displacement with the stick values that are stored */
	virtual void UpdatePlayerAcceleration(float delta_time);
	/** cooldown the weapon */
	void TickCooldown(float delta_time);
	void TickDashValues(float delta_time);

	/** change dash mode */
	void SetDashMode(bool dash);
	/** change reverse mode */
	void SetReverseMode(bool reversed_mode);
	
protected:

	float current_cooldown = 0.1f;
	float current_dash_duration = 0.0f;
	float current_dash_cooldown = 0.0f;

};
