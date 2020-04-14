#pragma once

#include <chaos/StandardHeaders.h> 
#include <death/PlayerDisplacementComponent.h>


// =========================================================
// LudumPlayerDisplacementComponent
// =========================================================

enum class PlayerDisplacementState : int
{
	GROUNDED,     // player is on the ground
	FALLING,      // player in the air and going down
	JUMPING,      // player jump button has been pressed
	JUMPING_DOWN, // whether the player has jumped down through a bridge
	CLIMBING      // whether the player is on a ladder a goind up or down
};

enum PlayerDisplacementCollisionFlags : int // XXXX: no class, so this can be implicitly converted to int
{
	NOTHING         = 0, // no collision of interrests
	TOUCHING_FLOOR  = (1 << 0),
	TOUCHING_BRIDGE = (1 << 1), // a bridge is a kind of floor you can jump-down through
	TOUCHING_CEIL   = (1 << 2),
	TOUCHING_WALL   = (1 << 3),
	TOUCHING_LADDER = (1 << 4)
};

class LudumPlayerDisplacementComponent : public death::PlayerDisplacementComponent
{

public:


	DEATH_GAMEFRAMEWORK_DECLARE_FRIENDSHIPS(Ludum);

	/** constructor */
	using death::PlayerDisplacementComponent::PlayerDisplacementComponent;

	/** override */
	virtual bool DoTick(float delta_time) override;

protected:

	/** compute the collision flags according to all */
	PlayerDisplacementCollisionFlags ApplyCollisionsToPlayer(chaos::box2& box, chaos::box2& extended_pawn_box, glm::vec2& velocity, std::vector<death::TileParticleCollisionInfo> const & colliding_tiles) const;
	/** compute the new displacement state */
	PlayerDisplacementState ComputeDisplacementState(chaos::box2 & pawn_box, bool jump_pressed, glm::vec2 const& stick_position, PlayerDisplacementCollisionFlags collision_flags);
	/** get the offset from the jumping point when the player is jumping */
	float GetJumpRelativeHeight(float jump_time) const;
	/** clamp the player velocity according to limits */
	glm::vec2 ClampPlayerVelocity(glm::vec2 velocity) const;

	/** compute the jump initial velocity to reach the wanted height */
	float ComputeJumpInitialVelocity(float max_height) const;



protected:

	/** whether the player left stick value is exactly -1, 0, +1 or whether this may have intermediate values */
	bool analogic_stick_mode = true;

		// VELOCITY

	/** the velocity of the pawn */
	glm::vec2 pawn_velocity = glm::vec2(0.0f, 0.0f);
	/** clamping the velocity in both direction */
	glm::vec2 max_pawn_velocity = glm::vec2(-1.0f, -1.0f);
	/** IMPULSE mode : the impulse value for the pawn (immediate velocity given along X axis) */
	glm::vec2 pawn_impulse = glm::vec2(200.0f, 0.0f);

	/** the gravity to apply to the pawn */
	float gravity = 300.0f;








	/** the current state for the player */
	PlayerDisplacementState displacement_state = PlayerDisplacementState::GROUNDED;

		// JUMP

	/** the maximum height jump */
	float max_jump_height = 64.0f;
	/** the full jump duration */
	float jump_duration = 0.4f;

	/** the jump velocity */
	float jump_velocity = 100.0f;
	/** the maximum extra jump count */
	int max_extra_jump_count = 1;
	/** the delay before jumping (usefull if we want to jump down through a plaftorm) */
	float jump_delay = 0.1f;

	/** the current Y position where the jump started */
	float current_jump_start_y = 0.0f;
	/** the jump timer */
	float current_jump_timer = 0.0f;

	/** the current jump count */
	int current_jump_count = 0;
	/** the current jump delay value */
	float current_jump_delay = 0.0f;

		// CLIMB

	/** the climb speed */
	float climp_velocity = 50.0f;

		// JUMP DOWN

	/** the distance that can be jumpdown before colliding bridge again */
	float max_jumpdown_height = 32.0f;
	/** the current Y position where the jump down started */
	float current_jumpdown_start_y = 0.0f;

	
	/** whether jump was pressed previous frame */
	bool was_jump_pressed = false;
};
