#pragma once

#include "Ludum43PCH.h"
#include "Ludum43Particles.h"

// =========================================================
// LudumPlayer
// =========================================================

class LudumPlayer : public chaos::Player
{

public:

	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS(LudumPlayer, chaos::Player);

	/** get the player particle */
	ParticlePlayer * GetPlayerParticle();
	ParticlePlayer const * GetPlayerParticle() const;

protected:

	/** override */
	virtual void OnLevelChanged(chaos::Level * new_level, chaos::Level * old_level, chaos::LevelInstance * new_level_instance) override;
	/** override */
	virtual void TickInternal(float delta_time) override;
	/** override */
	virtual bool EnumerateInputActions(chaos::InputActionProcessor& in_action_processor, chaos::EnumerateInputActionContext in_context) override;

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
