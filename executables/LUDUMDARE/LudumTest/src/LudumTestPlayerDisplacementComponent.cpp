#include "LudumTestPlayer.h"
#include "LudumTestPlayerDisplacementComponent.h"
#include "LudumTestLevel.h"
#include "LudumTestLevelInstance.h"
#include "LudumTestGame.h"
#include "LudumTestGameInstance.h"
#include "LudumTestParticles.h"
#include "LudumTestGameCheckpoint.h"



glm::vec2 LudumPlayerDisplacementComponent::ClampPlayerVelocity(glm::vec2 velocity) const
{
	for (int direction = 0; direction <= 1; ++direction)
		if (displacement_info.max_pawn_velocity[direction] >= 0.0f && std::abs(velocity[direction]) > displacement_info.max_pawn_velocity[direction])
			velocity[direction] = displacement_info.max_pawn_velocity[direction] * ((velocity[direction] > 0.0f) ? +1.0f : -1.0f);
	return velocity;
}

chaos::box2 LudumPlayerDisplacementComponent::ExtendBox(chaos::box2 const& src, float left, float right, float bottom, float top) const
{
	std::pair<glm::vec2, glm::vec2> box_extremums = chaos::GetBoxExtremums(src);
	box_extremums.first.x -= left;
	box_extremums.first.y -= bottom;
	box_extremums.second.x += right;
	box_extremums.second.y += top;
	return chaos::box2(box_extremums);
}

PlayerDisplacementCollisionFlags LudumPlayerDisplacementComponent::ApplyCollisionsToPlayer(chaos::box2& box, std::vector<death::TileParticleCollisionInfo> const& colliding_tiles)
{
	PlayerDisplacementCollisionFlags result = PlayerDisplacementCollisionFlags::NOTHING;

	for (death::TileParticleCollisionInfo const& collision : colliding_tiles)
	{
		// search the kind of object pawn is touching
		bool is_wall = false;
		bool is_bridge = false;
		bool is_ladder = false;

		if (chaos::TiledMapTools::IsObjectOfType(collision.tile_info.tiledata, "WALL"))
			is_wall = true;
		else if (chaos::TiledMapTools::IsObjectOfType(collision.tile_info.tiledata, "BRIDGE"))
			is_bridge = true;
		else if (chaos::TiledMapTools::IsObjectOfType(collision.tile_info.tiledata, "LADDER"))
			is_ladder = true;
		else
			continue;

		// there are only "HARD collision" with wall and bridge
		if (is_ladder)
		{
			result = (PlayerDisplacementCollisionFlags)(result | PlayerDisplacementCollisionFlags::TOUCHING_LADDER);
			continue;
		}


		// keep the box outside the

		glm::vec2 displacement = chaos::GetRestrictToOutsideDisplacement(collision.particle.bounding_box, box);
		if (displacement == glm::vec2(0.0f, 0.0f))
			continue;

		// ========================= WALL / CEIL / FLOOR (all have type WALL) =========================
		if (is_wall)
		{
			if (displacement.x != 0.0f)
			{
				result = (PlayerDisplacementCollisionFlags)(result | PlayerDisplacementCollisionFlags::TOUCHING_WALL); // pushed LEFT or RIGHT
				if (pawn_velocity.x * displacement.y < 0.0f)
					pawn_velocity.x = 0.0f;
			}
			if (displacement.y < 0.0f)
				result = (PlayerDisplacementCollisionFlags)(result | PlayerDisplacementCollisionFlags::TOUCHING_CEIL); // pushed DOWN
			else if (displacement.y > 0.0f)
				result = (PlayerDisplacementCollisionFlags)(result | PlayerDisplacementCollisionFlags::TOUCHING_FLOOR); // pushed UP
		}
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

	// get player position
	chaos::box2 pawn_box = pawn->GetBox();
	glm::vec2& pawn_position = pawn_box.position;

	// sum the forces 
	glm::vec2 sum_forces = glm::vec2(0.0f, 0.0f);

	if (displacement_state == PlayerDisplacementState::FALLING || displacement_state == PlayerDisplacementState::JUMPING_DOWN) // do not fall otherway
		sum_forces += glm::vec2(0.0f, -displacement_info.gravity);

	// mode IMPULSE : pushing the stick in 1 direction create an impulse (velocity is immediatly set)

#if 1
	// pawn is breaking
	if (stick_position.x == 0.0f)
		pawn_velocity.x = pawn_velocity.x * std::pow(displacement_info.pawn_break_ratio, delta_time);
	// pawn is accelerating forward
	else if (stick_position.x * pawn_velocity.x >= 0.0f)
		pawn_velocity.x = pawn_velocity.x + stick_position.x * displacement_info.pawn_impulse.x * delta_time;
	// pawn is changing direction (break harder)
	else if (stick_position.x * pawn_velocity.x < 0.0f)
		pawn_velocity.x = 
			pawn_velocity.x * std::pow(displacement_info.pawn_hardturn_break_ratio, delta_time) +
			stick_position.x * displacement_info.pawn_impulse.x * delta_time;

#else

	pawn_velocity.x =  stick_position.x * displacement_info.pawn_impulse.x;

#endif

	if (displacement_state == PlayerDisplacementState::GROUNDED)
	{
		current_jump_count = 0;
		pawn_velocity.y = 0.0f;
	}		
	else if (displacement_state == PlayerDisplacementState::CLIMBING)
	{
		current_jump_count = 0;
		pawn_velocity.y = stick_position.y * displacement_info.climp_velocity;
	}
	else if (displacement_state == PlayerDisplacementState::JUMPING)
	{
		current_jump_timer = std::min(current_jump_timer + delta_time, GetMaxJumpDuration());
		pawn_position.y = current_jump_start_y + GetJumpRelativeHeight(current_jump_timer);
		pawn_velocity.y = 0.0f;
	}
	else if (displacement_state == PlayerDisplacementState::JUMPING_DOWN || displacement_state == PlayerDisplacementState::FALLING)
	{
		pawn_velocity += (sum_forces * delta_time);
	}

	pawn_position += pawn_velocity * delta_time;

	// extend the pawn box for ground collision
	chaos::box2 extended_pawn_box = pawn_box;
	extended_pawn_box.half_size += displacement_info.pawn_box_extend;

	// get colliding tiles
	std::vector<death::TileParticleCollisionInfo> colliding_tiles;

	death::TiledMapLevelInstance* level_instance = GetLevelInstance();
	if (level_instance != nullptr)
		level_instance->FindPlayerTileCollisions(player, colliding_tiles, &extended_pawn_box);


	// compute collisions and keep trace of all collided objects
	PlayerDisplacementCollisionFlags collision_flags = ApplyCollisionsToPlayer(pawn_box, colliding_tiles);

	// update player state
	displacement_state = ComputeDisplacementState(pawn_box, jump_pressed, stick_position, collision_flags);

	pawn_velocity = ClampPlayerVelocity(pawn_velocity);

	// update the player pawn
	pawn->SetPosition(pawn_box.position);

	was_jump_pressed = jump_pressed;

	return true;
}
