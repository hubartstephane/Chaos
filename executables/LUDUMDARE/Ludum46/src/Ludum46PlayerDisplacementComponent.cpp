#include "Ludum46Player.h"
#include "Ludum46PlayerDisplacementComponent.h"
#include "Ludum46Level.h"
#include "Ludum46LevelInstance.h"
#include "Ludum46Game.h"
#include "Ludum46GameInstance.h"
#include "Ludum46Particles.h"


#include "chaos/TiledMap.h"
#include "chaos/Direction.h"

#include "death/TM.h"
#include "death/CollisionMask.h"


bool LudumPlayerDisplacementComponent::DoTick(float delta_time)
{


	// early exit
	death::PlayerPawn* pawn = player->GetPawn();
	if (pawn == nullptr)
		return true;

	// get player inputs of interrests
	glm::vec2 stick_position = player->GetLeftStickPosition();

	stick_position.x = chaos::MathTools::AnalogicToDiscret(stick_position.x);
	stick_position.y = chaos::MathTools::AnalogicToDiscret(stick_position.y);

#if 0
	if (displacement_info.discrete_stick_mode)
	{
		stick_position.x = chaos::MathTools::AnalogicToDiscret(stick_position.x);
		stick_position.y = chaos::MathTools::AnalogicToDiscret(stick_position.y);
	}
#endif
	stick_position.y = -stick_position.y; // Y stick is inverted

	int const jump_key_buttons[] = { GLFW_KEY_SPACE, -1 };
	bool jump_pressed = player->CheckButtonPressed(jump_key_buttons, chaos::XBoxButton::BUTTON_A);

	int const run_key_buttons[] = { GLFW_KEY_LEFT_SHIFT, GLFW_KEY_RIGHT_SHIFT, -1 };
	bool run_pressed = player->CheckButtonPressed(run_key_buttons, chaos::XBoxButton::BUTTON_RIGHTTRIGGER);


	int const attack_key_buttons[] = { GLFW_KEY_LEFT_CONTROL, GLFW_KEY_RIGHT_CONTROL, -1 };

	// get player position
	chaos::box2 pawn_box = pawn->GetBoundingBox();

	chaos::box2 next_pawn_box = pawn_box;
	next_pawn_box.position += 200.0f * stick_position * delta_time;


	std::vector<death::TileCollisionInfo> colliding_tiles;

	death::TMLevelInstance* level_instance = GetLevelInstance();
	if (level_instance != nullptr)
	{
		death::TMTileCollisionIterator it = level_instance->GetTileCollisionIterator(next_pawn_box | pawn_box, death::CollisionMask::PLAYER);

		while (it)
		{
			if (it->allocation == pawn->GetAllocation())
			{
				it.NextAllocation();
				continue;
			}

			if (!chaos::Collide(next_pawn_box, it->particle->bounding_box))
			{
				++it;
				continue;
			}



			int particle_flags = it->particle->flags;


			if ((particle_flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_LEFT) == 0) // only test LEFT side if no side neighbour
			{
				if (next_pawn_box.position.x > pawn_box.position.x)
				{
					next_pawn_box.position.x = pawn_box.position.x;

				}
			}
			if ((particle_flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_RIGHT) == 0)
			{
				if (next_pawn_box.position.x < pawn_box.position.x)
				{
					next_pawn_box.position.x = pawn_box.position.x;
				}
			}

			if ((particle_flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_TOP) == 0)
			{
				if (next_pawn_box.position.y < pawn_box.position.y)
				{
					next_pawn_box.position.y = pawn_box.position.y;

				}


			}
			if ((particle_flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_BOTTOM) == 0)
			{

				if (next_pawn_box.position.y > pawn_box.position.y)
				{
					next_pawn_box.position.y = pawn_box.position.y;

				}
			}







			if (chaos::RestrictToOutside(it->particle->bounding_box, next_pawn_box))
			{
				level_instance = level_instance;
			}




			if (it->particle->flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_TOP)
			{




				level_instance = level_instance;
			}




			colliding_tiles.push_back(*it);
			++it;
		}
	}


	




























	pawn->SetBoundingBox(next_pawn_box);

	return true;
}
