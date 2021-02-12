#include <chaos/Chaos.h>

namespace chaos
{

	// =====================================
	// TileCollisionComputer implementation
	// =====================================

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
	//         +----+                  => ignore edges that are shared by 2 tiles
	//            |
	//          EDGE A        => on that purpose we can use automatic computation with NEIGHBOURING
	//                        => or we can use manual made wangsets
	//
	//
	//        +-----+ PAWN
	//        |     |
	//        |     |        PAWN only reacts to EDGE B for collision
	//   +-------+  |        if PAWN was colliding with EDGE A, this would produce PAWN teleport at the full left of the tile
	//   |    |  |  |
	//   |       |
	//   |       |
	// EDGE A  EDGE B
	//
	// -------------------
	// Collision Artifacts
	// -------------------
	//
	// alternating collision/no collision state
	// ----------------------------------------
	//
	// For a falling PAWN, as soon as the PAWN interpenetrate the ground, it is pushed upward. This leads to situation where the pawn is falling for one frame and is grounded the other
	//
	//      ^ pushed upward                                        +-------+ PAWN is FALLING
	//      |                                                      |       |
	//  +-------+ PAWN is GROUNDED                                 |       |
	//  |       |                          <-------------->        |       |
	//  |       |                                                  +-------+
	//  | +----------------+                                          +----------------+
	//  +-|-----+          |                                          |                |
 	//    |                |                                          |                |
	//
	//
	//
	// To solve with issue we use extended collisions : We define 3 zones for the PAWN
	// 
	//
	// +----------------+        -XXXX zone is strictly the PAWN 
	// |     ZONE 2     |
	// |  +----------+  |        -ZONE 1 : if collision happens inside ZONE 1, the PAWN reacts and is pushed away so that the collision is now at the limit of ZONE 1 and ZONE 2
	// |  |  ZONE 1  |  |
	// |  |  +----+  |  |        -ZONE 2 : if collision happens inside ZONE 2, the collision is detected (means objects know they are touching each other) but there is no repulsion
	// |  |  |XXXX|  |  | 
	// |  |  |XXXX|  |  |
	// |  |  +----+  |  |
	// |  |          |  |
	// |  +----------+  |
	// |       +---------------------+
	// +-------|--------+            |
	//         |                     |
	//
	//
	//      +----+
	//      |    |
	//      +----+
	//      |    |           Whenever checking for collision between an EDGE and ZONE 1/ZONE 2,
	//      +----+           we do not use extended box among all direction
	//      |XXXX|
	//      |XXXX|
	//      +----+
	//      |    |
	//      +----+         EDGE
	//      |    |  +----------------+
	//      +----+  |                |
	//              |                |
	//
	// Slow collision
	// --------------
	//
	//   Whenever PAWN is approching a wall, it can slowly approach from ZONE 1 to ZONE 2. This should pratically be invisible if the extends for ZONE 1/ZONE 2 is small
	//
	//
	// Slickery collision
	// ------------------
	//
	// +----------------+ PAWN         The previous changes, can lead to situation where the two objets want to go further one another
	// |     ZONE 2     |              but the collision become slickery
	// |                |
	// |     +----+     |  move        to solve this issue we check for relative velocity of both objects (we store PAWN previous position)
	// |     |    |     | <----        if the 2 objects are naturally going further one another, we ignore collisions
	// |     +----+     |
	//               +-----------
	//               |

	static bool RangeOverlaps(std::pair<glm::vec2, glm::vec2> const & range1, std::pair<glm::vec2, glm::vec2> const& range2, int component)
	{
		if (range1.first[component] > range2.second[component] || range1.second[component] < range2.first[component])
			return false;
		return true;
	}

	box2 TileCollisionComputer::Run(std::function<void(TileCollisionInfo const& collision_info)> func)
	{
		assert(level_instance != nullptr);

		// work on extended copy of the box
		box2 extended_box = src_box | dst_box;

		glm::vec2 delta = box_extend;
		extended_box.half_size += delta;

		glm::vec2 delta_position = dst_box.position - src_box.position;

		// iterate over all particles
		TMTileCollisionIterator it = level_instance->GetTileCollisionIterator(extended_box, collision_mask, false);
		while (it)
		{
			// ignore pawn allocation
			if (it->allocation == ignore_allocation)
			{
				it.NextAllocation();
				continue;
			}
			// give caller opportunity to do what he wants
			func(*it);
			// next
			++it;
		}
		return dst_box;
	}

	void TileCollisionComputer::ComputeReaction(TileCollisionInfo const& collision_info, std::function<bool(TileCollisionInfo const &, Edge)> func)
	{
		int particle_flags = collision_info.particle->flags;

		// search for wang flags (use cache for faster search)
		TiledMap::WangTile wangtile;

		if (wangset_name != nullptr)
		{
			// cannot do anything without a known tileset : this should never happen
			if (collision_info.tile_info.tileset == nullptr)
				return;
			// different tileset than the one for previous particle ?
			if (collision_info.tile_info.tileset != tileset)
			{
				tileset = collision_info.tile_info.tileset;
				wangset = tileset->FindWangset(wangset_name);
			}
			// no wangset cannot continue
			if (wangset == nullptr)
				return;
			// get the wang tile and apply particle transforms
			wangtile = wangset->GetWangTile(collision_info.tile_info.id);
			wangtile.ApplyParticleFlags(particle_flags);
		}

		std::pair<glm::vec2, glm::vec2> particle_corners = GetBoxCorners(collision_info.particle->bounding_box);
		std::pair<glm::vec2, glm::vec2> dst_corners = GetBoxCorners(dst_box);

		// XXX : an edge with wang value 
		//         0 -> the tile does not use the wangset at all
		//         1 -> this is the empty wang value
		//        +2 -> good

		glm::vec2 best_position = dst_box.position;
		float best_distance = std::numeric_limits<float>::max();

		// LEFT EDGE
		bool left_collision_candidate = (wangset != nullptr) ? (wangtile.GetEdgeValue(Edge::LEFT) > 1) : ((particle_flags & TiledMap::TileParticleFlags::NEIGHBOUR_LEFT) == 0);

		// check whether the EDGE is valid and whether the distance between the objects does no increase
		if (left_collision_candidate && delta_position.x >= 0.0f)
		{
			// check whether EDGE/BOX collision may happen
			if (MathTools::IsInRange(particle_corners.first.x, dst_box.position.x, dst_corners.second.x + box_extend.x) && RangeOverlaps(dst_corners, particle_corners, 1))
			{
				// check whether the collision is at least in ZONE 2
				if (particle_corners.first.x < dst_corners.second.x + box_extend.x)
				{
					bool displacement_enabled = func(collision_info, Edge::LEFT); // ZONE 1 or 2 : indicates to caller that there is a touch

					// in ZONE 1 ?
					if (displacement_enabled && (particle_corners.first.x < dst_corners.second.x + box_extend.x * 0.5f))
					{
						float new_x = particle_corners.first.x - dst_box.half_size.x - box_extend.x * 0.5f;

						float distance = std::abs(dst_box.position.x - new_x);

						// check for best distance among other edges
						if (distance < best_distance)
						{
							best_position.x = new_x;
							best_position.y = dst_box.position.y;
							best_distance = distance;
						}
					}
				}
			}
		}

		// RIGHT EDGE
		bool right_collision_candidate = (wangset != nullptr) ? (wangtile.GetEdgeValue(Edge::RIGHT) > 1) : ((particle_flags & TiledMap::TileParticleFlags::NEIGHBOUR_RIGHT) == 0);

		// check whether the EDGE is valid and whether the distance between the objects does no increase
		if (right_collision_candidate && delta_position.x <= 0.0f)
		{
			// check whether EDGE/BOX collision may happen
			if (MathTools::IsInRange(particle_corners.second.x, dst_corners.first.x - box_extend.x, dst_box.position.x) && RangeOverlaps(dst_corners, particle_corners, 1))
			{
				// check whether the collision is at least in ZONE 2
				if (particle_corners.second.x > dst_corners.first.x - box_extend.x)
				{
					bool displacement_enabled = func(collision_info, Edge::RIGHT); // ZONE 1 or 2 : indicates to caller that there is a touch

					// in ZONE 1 ?
					if (displacement_enabled && (particle_corners.second.x > dst_corners.first.x - box_extend.x * 0.5f))
					{
						float new_x = particle_corners.second.x + dst_box.half_size.x + box_extend.x * 0.5f;

						float distance = std::abs(dst_box.position.x - new_x);

						// check for best distance among other edges
						if (distance < best_distance)
						{
							best_position.x = new_x;
							best_position.y = dst_box.position.y;
							best_distance = distance;
						}
					}
				}
			}
		}

		// BOTTOM EDGE
		bool bottom_collision_candidate = (wangset != nullptr) ? (wangtile.GetEdgeValue(Edge::BOTTOM) > 1) : ((particle_flags & TiledMap::TileParticleFlags::NEIGHBOUR_BOTTOM) == 0);

		// check whether the EDGE is valid and whether the distance between the objects does no increase
		if (bottom_collision_candidate && delta_position.y >= 0.0f)
		{
			// check whether EDGE/BOX collision may happen
			if (MathTools::IsInRange(particle_corners.first.y, dst_box.position.y, dst_corners.second.y + box_extend.y) && RangeOverlaps(dst_corners, particle_corners, 0))
			{
				// check whether the collision is at least in ZONE 2
				if (particle_corners.first.y < dst_corners.second.y + box_extend.y)
				{
					bool displacement_enabled = func(collision_info, Edge::BOTTOM); // ZONE 1 or 2 : indicates to caller that there is a touch

					// in ZONE 1 ?
					if (displacement_enabled && (particle_corners.first.y < dst_corners.second.y + box_extend.y * 0.5f))
					{
						float new_y = particle_corners.first.y - dst_box.half_size.y - box_extend.y * 0.5f;

						float distance = std::abs(dst_box.position.y - new_y);

						// check for best distance among other edges
						if (distance < best_distance)
						{
							best_position.x = dst_box.position.x;
							best_position.y = new_y;
							best_distance = distance;
						}
					}
				}
			}
		}

		// TOP EDGE
		bool top_collision_candidate = (wangset != nullptr) ? (wangtile.GetEdgeValue(Edge::TOP) > 1) : ((particle_flags & TiledMap::TileParticleFlags::NEIGHBOUR_TOP) == 0);

		// check whether the EDGE is valid and whether the distance between the objects does no increase
		if (top_collision_candidate && delta_position.y <= 0.0f)
		{
			// check whether EDGE/BOX collision may happen
			if (MathTools::IsInRange(particle_corners.second.y, dst_corners.first.y - box_extend.y, dst_box.position.y) && RangeOverlaps(dst_corners, particle_corners, 0))
			{
				// check whether the collision is at least in ZONE 2
				if (particle_corners.second.y > dst_corners.first.y - box_extend.y)
				{
					bool displacement_enabled = func(collision_info, Edge::TOP); // ZONE 1 or 2 : indicates to caller that there is a touch

					// in ZONE 1 ?
					if (displacement_enabled && (particle_corners.second.y > dst_corners.first.y - box_extend.y * 0.5f))
					{
						float new_y = particle_corners.second.y + dst_box.half_size.y + box_extend.y * 0.5f;

						float distance = std::abs(dst_box.position.y - new_y);

						// check for best distance among other edges
						if (distance < best_distance)
						{
							best_position.x = dst_box.position.x;
							best_position.y = new_y;
							best_distance = distance;
						}
					}
				}
			}
		}

		// displace the box among the best edge
		if (best_distance != std::numeric_limits<float>::max())
		{
			dst_box.position = best_position;
		}
	} 

}; // namespace chaos
