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

	for (int direction = 0; direction <= 1; ++direction)
	{
		float max_vel = displacement_info.max_pawn_velocity[direction] * runfactor[direction];

		if (max_vel >= 0.0f && std::abs(velocity[direction]) > max_vel)
			velocity[direction] = max_vel * ((velocity[direction] > 0.0f) ? +1.0f : -1.0f);
	}
	return velocity;
}

float LudumPlayerDisplacementComponent::ComputeBoxUnionSideLength(chaos::box2 const& b1, chaos::box2 const& b2, int axis) const
{
	// compute the HEIGHT (or WIDTH) of the box UNION
	//
	// ------+ A
	//       |
	//  C +------    ^
	//    |  |       | 
	//    |  |       | Result
	//    |  |       |
	// ------+ B     v
    //    |   
	//  D +------
	//
	// XXX : this 'could be' replaced by
	//
	//       return (b1 & b2).half_size.y * 2.0f;
	//
	// XXX : in fact this can't because for SOFT TOUCHING box (along x axis for example), the half_size.y is set to -1 instead of 0
	//

	float a = b1.position[axis] + b1.half_size[axis];
	float b = b1.position[axis] - b1.half_size[axis];
	float c = b2.position[axis] + b2.half_size[axis];
	float d = b2.position[axis] - b2.half_size[axis];

	float mn = std::min(a, c);
	float mx = std::max(b, d);
	float dist = mn - mx;
	return dist;
}


bool LudumPlayerDisplacementComponent::CheckWallCollision(chaos::box2& box, chaos::box2 const & pb, PlayerDisplacementCollisionFlags& collision_flag)
{
	// we can choose between a wall or a floor/ceil collision depending on the height of the interpenetration zone
	float h = ComputeBoxUnionSideLength(box, pb, 1);
	if (h > box.half_size.y)
	{
		float min_pawn_x = box.position.x - box.half_size.x;
		float max_pawn_x = box.position.x + box.half_size.x;

		if ((pb.position.x + pb.half_size.x >= min_pawn_x) && (pb.position.x + pb.half_size.x <= max_pawn_x)) // WALL is on the LEFT
		{
			box.position.x = pb.position.x + pb.half_size.x + box.half_size.x;
			if (pawn_velocity.x < 0.0f)
				pawn_velocity.x = 0.0f;
		}
		else if ((pb.position.x - pb.half_size.x >= min_pawn_x) && (pb.position.x - pb.half_size.x <= max_pawn_x)) // WALL is on the RIGHT
		{
			box.position.x = pb.position.x - pb.half_size.x - box.half_size.x;
			if (pawn_velocity.x > 0.0f)
				pawn_velocity.x = 0.0f;
		}

		collision_flag = (PlayerDisplacementCollisionFlags)(collision_flag | PlayerDisplacementCollisionFlags::TOUCHING_WALL); // pushed HORIZONTALLY	
		return true;
	}
	return false;
}
 
PlayerDisplacementCollisionFlags LudumPlayerDisplacementComponent::ApplyCollisionsToPlayer(chaos::box2& box, std::vector<death::TileCollisionInfo> const& colliding_tiles)
{
	PlayerDisplacementCollisionFlags result = PlayerDisplacementCollisionFlags::NOTHING;

	// detect collision flags
	for (death::TileCollisionInfo const& collision : colliding_tiles)
	{
		// check for real collisions because the very first TiledMap request may have use a an extra padding
		// (Collide(...) detects SOFT TOUCH (strict 0 distance) as collision)
		chaos::box2 const& pb = collision.particle->bounding_box;
		if (!chaos::Collide(box, pb))
			continue;

		// search the kind of object pawn is touching
		bool is_wall = false;
		bool is_bridge = false;
		bool is_ladder = false;

		if (collision.tile_info.tiledata->IsObjectOfType("WALL"))
			is_wall = true;
		else if (collision.tile_info.tiledata->IsObjectOfType("BRIDGE"))
			is_bridge = true;
		else if (collision.tile_info.tiledata->IsObjectOfType("LADDER"))
			is_ladder = true;


		// ladder ?
		if (is_ladder)
		{
			result = (PlayerDisplacementCollisionFlags)(result | PlayerDisplacementCollisionFlags::TOUCHING_LADDER);
			continue;
		}

		// can continue
		if (!is_wall && !is_bridge)
			continue;

		// is FLOOR ? (check this first to have precedence)
		if ((pb.position.y + pb.half_size.y >= box.position.y - box.half_size.y) && (pb.position.y + pb.half_size.y <= box.position.y + box.half_size.y))
		{
			// to known whether to keep a FLOOR or a WALL collision we compute the HEIGHT of the interpenetration
			if (!CheckWallCollision(box, pb, result) && ComputeBoxUnionSideLength(box,pb, 0) > 0.0f)
			{
				box.position.y = pb.position.y + pb.half_size.y + box.half_size.y;				
				pawn_velocity.y = 0.0f;

				result = (PlayerDisplacementCollisionFlags)(result | PlayerDisplacementCollisionFlags::TOUCHING_FLOOR); // pushed UP	
			}
		}
		// is CEIL ? (cannot be both floor and ceil)
		else if ((pb.position.y - pb.half_size.y >= box.position.y - box.half_size.y) && (pb.position.y - pb.half_size.y <= box.position.y + box.half_size.y))
		{
			// to known whether to keep a FLOOR or a WALL collision we compute the HEIGHT of the interpenetration
			if (!CheckWallCollision(box, pb, result) && ComputeBoxUnionSideLength(box, pb, 0) > 0.0f)
			{
				box.position.y = pb.position.y - pb.half_size.y - box.half_size.y;				
				pawn_velocity.y = 0.0f;				

				result = (PlayerDisplacementCollisionFlags)(result | PlayerDisplacementCollisionFlags::TOUCHING_CEIL); // pushed UP			
			}				
		}
	}
	return result;
}









#if 0

PlayerDisplacementCollisionFlags LudumPlayerDisplacementComponent::ApplyCollisionsToPlayer(chaos::box2& box, std::vector<death::TileCollisionInfo> const& colliding_tiles)
{


		// keep the box outside the

		// ========================= BRIDGE =========================
		else if (is_bridge)
		{
			// can go through a bridge from down to up
			if (pawn_velocity.y > 0.0f)
				continue;

			// pushed DOWN -> the bridge is not a collider as a ceil
			if (displacement.y < 0.0f)
				continue;

			// jumping down ?
			if (displacement_state == PlayerDisplacementState::JUMPING_DOWN)
				if (current_jumpdown_start_y - box.position.y < displacement_info.max_jumpdown_height) // player has just started jumping down -> ignore the bridge
					continue;
			// pushed UP
			if (displacement.y > 0.0f)
				result = (PlayerDisplacementCollisionFlags)(result | PlayerDisplacementCollisionFlags::TOUCHING_BRIDGE);

		}
		chaos::RestrictToOutside(collision.particle.bounding_box, box); // displace the pawn box according to the NON EXTENDED pawn box
	}
	return result;
}

#endif

PlayerDisplacementState LudumPlayerDisplacementComponent::ComputeDisplacementState(chaos::box2 &pawn_box, bool jump_pressed, glm::vec2 const & stick_position, PlayerDisplacementCollisionFlags collision_flags)
{
	glm::vec2& pawn_position = pawn_box.position;

	bool touching_ceil   = (collision_flags & PlayerDisplacementCollisionFlags::TOUCHING_CEIL);
	bool touching_floor  = (collision_flags & PlayerDisplacementCollisionFlags::TOUCHING_FLOOR);
	bool touching_bridge = (collision_flags & PlayerDisplacementCollisionFlags::TOUCHING_BRIDGE);
	bool touching_wall   = (collision_flags & PlayerDisplacementCollisionFlags::TOUCHING_WALL);
	bool touching_ladder = (collision_flags & PlayerDisplacementCollisionFlags::TOUCHING_LADDER);

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


	int const attack_key_buttons[] = { GLFW_KEY_LEFT_CONTROL, GLFW_KEY_RIGHT_CONTROL, -1 };

	// get player position
	chaos::box2 pawn_box = pawn->GetBoundingBox();
	glm::vec2& pawn_position = pawn_box.position;

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
		float run_factor = (run_pressed && displacement_state != PlayerDisplacementState::CLIMBING) ? displacement_info.run_velocity_factor : 1.0f;

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
	pawn_position += pawn_velocity * delta_time;

	// extend the pawn box for ground/wall collision
	chaos::box2 extended_pawn_box = pawn_box;
	extended_pawn_box.half_size += displacement_info.pawn_box_extend;

	// get colliding tiles
	std::vector<death::TileCollisionInfo> colliding_tiles;

	death::TMLevelInstance* level_instance = GetLevelInstance();
	if (level_instance != nullptr)
	{
		death::TMTileCollisionIterator it = level_instance->GetTileCollisionIterator(extended_pawn_box, death::CollisionMask::PLAYER);

		while (it)
		{
			if (it->allocation == pawn->GetAllocation())
			{
				it.NextAllocation();
				continue;
			}
			colliding_tiles.push_back(*it);
			++it;
		}
	}

	// compute collisions and keep trace of all collided objects
	PlayerDisplacementCollisionFlags collision_flags = ApplyCollisionsToPlayer(pawn_box, colliding_tiles);

	// update player state
	displacement_state = ComputeDisplacementState(pawn_box, jump_pressed, stick_position, collision_flags);

	pawn_velocity = ClampPlayerVelocity(pawn_velocity, run_pressed && displacement_state != PlayerDisplacementState::CLIMBING);

	// update the player pawn
	pawn->SetPosition(pawn_box.position);

	was_jump_pressed = jump_pressed;

	return true;
}
