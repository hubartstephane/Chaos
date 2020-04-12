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
		if (max_pawn_velocity[direction] >= 0.0f && std::abs(velocity[direction]) > max_pawn_velocity[direction])
			velocity[direction] = max_pawn_velocity[direction] * ((velocity[direction] > 0.0f) ? +1.0f : -1.0f);
	return velocity;
}

PlayerDisplacementCollisionFlags LudumPlayerDisplacementComponent::ApplyCollisionsToPlayer(chaos::box2& pawn_box, std::vector<death::TileParticleCollisionInfo> const& colliding_tiles) const
{
	PlayerDisplacementCollisionFlags result = PlayerDisplacementCollisionFlags::NOTHING;

	for (death::TileParticleCollisionInfo const& collision : colliding_tiles)
	{
		// search the kind of object pawn is touching
		bool is_wall = false;
		bool is_bridge = false;
		bool is_ladder = false;

		if (chaos::TiledMapTools::IsObjectOfType(collision.tile_info.tiledata, "WALL"))
		{
			is_wall = true;
		}
		else if (chaos::TiledMapTools::IsObjectOfType(collision.tile_info.tiledata, "BRIDGE"))
		{
			result = (PlayerDisplacementCollisionFlags)(result | PlayerDisplacementCollisionFlags::TOUCHING_BRIDGE);
			is_bridge = true;
		}
		else if (chaos::TiledMapTools::IsObjectOfType(collision.tile_info.tiledata, "LADDER"))
		{
			result = (PlayerDisplacementCollisionFlags)(result | PlayerDisplacementCollisionFlags::TOUCHING_LADDER);
			is_ladder = true;
		}

		// there are only "HARD collision" with wall and bridge
		if (!is_bridge && !is_wall)
			continue;

		// keep the player outside the
		chaos::box2 new_pawn_box = pawn_box;
		if (chaos::RestrictToOutside(collision.particle.bounding_box, new_pawn_box))
		{




			pawn_box = new_pawn_box;
		}
	}

#if 0

		if (new_pawn_box.position.y > pawn_box.position.y&& pawn_velocity.y < 0.0f)
		{
			initial_pawn_position.y = new_pawn_box.position.y; // force Y velociy to 0 after the loop
			touching_floor = true;
		}
		// touching the ceil
		if (new_pawn_box.position.y < pawn_box.position.y && pawn_velocity.y > 0.0f)
		{
			initial_pawn_position.y = new_pawn_box.position.y; // force Y velociy to 0 after the loop
			touching_ceil = true;
		}
		// touching walls
		if ((new_pawn_box.position.x - pawn_box.position.x) * (pawn_velocity.x) < 0.0f)
		{
			initial_pawn_position.x = new_pawn_box.position.x; // force X velociy to 0 after the loop
			touching_wall = true;
		}


	}

#endif

#if 0

	for (death::TileParticleCollisionInfo const & collision : colliding_tiles)
	{







		// WALLS
		chaos::box2 new_pawn_box = pawn_box;
		if (chaos::RestrictToOutside(collision.particle.bounding_box, new_pawn_box))
		{
			bool is_wall = true;
			bool is_ladder = false;
			bool can_traverse_wall = false;


			// touching the floor
			if (new_pawn_box.position.y > pawn_box.position.y&& pawn_velocity.y < 0.0f)
			{
				initial_pawn_position.y = new_pawn_box.position.y; // force Y velociy to 0 after the loop
				touching_floor = true;
			}
			// touching the ceil
			if (new_pawn_box.position.y < pawn_box.position.y && pawn_velocity.y > 0.0f)
			{
				initial_pawn_position.y = new_pawn_box.position.y; // force Y velociy to 0 after the loop
				touching_ceil = true;
			}
			// touching walls
			if ((new_pawn_box.position.x - pawn_box.position.x) * (pawn_velocity.x) < 0.0f)
			{
				initial_pawn_position.x = new_pawn_box.position.x; // force X velociy to 0 after the loop
				touching_wall = true;
			}
		}
		pawn_box = new_pawn_box;
	}
#endif

	return result;
}

bool LudumPlayerDisplacementComponent::DoTick(float delta_time)
{
	// early exit
	death::PlayerPawn* pawn = player->GetPawn();
	if (pawn == nullptr)
		return true;

	// get colliding tiles
	std::vector<death::TileParticleCollisionInfo> colliding_tiles;

	death::TiledMapLevelInstance * level_instance = GetLevelInstance();
	if (level_instance != nullptr)
		level_instance->FindPlayerTileCollisions(player, colliding_tiles);

	// get player inputs of interrests
	glm::vec2 stick_position = player->GetLeftStickPosition();
	if (!analogic_stick_mode)
	{
		stick_position.x = chaos::MathTools::AnalogicToDiscret(stick_position.x);
		stick_position.y = chaos::MathTools::AnalogicToDiscret(stick_position.y);
	}

	int const jump_key_buttons[] = { GLFW_KEY_SPACE, -1 };
	bool jump_pressed = player->CheckButtonPressed(jump_key_buttons, chaos::XBoxButton::BUTTON_A);

	// get player position
	chaos::box2 pawn_box = pawn->GetBox();
	
	glm::vec2 initial_pawn_position = pawn_box.position;

	// sum the forces 
	glm::vec2 sum_forces = glm::vec2(0.0f, 0.0f);

	if (player_state == PlayerDisplacementState::FALLING) 
		sum_forces += glm::vec2(0.0f, -gravity);

	
	// mode IMPULSE : pushing the stick in 1 direction create an impulse (velocity is immediatly set)
	pawn_velocity.x = stick_position.x * pawn_impulse.x;



	

	

	// clamp the pawn velocity/acceleration












	

	// handle the collisions
	bool touching_floor = false;
	bool touching_ceil  = false;
	bool touching_wall  = false;

	for (death::TileParticleCollisionInfo& collision : colliding_tiles)
	{
		// WALLS
		chaos::box2 new_pawn_box = pawn_box;
		if (chaos::RestrictToOutside(collision.particle.bounding_box, new_pawn_box))
		{
			bool is_wall = true;
			bool is_ladder = false;
			bool can_traverse_wall = false;


			// touching the floor
			if (new_pawn_box.position.y > pawn_box.position.y && pawn_velocity.y < 0.0f)
			{
				initial_pawn_position.y = new_pawn_box.position.y; // force Y velociy to 0 after the loop
				touching_floor = true;
			}
			// touching the ceil
			if (new_pawn_box.position.y < pawn_box.position.y && pawn_velocity.y > 0.0f)
			{
				initial_pawn_position.y = new_pawn_box.position.y; // force Y velociy to 0 after the loop
				touching_ceil = true;
			}
			// touching walls
			if ((new_pawn_box.position.x - pawn_box.position.x) * (pawn_velocity.x) < 0.0f)
			{
				initial_pawn_position.x = new_pawn_box.position.x; // force X velociy to 0 after the loop
				touching_wall = true;
			}
		}		
		pawn_box = new_pawn_box;
	}

	//
	if (jump_pressed)
	{
		// try to start a new jump 
		if (player_state != PlayerDisplacementState::JUMPING)
		{
			if (current_jump_count < max_jump_count) // only 
			{
				++current_jump_count;
				current_jump_start_y = pawn_box.position.y;
				pawn_velocity.y = jump_velocity;
				player_state = PlayerDisplacementState::JUMPING;
			}
		}
		// continue new 
		else
		{
			// 
			if (pawn_box.position.y - current_jump_start_y < max_jump_height)
			{
				pawn_velocity.y = jump_velocity;


			}
			else
			{

			}

		}



	}


	// update velocity with force, then clamp
	pawn_velocity = ClampPlayerVelocity(pawn_velocity + sum_forces * delta_time);


	// update internals
	pawn_box.position += pawn_velocity * delta_time;



	// do not compute velocity with acceleration : just take the difference of positions
	if (delta_time == 0.0f)
		pawn_velocity = glm::vec2(0.0f, 0.0f);
	else
		pawn_velocity = (pawn_box.position - initial_pawn_position) / delta_time;

	pawn_velocity = ClampPlayerVelocity(pawn_velocity);

	// update the player pawn
	pawn->SetPosition(pawn_box.position);

	return true;
}
