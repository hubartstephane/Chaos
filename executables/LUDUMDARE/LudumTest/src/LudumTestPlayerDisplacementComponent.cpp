#include "LudumTestPlayer.h"
#include "LudumTestPlayerDisplacementComponent.h"
#include "LudumTestLevel.h"
#include "LudumTestLevelInstance.h"
#include "LudumTestGame.h"
#include "LudumTestGameInstance.h"
#include "LudumTestParticles.h"
#include "LudumTestGameCheckpoint.h"

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

	// update player displacement
	chaos::box2 pawn_box = pawn->GetBox();

	glm::vec2 stick_position = player->GetLeftStickPosition();
	if (!analogic_stick_mode)
	{
		stick_position.x = chaos::MathTools::AnalogicToDiscret(stick_position.x);
		stick_position.y = chaos::MathTools::AnalogicToDiscret(stick_position.y);
	}

	// sum the forces 
	glm::vec2 sum_forces = glm::vec2(0.0f, 0.0f);

	sum_forces += glm::vec2(0.0f, -gravity);

	
	// mode IMPULSE : pushing the stick in 1 direction create an impulse (velocity is immediatly set)
	if (impulse_mode)
	{
		pawn_velocity.x = stick_position.x * pawn_impulse.x;



	}
	// mode ACCELERATING : pushing the stick in 1 direction an acceleration (create an acceleration, velocity has an inertia)
	else
	{
		sum_forces += stick_position * pawn_acceleration;



	}



	


	// clamp the pawn velocity/acceleration
	for (int direction = 0; direction <= 1; ++direction)
	{
		if (max_pawn_velocity[direction] >= 0.0f && std::abs(pawn_velocity[direction]) > max_pawn_velocity[direction])
		{
			pawn_velocity[direction] = max_pawn_velocity[direction] * ((pawn_velocity[direction] > 0.0f)? +1.0f : -1.0f);
			sum_forces[direction] = 0.0f;
		}
	}

	

	// handle the collisions
	bool stopped_x = false;
	bool stopped_y = false;

	bool touching_floor = false;
	bool touching_ceil  = false;
	bool touching_wall  = false;

	for (death::TileParticleCollisionInfo& collision : colliding_tiles)
	{
		chaos::box2 new_pawn_box = pawn_box;
		if (chaos::RestrictToOutside(collision.particle.bounding_box, new_pawn_box))
		{
			// touching the floor
			if (!stopped_y && new_pawn_box.position.y > pawn_box.position.y)
			{
				pawn_velocity.y = 0.0f;
				sum_forces.y = 0.0f;
				stopped_y = true;
				touching_floor = true;
			}
			// touching the ceil
			if (!stopped_y && new_pawn_box.position.y < pawn_box.position.y)
			{
				pawn_velocity.y = 0.0f;
				sum_forces.y = 0.0f;
				stopped_y = true;
				touching_ceil = true;
			}
			// touching walls
			if (!stopped_x && new_pawn_box.position.x != pawn_box.position.x)
			{
				pawn_velocity.x = 0.0f;
				sum_forces.x = 0.0f;
				stopped_x = true;
				touching_wall = true;
			}
		}		
		pawn_box = new_pawn_box;

		if (stopped_x && stopped_y)
			break;
	}


	// update internals
	pawn_box.position += pawn_velocity * delta_time;
	pawn_velocity += sum_forces * delta_time;

	// update the player pawn
	pawn->SetPosition(pawn_box.position);

	return true;
}
