#include "Ludum46Player.h"
#include "Ludum46PlayerDisplacementComponent.h"
#include "Ludum46Level.h"
#include "Ludum46LevelInstance.h"
#include "Ludum46Game.h"
#include "Ludum46GameInstance.h"
#include "Ludum46Particles.h"


#include "chaos/TiledMap.h"
#include "chaos/MathTools.h"
#include "chaos/Direction.h"
#include "chaos/Edge.h"

#include "death/TM.h"
#include "death/CollisionMask.h"





//
//    +------+------+
//    |      |      |
//    |   <----->   |   if 2 tiles have a common edge, this edge does not count for collision
//    |      |      |
//    +------+------+
//
//    elsewhere this would make some strange artefacts collision with the pawn
//
//         +----+ PAWN
//         |    |
//    +----|--+-|----+    PAWN interpenetrates 2 tiles due to gravity
//    |    |  | |    |    EDGE A : wants to push the PAWN on the left or on the right => pawn cannot walk smoothly on the tiles
//         +----+                  => ignore the edge
//            |
//          EDGE A
//
//
//        +-----+ PAWN
//        |     |
//        |     |        PAWN only reacts to EDGE 2 for collision
//   +-------+  |        if PAWN was colliding with EDGE 1, this would produce PAWN teleport at the full left of the tile
//   |    |  |  |
//   |       |
//   |       |
// EDGE 1  EDGE 2
//




chaos::box2 LudumPlayerDisplacementComponent::ComputeCollisions(chaos::box2 const& src_box, chaos::box2 const& dst_box, int collision_mask, chaos::ParticleAllocationBase * ignore_allocation, char const * wangset_name, std::function<void(chaos::box2 const &, death::TMParticle &, chaos::Edge)> func)
{
	chaos::box2 result = dst_box;

	// early exit
	death::TMLevelInstance* level_instance = GetLevelInstance();
	if (level_instance == nullptr)
		return result;

	// collision over the extended bounding box
	death::TMTileCollisionIterator it = level_instance->GetTileCollisionIterator(src_box | dst_box, collision_mask, true);

	// for faster access, cache the wangset
	chaos::TiledMap::Wangset const* wangset = nullptr;
	chaos::TiledMap::TileSet const* tileset = nullptr;

	// iterate over all particles
	while (it)
	{
		// ignore pawn allocation
		if (it->allocation == ignore_allocation)
		{
			it.NextAllocation();
			continue;
		}
		// while dst_box may change, while the request box is extended, this collision maybe does not even happen
		if (!chaos::Collide(result, it->particle->bounding_box))
		{
			++it;
			continue;
		}

		// some data
		int particle_flags = it->particle->flags;

		// search for wang flags (use cache for faster search
		chaos::TiledMap::WangTile wangtile;

		if (wangset_name != nullptr)
		{
			// cannot do anything without a known tileset : this should never happen
			if (it->tile_info.tileset == nullptr)
				continue;

			// different tileset than the one for previous particle ?
			if (it->tile_info.tileset != tileset) 
			{
				tileset = it->tile_info.tileset;

				wangset = tileset->FindWangset(wangset_name);
			}
			// no wangset cannot continue
			if (wangset == nullptr)
				continue;

			// get the wang tile and apply particle transforms
			wangtile = wangset->GetWangTile(it->tile_info.id);
			wangtile.ApplyParticleFlags(particle_flags);
		}

		// search the displacement that is the smallest so that pawn becomes outside the particle
		float best_distance = std::numeric_limits<float>::max();
		glm::vec2 best_center;
		chaos::Edge best_edge;

		std::pair<glm::vec2, glm::vec2> particle_corners = chaos::GetBoxCorners(it->particle->bounding_box);
		std::pair<glm::vec2, glm::vec2> dst_corners = chaos::GetBoxCorners(result);

		// XXX : an edge with wang value 
		//         0 -> the tile does not use the wangset at all
		//         1 -> this is the empty wang value
		//        +2 -> good

		// only test LEFT side if no neighbour
		bool left_collision = (wangset != nullptr) ?
			(wangtile.GetEdgeValue(chaos::Edge::LEFT) > 1) :
			((particle_flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_LEFT) == 0);

		if (left_collision)
		{
			if (chaos::MathTools::IsInRange(particle_corners.first.x, dst_corners.first.x, dst_corners.second.x))
			{
				float new_x = particle_corners.first.x - result.half_size.x;

				float d = std::abs(new_x - result.position.x);
				if (d < best_distance)
				{
					best_distance = d;
					best_center.x = new_x;
					best_center.y = result.position.y;
					best_edge = chaos::Edge::LEFT;
				}
			}
		}

		// only test RIGHT side if no neighbour
		bool right_collision = (wangset != nullptr) ?
			(wangtile.GetEdgeValue(chaos::Edge::RIGHT) > 1) :
			((particle_flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_RIGHT) == 0);

		if (right_collision)
		{
			if (chaos::MathTools::IsInRange(particle_corners.second.x, dst_corners.first.x, dst_corners.second.x))
			{
				float new_x = particle_corners.second.x + result.half_size.x;

				float d = std::abs(new_x - result.position.x);
				if (d < best_distance)
				{
					best_distance = d;
					best_center.x = new_x;
					best_center.y = result.position.y;
					best_edge = chaos::Edge::RIGHT;
				}
			}
		}

		// only test TOP side if no neighbour
		bool top_collision = (wangset != nullptr) ?
			(wangtile.GetEdgeValue(chaos::Edge::TOP) > 1) :
			((particle_flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_TOP) == 0);

		if (top_collision)
		{
			if (chaos::MathTools::IsInRange(particle_corners.second.y, dst_corners.first.y, dst_corners.second.y))
			{
				float new_y = particle_corners.second.y + result.half_size.y;

				float d = std::abs(new_y - result.position.y);
				if (d < best_distance)
				{
					best_distance = d;
					best_center.x = result.position.x;
					best_center.y = new_y;
					best_edge = chaos::Edge::TOP;
				}
			}
		}

		// only test BOTTOM side if no neighbour
		bool bottom_collision = (wangset != nullptr) ?
			(wangtile.GetEdgeValue(chaos::Edge::BOTTOM) > 1) :
			((particle_flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_BOTTOM) == 0);

		if (bottom_collision)
		{
			if (chaos::MathTools::IsInRange(particle_corners.first.y, dst_corners.first.y, dst_corners.second.y))
			{
				float new_y = particle_corners.first.y - result.half_size.y;

				float d = std::abs(new_y - result.position.y);
				if (d < best_distance)
				{
					best_distance = d;
					best_center.x = result.position.x;
					best_center.y = new_y;
					best_edge = chaos::Edge::BOTTOM;
				}
			}
		}

		// if a displacement is found to stop the collision, apply it
		if (best_distance < std::numeric_limits<float>::max())
		{
			result.position = best_center;
			if (func != nullptr)
				func(result, *it->particle, best_edge);
		}
		++it;
	}
	return result;
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




	//next_pawn_box = ComputeCollisions(pawn_box, next_pawn_box, death::CollisionMask::PLAYER, pawn->GetAllocation(), "CollisionPlatformer", [](chaos::box2 const & b, death::TMParticle & p, chaos::Edge edge) 

	next_pawn_box = ComputeCollisions(pawn_box, next_pawn_box, death::CollisionMask::PLAYER, pawn->GetAllocation(), nullptr, [](chaos::box2 const& b, death::TMParticle& p, chaos::Edge edge)
	{
	
		p.color.x = chaos::MathTools::RandFloat();
		p.color.y = chaos::MathTools::RandFloat();
		p.color.z = chaos::MathTools::RandFloat();
	
	});



	pawn->SetBoundingBox(next_pawn_box);

	return true;
}
