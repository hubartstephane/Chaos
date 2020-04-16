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

class PlayerDisplacementComponentInfo
{
public:

	/** whether the player left stick value is exactly -1, 0, +1 or whether this may have intermediate values */
	bool discrete_stick_mode = true;
	/** clamping the velocity in both direction */
	glm::vec2 max_pawn_velocity = glm::vec2(500.0f, 500.0f);
	/** IMPULSE mode : the impulse value for the pawn (immediate velocity given along X axis) */
	glm::vec2 pawn_impulse = glm::vec2(500.0f, 0.0f);
	/** the gravity to apply to the pawn */
	float gravity = 2000.0f;
	/** the maximum height jump */
	float max_jump_height = 64.0f;
	/** the maximum extra jump count */
	int max_extra_jump_count = 1;
	/** whenever the jump button is released during a jump, some ratio of the jump velocity is transmitted to smooth the animation */
	float jump_released_velocity_factor = 0.3f;
	/** the climb speed */
	glm::vec2 climp_velocity = glm::vec2(30.0f, 75.0f);
	/** the distance that can be jumpdown before colliding bridge again */
	float max_jumpdown_height = 32.0f;
#if 0
	/** pawn do not advance forward anymore */
	float pawn_break_ratio = 0.01f;
	/** pawn is trying to change its direction */
	float pawn_hardturn_break_ratio = 0.005f;
#endif


	/** pawn do not advance forward anymore */
	float pawn_break_ratio = 0.1f;
	/** pawn is trying to change its direction */
	float pawn_hardturn_break_ratio = 0.05f;

	/** an horizontal velocity above which climb is forbidden */
	float climb_max_horizontal_velocity = 32.0f;

	/* an extend of the pawn box so we can detect collision */
	glm::vec2 pawn_box_extend = glm::vec2(15.0f, 15.0f); 

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
	PlayerDisplacementCollisionFlags ApplyCollisionsToPlayer(chaos::box2& box, std::vector<death::TileParticleCollisionInfo> const & colliding_tiles);
	/** compute the new displacement state */
	PlayerDisplacementState ComputeDisplacementState(chaos::box2 & pawn_box, bool jump_pressed, glm::vec2 const& stick_position, PlayerDisplacementCollisionFlags collision_flags);
	/** get the offset from the jumping point when the player is jumping */
	float GetJumpRelativeHeight(float jump_time) const;
	/** get the duration of a jump according to gravity/max_height */
	float GetMaxJumpDuration() const;
	/** get the velocity of jumper at given time */
	float GetJumpVelocity(float jump_time) const;

	/** clamp the player velocity according to limits */
	glm::vec2 ClampPlayerVelocity(glm::vec2 velocity) const;

	/** extend the box by adding some padding */
	chaos::box2 ExtendBox(chaos::box2 const& src, float left, float right, float bottom, float top) const;

protected:

	/** the rules for the displacement */
	PlayerDisplacementComponentInfo displacement_info;

	/** the current state for the player */
	PlayerDisplacementState displacement_state = PlayerDisplacementState::GROUNDED;

	/** the current velocity of the pawn */
	glm::vec2 pawn_velocity = glm::vec2(0.0f, 0.0f);

	/** the current Y position where the jump started */
	float current_jump_start_y = 0.0f;
	/** the jump timer */
	float current_jump_timer = 0.0f;
	/** the current jump count */
	int current_jump_count = 0;

	/** the current Y position where the jump down started */
	float current_jumpdown_start_y = 0.0f;
	
	/** whether jump was pressed previous frame */
	bool was_jump_pressed = false;
};
