#pragma once

#include "Ludum44PCH.h"
#include "Ludum44Particles.h"

// =========================================================
// LudumPlayer
// =========================================================

class LudumPlayer : public chaos::Player
{

public:

	friend class LudumPowerUp;
	friend class LudumSpeedUp;
	friend class LudumDamageUp;
	friend class LudumFireRateUp;

	CHAOS_DECLARE_GAMEPLAY_FRIENDSHIPS(Ludum);

	CHAOS_DECLARE_OBJECT_CLASS(LudumPlayer, chaos::Player);

	float GetBuyTimer() const { return buy_timer; }

protected:

	/** override */
	virtual bool SerializeFromJSON(chaos::JSONReadConfiguration config) override;
	/** override */
	virtual bool SerializeIntoJSON(nlohmann::json * json) const override;

	/** update player displacement with the stick values that are stored */
	virtual void UpdatePlayerAcceleration(glm::vec2 const& stick_position, float delta_time);

	virtual bool EnumerateInputActions(chaos::InputActionProcessor& in_action_processor, chaos::EnumerateInputActionContext in_context) override;

	/** test whether the player is firing */
	void UpdatePlayerFire(bool fire_pressed, bool charged_pressed, float delta_time);
	/** test whether the player wants to by an item */
	void UpdatePlayerBuyingItem(bool buy_pressed, float delta_time);

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
