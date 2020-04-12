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
	JUMPING_DOWN, // player is going down through a platform
	CLIMBING      // whether the player is on a ladder a goind up or down
};

enum class PlayerDisplacementCollisionFlags : int
{
	NOTHING         = 0, // no collision of interrests
	TOUCHING_FLOOR  = (1 << 0),
	TOUCHING_CEIL   = (1 << 1),
	TOUCHING_WALL   = (1 << 2),
	TOUCHING_LADDER = (1 << 2)
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
	PlayerDisplacementCollisionFlags ApplyCollisionsToPlayer(chaos::box2 & pawn_box, std::vector<death::TileParticleCollisionInfo> const & colliding_tiles) const;

	/** clamp the player velocity according to limits */
	glm::vec2 ClampPlayerVelocity(glm::vec2 velocity) const;

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
	float gravity = 100.0f;


	/** the current state for the player */
	PlayerDisplacementState player_state = PlayerDisplacementState::GROUNDED;



	/** the maximum height jump */
	float max_jump_height = 60.0f;
	/** the current Y position where the jump started */
	float current_jump_start_y = 0.0f;
	/** the jump velocity */
	float jump_velocity = 30.0f;



	/** the maximum jump count */
	int max_jump_count = 2;
	/** the current jump count */
	int current_jump_count = 0;

	/** the delay before jumping (usefull if we want to jump down through a plaftorm) */
	float jump_delay = 0.1f;
	/** the current jump delay value */
	float current_jump_delay = 0.0f;
	


};
