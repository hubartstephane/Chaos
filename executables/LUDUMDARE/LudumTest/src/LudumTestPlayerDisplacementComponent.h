#pragma once

#include <chaos/StandardHeaders.h> 
#include <death/PlayerDisplacementComponent.h>


// =========================================================
// LudumPlayerDisplacementComponent
// =========================================================

class LudumPlayerDisplacementComponent : public death::PlayerDisplacementComponent
{

public:


	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	/** constructor */
	using death::PlayerDisplacementComponent::PlayerDisplacementComponent;

	/** override */
	virtual bool DoTick(float delta_time) override;

protected:

	/** whether the player left stick value is exactly -1, 0, +1 or whether this may have intermediate values */
	bool analogic_stick_mode = true;

	/** the velocity of the pawn */
	glm::vec2 pawn_velocity = glm::vec2(0.0f, 0.0f);
	/** clamping the velocity in both direction */
	glm::vec2 max_pawn_velocity = glm::vec2(-1.0f, -1.0f);

	/** 2 modes : IMPULSE / ACCELERATING for the pawn (for the left stick) */
	bool impulse_mode = true;

	/** IMPULSE mode : the impulse value for the pawn according (immediate velocity given) */
	glm::vec2 pawn_impulse = glm::vec2(200.0f, 0.0f);
	/** ACCELERATION mode : acceleration given to the pawn */
	glm::vec2 pawn_acceleration = glm::vec2(10.0f, 0.0f);

	/** the gravity to apply to the pawn */
	float gravity = 10.0f;

};
