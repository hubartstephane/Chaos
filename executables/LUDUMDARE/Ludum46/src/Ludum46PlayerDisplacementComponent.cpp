
#include "Ludum46Player.h"
#include "Ludum46PlayerDisplacementComponent.h"
#include "Ludum46Level.h"
#include "Ludum46LevelInstance.h"
#include "Ludum46Game.h"
#include "Ludum46GameInstance.h"
#include "Ludum46Particles.h"

#include "death/TM.h"
#include "death/CollisionMask.h"


glm::vec2 LudumPlayerDisplacementComponent::ClampPlayerVelocity(glm::vec2 velocity, bool running) const
{
	glm::vec2 runfactor = (running) ? glm::vec2(displacement_info.run_velocity_factor, 1.0f) : glm::vec2(1.0f, 1.0f);

	for (int direction : {0, 1})
	{
		float max_vel = displacement_info.max_pawn_velocity[direction] * runfactor[direction];

		if (max_vel >= 0.0f && std::abs(velocity[direction]) > max_vel)
			velocity[direction] = max_vel * ((velocity[direction] > 0.0f) ? +1.0f : -1.0f);
	}
	return velocity;
}

PlayerDisplacementState LudumPlayerDisplacementComponent::ComputeDisplacementState(chaos::box2& pawn_box, bool jump_pressed, glm::vec2 const& stick_position, int collision_flags)
{
	glm::vec2& pawn_position = pawn_box.position;

	// extract collision flags
	bool touching_ceil = (collision_flags & PlayerDisplacementCollisionFlags::TOUCHING_CEIL);
	bool touching_floor = (collision_flags & PlayerDisplacementCollisionFlags::TOUCHING_FLOOR);
	bool touching_bridge = (collision_flags & PlayerDisplacementCollisionFlags::TOUCHING_BRIDGE);
	bool touching_wall = (collision_flags & PlayerDisplacementCollisionFlags::TOUCHING_WALL);
	bool touching_ladder = (collision_flags & PlayerDisplacementCollisionFlags::TOUCHING_LADDER);

	// tweak velocity
	if (touching_wall)
		pawn_velocity.x = 0.0f;
	if (touching_ceil || touching_floor)
		pawn_velocity.y = 0.0f;

	// current state
	bool is_jumping      = (displacement_state == PlayerDisplacementState::JUMPING);
	bool is_jumping_down = (displacement_state == PlayerDisplacementState::JUMPING_DOWN);
	bool is_climbing     = (displacement_state == PlayerDisplacementState::CLIMBING);
	bool is_grounded     = (displacement_state == PlayerDisplacementState::GROUNDED);

	if (jump_pressed)
	{
		// start jumping down ?
		if (stick_position.y < 0.0f)
		{
			if (!was_jump_pressed)
			{
				if (is_climbing)
					return PlayerDisplacementState::FALLING;

				if (!is_jumping_down)
				{
					if (is_grounded && touching_bridge && !touching_floor)
					{
						current_jumpdown_start_y = pawn_position.y;
						return PlayerDisplacementState::JUMPING_DOWN;
					}
				}
			}
		}
		// start / stop / continue jump
		if (is_jumping)
		{
			if (touching_ceil || current_jump_timer >= GetMaxJumpDuration())
			{
				pawn_velocity.y = 0.0f;
				return PlayerDisplacementState::FALLING;
			}
			return PlayerDisplacementState::JUMPING;
		}
		// start jumping ?
		else if (!was_jump_pressed)
		{
			if ((is_grounded || is_climbing) || (current_jump_count < displacement_info.max_extra_jump_count))
			{
				current_jump_timer = 0.0f;
				current_jump_start_y = pawn_position.y;
				if (!is_grounded && !is_climbing)
					++current_jump_count;
				return PlayerDisplacementState::JUMPING;
			}
		}
	}
	else
	{
		// jump button is released before the end of the jump ...
		if (is_jumping)
		{
			displacement_state = PlayerDisplacementState::FALLING;
			pawn_velocity.y = std::max(0.0f, displacement_info.jump_released_velocity_factor * GetJumpVelocity(current_jump_timer)); // do not clamp the velocity to 0 => smooth it instead
		}
	}

	if (touching_ladder && is_grounded && stick_position.y != 0.0f)
	{
		if (displacement_info.climb_max_horizontal_velocity <= 0.0f || std::abs(pawn_velocity.x) < displacement_info.climb_max_horizontal_velocity) // cannot walk too fast to start a climb
			return PlayerDisplacementState::CLIMBING;
	}

	if (touching_floor || touching_bridge)
	{
		pawn_velocity.y = 0.0f;
		return PlayerDisplacementState::GROUNDED;
	}
	else
	{
		if (is_climbing)
		{
			if (!touching_ladder)
				return PlayerDisplacementState::GROUNDED;
			return PlayerDisplacementState::CLIMBING;
		}
		if (is_jumping_down)
			return PlayerDisplacementState::JUMPING_DOWN;
		return PlayerDisplacementState::FALLING;
	}
}

// 1. forces
//
//   sum(Forces) = m.gravity = m.acceleration 
//
//   acceleration = gravity
//
// 2. this is an uniformly accelerated movement
//
//   position(t) = (1/2 * a * t ^ 2)  +  (v0 * t)  +  position(0)   | -we can set   position(0) == 0
//                                                                  | -we know      a           == gravity
//   velocity(t) = (a * t)   +   velocity(0)                        | -we search    velocity(0) ???
//
// 3. when the jumper reach the max height we know   
//
//   velocity(tmax) = 0
//
//   position(tmax) = max_jump_height
//
// 4.
//
//   0 = (a * tmax)   +    v0
//
//      => v0 = -(a * tmax)
//
//   max_jump_height = (1/2 * a * tmax ^ 2)   +   (v0 * tmax)
//	
// 5.
//  
//    max_jump_height = (1/2 * a * tmax ^ 2)   -   (a * tmax ^ 2)
//
//      => tmax = sqrt(  -2 * max_jump_height / gravity   )                   XXX : gravity is NEGATIVE !! dont panic

float LudumPlayerDisplacementComponent::GetJumpVelocity(float jump_time) const
{
	float t_max = std::sqrt(2.0f * displacement_info.max_jump_height / displacement_info.gravity);

	float v0 = displacement_info.gravity * t_max;

	return -displacement_info.gravity * jump_time + v0;
}

float LudumPlayerDisplacementComponent::GetMaxJumpDuration() const
{
	return std::sqrt(2.0f * displacement_info.max_jump_height / displacement_info.gravity);
}

float LudumPlayerDisplacementComponent::GetJumpRelativeHeight(float jump_time) const
{
	float t_max = std::sqrt(2.0f * displacement_info.max_jump_height / displacement_info.gravity);

	float v0 = displacement_info.gravity * t_max;

	return (-0.5f * displacement_info.gravity * jump_time * jump_time) + v0 * jump_time;
}












bool LudumPlayerDisplacementComponent::DoTick(float delta_time)
{
	// early exit
	death::PlayerPawn* pawn = player->GetPawn();
	if (pawn == nullptr)
		return true;

	// get player inputs of interrests
	glm::vec2 stick_position = player->GetLeftStickPosition();
	if (displacement_info.discrete_stick_mode)
	{
		stick_position.x = chaos::MathTools::AnalogicToDiscret(stick_position.x);
		stick_position.y = chaos::MathTools::AnalogicToDiscret(stick_position.y);
	}
	stick_position.y = -stick_position.y; // Y stick is inverted

	int const jump_key_buttons[] = { GLFW_KEY_SPACE, -1 };
	bool jump_pressed = player->CheckButtonPressed(jump_key_buttons, chaos::XBoxButton::BUTTON_A);

	int const run_key_buttons[] = { GLFW_KEY_LEFT_SHIFT, GLFW_KEY_RIGHT_SHIFT, -1 };
	bool run_pressed = player->CheckButtonPressed(run_key_buttons, chaos::XBoxButton::BUTTON_RIGHTTRIGGER);

	// get player position
	chaos::box2 initial_pawn_box = pawn->GetBoundingBox();

	chaos::box2 pawn_box = initial_pawn_box;
	glm::vec2 & pawn_position = pawn_box.position;

	// sum the forces 
	glm::vec2 sum_forces = glm::vec2(0.0f, 0.0f);

	if (displacement_state == PlayerDisplacementState::FALLING || displacement_state == PlayerDisplacementState::JUMPING_DOWN) // do not fall otherway
		sum_forces += glm::vec2(0.0f, -displacement_info.gravity);

	// compute horizonral velocity (based on uniform acceleration or not for climbing)
	if (displacement_state == PlayerDisplacementState::CLIMBING)
	{
		pawn_velocity.x = stick_position.x * displacement_info.climp_velocity.x;
	}
	else
	{
		float run_factor = (run_pressed) ? displacement_info.run_velocity_factor : 1.0f;

		// pawn is breaking
		if (stick_position.x == 0.0f)
			pawn_velocity.x = pawn_velocity.x * std::pow(displacement_info.pawn_break_ratio, delta_time);
		// pawn is accelerating forward
		else if (stick_position.x * pawn_velocity.x >= 0.0f)
			pawn_velocity.x = pawn_velocity.x + run_factor * stick_position.x * displacement_info.pawn_impulse.x * delta_time;
		// pawn is changing direction (break harder)
		else if (stick_position.x * pawn_velocity.x < 0.0f)
			pawn_velocity.x =
			pawn_velocity.x * std::pow(displacement_info.pawn_hardturn_break_ratio, delta_time) +
			run_factor * stick_position.x * displacement_info.pawn_impulse.x * delta_time;
	}

	// compute vertical velocity
	if (displacement_state == PlayerDisplacementState::GROUNDED)
	{
		current_jump_count = 0;
		pawn_velocity.y = 0.0f;
	}
	else if (displacement_state == PlayerDisplacementState::CLIMBING)
	{
		current_jump_count = 0;
		pawn_velocity.y = stick_position.y * displacement_info.climp_velocity.y;
	}
	else if (displacement_state == PlayerDisplacementState::JUMPING)
	{
		current_jump_timer = std::min(current_jump_timer + delta_time, GetMaxJumpDuration());
		pawn_position.y = current_jump_start_y + GetJumpRelativeHeight(current_jump_timer);
		pawn_velocity.y = 0.0f; // no physics (ignore gravity, velocity) for the jump, just use a curve for the height of the player (even if that curve is based on physical equations)
	}
	else if (displacement_state == PlayerDisplacementState::JUMPING_DOWN || displacement_state == PlayerDisplacementState::FALLING)
	{
		pawn_velocity += (sum_forces * delta_time);
	}

	// update pawn position
	pawn_velocity = ClampPlayerVelocity(pawn_velocity, run_pressed && displacement_state != PlayerDisplacementState::CLIMBING);
	pawn_position += pawn_velocity * delta_time;

	// search collisions, apply collision reaction
	int collision_flags = PlayerDisplacementCollisionFlags::NOTHING;

	char const* wangset_name = nullptr; //"CollisionPlatformer"

	pawn_box = death::ComputeTileCollisionAndReaction(GetLevelInstance(), initial_pawn_box, pawn_box, death::CollisionMask::PLAYER, pawn->GetAllocation(), wangset_name, [&collision_flags](death::TMParticle& p, chaos::Edge edge)	{
		if (edge == chaos::Edge::TOP)
			collision_flags |= PlayerDisplacementCollisionFlags::TOUCHING_FLOOR;
		else if (edge == chaos::Edge::BOTTOM)
			collision_flags |= PlayerDisplacementCollisionFlags::TOUCHING_CEIL; 
		else if (edge == chaos::Edge::LEFT || edge == chaos::Edge::RIGHT)
			collision_flags |= PlayerDisplacementCollisionFlags::TOUCHING_WALL;
		return true;
	});

	// update player state
	displacement_state = ComputeDisplacementState(pawn_box, jump_pressed, stick_position, collision_flags);

	// update the player pawn
	pawn->SetPosition(pawn_box.position);

	was_jump_pressed = jump_pressed;

	return true;
}
