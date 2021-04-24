#include "Ludum48PlayerDisplacementComponent.h"

bool LudumPlayerDisplacementComponent::DoTick(float delta_time)
{
	// early exit
	chaos::PlayerPawn* pawn = player->GetPawn();
	if (pawn == nullptr)
		return true;

	// get player inputs of interrests
	glm::vec2 stick_position = player->GetLeftStickPosition();
	stick_position.x = chaos::MathTools::AnalogicToDiscret(stick_position.x);
	stick_position.y = chaos::MathTools::AnalogicToDiscret(stick_position.y);
	if (stick_position.x != 0.0f)
		stick_position.y = 0.0f;

	// get player position
	chaos::box2 initial_pawn_box = pawn->GetBoundingBox();

	chaos::box2 pawn_box = initial_pawn_box;
	glm::vec2& pawn_position = pawn_box.position;

	// start displacement in given direction
	if (pawn_resting && stick_position != glm::vec2(0.0f, 0.0f))
	{
		start_pawn_position = pawn_position;
		pawn_velocity = stick_position;
		pawn_offset = 0.0f;
		pawn_resting = false;
	}

	// update displacement
	if (!pawn_resting)
	{
		for (size_t axis : {0, 1})
		{
			if (pawn_velocity[axis] == 0.0f)
				continue;
			pawn_offset = std::min(pawn_offset + pawn_max_velocity[axis] * delta_time, 1.0f);
		}

		pawn_position = start_pawn_position + pawn_offset * pawn_velocity * tile_size;
			
		if (pawn_offset == 1.0f)
			pawn_resting = true;



		// update the player pawn
		pawn->SetPosition(pawn_box.position);
	}

	return true;
}

#if 0



bool PlatformerDisplacementComponent::DoTick(float delta_time)
{
	// early exit
	PlayerPawn* pawn = player->GetPawn();
	if (pawn == nullptr)
		return true;

	// get player inputs of interrests
	glm::vec2 stick_position = player->GetLeftStickPosition();
	if (displacement_info.discrete_stick_mode)
	{
		stick_position.x = MathTools::AnalogicToDiscret(stick_position.x);
		stick_position.y = MathTools::AnalogicToDiscret(stick_position.y);
	}

	Key const jump_key_buttons[] = { KeyboardButton::SPACE, GamepadButton::A, Key() };
	bool jump_pressed = player->CheckButtonPressed(jump_key_buttons);

	Key const run_key_buttons[] = { KeyboardButton::LEFT_SHIFT, KeyboardButton::RIGHT_SHIFT, GamepadButton::RIGHT_TRIGGER, Key() };
	bool run_pressed = player->CheckButtonPressed(run_key_buttons);

	// get player position
	box2 initial_pawn_box = pawn->GetBoundingBox();

	box2 pawn_box = initial_pawn_box;
	glm::vec2& pawn_position = pawn_box.position;

	// sum the forces 
	glm::vec2 sum_forces = glm::vec2(0.0f, 0.0f);

	if (displacement_state == PlatformerDisplacementState::FALLING || displacement_state == PlatformerDisplacementState::JUMPING_DOWN) // do not fall otherway
		sum_forces += glm::vec2(0.0f, -displacement_info.gravity);

	// compute horizonral velocity (based on uniform acceleration or not for climbing)
	if (displacement_state == PlatformerDisplacementState::CLIMBING)
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
	if (displacement_state == PlatformerDisplacementState::GROUNDED)
	{
		current_jump_count = 0;
		pawn_velocity.y = 0.0f;
	}
	else if (displacement_state == PlatformerDisplacementState::CLIMBING)
	{
		current_jump_count = 0;
		pawn_velocity.y = stick_position.y * displacement_info.climp_velocity.y;
	}
	else if (displacement_state == PlatformerDisplacementState::JUMPING)
	{
		current_jump_timer = std::min(current_jump_timer + delta_time, GetMaxJumpDuration());
		pawn_position.y = current_jump_start_y + GetJumpRelativeHeight(current_jump_timer);
		pawn_velocity.y = 0.0f; // no physics (ignore gravity, velocity) for the jump, just use a curve for the height of the player (even if that curve is based on physical equations)
	}
	else if (displacement_state == PlatformerDisplacementState::JUMPING_DOWN || displacement_state == PlatformerDisplacementState::FALLING)
	{
		pawn_velocity += (sum_forces * delta_time);
	}

	// update pawn position
	pawn_velocity = ClampPlayerVelocity(pawn_velocity, run_pressed && displacement_state != PlatformerDisplacementState::CLIMBING);
	pawn_position += pawn_velocity * delta_time;

	// search collisions, apply collision reaction
	int collision_flags = PlatformerDisplacementCollisionFlags::NOTHING;

	char const* wangset_name = nullptr;  //"CollisionPlatformer"; 

	TileCollisionComputer computer = TileCollisionComputer(GetLevelInstance(), initial_pawn_box, pawn_box, CollisionMask::PLAYER, pawn->GetAllocation(), displacement_info.pawn_extend, wangset_name);

	pawn_box = computer.Run([&computer, &collision_flags](TileCollisionInfo const& collision_info)
	{
		if ((collision_info.particle->flags & PlatformerParticleFlags::LADDER) != 0)
		{
			if (Collide(computer.dst_box, collision_info.particle->bounding_box))
			{
				collision_flags |= PlatformerDisplacementCollisionFlags::TOUCHING_LADDER;
			}
		}
		else
		{
			computer.ComputeReaction(collision_info, [&collision_flags](TileCollisionInfo const& collision_info, Edge edge)
			{
				if (edge == Edge::TOP)
				{
					if ((collision_info.particle->flags & PlatformerParticleFlags::BRIDGE) != 0)
						collision_flags |= PlatformerDisplacementCollisionFlags::TOUCHING_BRIDGE;
					collision_flags |= PlatformerDisplacementCollisionFlags::TOUCHING_FLOOR;
				}
				else if (edge == Edge::BOTTOM)
					collision_flags |= PlatformerDisplacementCollisionFlags::TOUCHING_CEIL;
				else if (edge == Edge::LEFT || edge == Edge::RIGHT)
					collision_flags |= PlatformerDisplacementCollisionFlags::TOUCHING_WALL;
				return true;
			});
		}
	});

	// update player state
	displacement_state = ComputeDisplacementState(pawn_box, jump_pressed, stick_position, collision_flags);

	// update the player pawn
	pawn->SetPosition(pawn_box.position);

	was_jump_pressed = jump_pressed;

	return true;
	}


#endif