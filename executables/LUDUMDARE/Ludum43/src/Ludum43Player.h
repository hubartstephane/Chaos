#pragma once

#include <chaos/Chaos.h>

#include "Ludum43Particles.h"

// =========================================================
// LudumPlayer
// =========================================================

class LudumPlayer : public chaos::Player
{

public:

	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS2(LudumPlayer, chaos::Player);

	/** get the player particle */
	ParticlePlayer * GetPlayerParticle();
	ParticlePlayer const * GetPlayerParticle() const;

protected:

	/** override */
	virtual void OnLevelChanged(chaos::Level * new_level, chaos::Level * old_level, chaos::LevelInstance * new_level_instance) override;
	/** override */
	virtual void TickInternal(float delta_time) override;
	/** override */
	virtual void TickPlayerDisplacement(float delta_time) override;
	/** override */
	virtual void InternalHandleGamepadInputs(float delta_time, chaos::GamepadState const * gpd) override;
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

	/** read the health back from the particle */
	void GetHealthFromParticle();
	
protected:

	float current_cooldown = 0.1f;
	float current_dash_duration = 0.0f;
	float current_dash_cooldown = 0.0f;

};
