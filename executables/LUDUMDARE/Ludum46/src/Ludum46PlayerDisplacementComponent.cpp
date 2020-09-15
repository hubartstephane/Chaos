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

template<typename T>
static bool IsInRange(T value, T min_value, T max_value)
{
	if (value <= min_value || value >= max_value)
		return false;
	return true;
}

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


	death::TMParticle* ppp = nullptr;



	std::vector<death::TileCollisionInfo> colliding_tiles;

	death::TMLevelInstance* level_instance = GetLevelInstance();
	if (level_instance != nullptr)
	{
		death::TMTileCollisionIterator it = level_instance->GetTileCollisionIterator(next_pawn_box | pawn_box, death::CollisionMask::PLAYER, true); // collision over the extended bounding box



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

			auto particle_corners = chaos::GetBoxCorners(it->particle->bounding_box);


			int particle_flags = it->particle->flags;


			float best_distance = std::numeric_limits<float>::max();
			glm::vec2 best_center;

			auto next_pawn_corner = chaos::GetBoxCorners(next_pawn_box);

			if ((particle_flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_LEFT) == 0) // only test LEFT side if no side neighbour
			{
				if (IsInRange(particle_corners.first.x, next_pawn_corner.first.x, next_pawn_corner.second.x))

				//if (next_pawn_box.position.x > pawn_box.position.x)
				{
					float new_x = particle_corners.first.x - next_pawn_box.half_size.x;

					float d = std::abs(new_x - next_pawn_box.position.x);
					if (d < best_distance)
					{
						best_distance = d;
						best_center.x = new_x;
						best_center.y = next_pawn_box.position.y;
					}
				}
			}










			if ((particle_flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_RIGHT) == 0)
			{
				//if (next_pawn_box.position.x < pawn_box.position.x)
				if (IsInRange(particle_corners.second.x, next_pawn_corner.first.x, next_pawn_corner.second.x))
				{
					float new_x = particle_corners.second.x + next_pawn_box.half_size.x;

					float d = std::abs(new_x - next_pawn_box.position.x);
					if (d < best_distance)
					{
						best_distance = d;
						best_center.x = new_x;
						best_center.y = next_pawn_box.position.y;
					}
				}
			}

			if ((particle_flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_TOP) == 0)
			{
				//if (next_pawn_box.position.y < pawn_box.position.y)
				if (IsInRange(particle_corners.second.y, next_pawn_corner.first.y, next_pawn_corner.second.y))
				{
					float new_y = particle_corners.second.y + next_pawn_box.half_size.y;

					float d = std::abs(new_y - next_pawn_box.position.y);
					if (d < best_distance)
					{
						best_distance = d;
						best_center.x = next_pawn_box.position.x;
						best_center.y = new_y;
					}
				}
			}
			
			
			if ((particle_flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_BOTTOM) == 0)
			{
				//if (next_pawn_box.position.y > pawn_box.position.y)
				if (IsInRange(particle_corners.first.y, next_pawn_corner.first.y, next_pawn_corner.second.y))
				{
					float new_y = particle_corners.first.y - next_pawn_box.half_size.y;

					float d = std::abs(new_y - next_pawn_box.position.y);
					if (d < best_distance)
					{
						best_distance = d;
						best_center.x = next_pawn_box.position.x;
						best_center.y = new_y;
					}
				}
			}

			if (best_distance < std::numeric_limits<float>::max())
			{
				next_pawn_box.position = best_center;
				ppp = it->particle;
			}


			colliding_tiles.push_back(*it);
			++it;
		}
	}


	
























	if (ppp != nullptr)
	{
		ppp->color.x = chaos::MathTools::RandFloat();
		ppp->color.y = chaos::MathTools::RandFloat();
		ppp->color.z = chaos::MathTools::RandFloat();
	}



	pawn->SetBoundingBox(next_pawn_box);

	return true;
}
