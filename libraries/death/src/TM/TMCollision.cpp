#include <death/TM.h>




#include <chaos/Application.h>

#include <chaos/MyGLFWSingleWindowApplication.h> 






namespace death
{

	// =====================================
	// TMCollisionIteratorBase implementation
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
	//          EDGE A        => on that purpose we can use auomatic computation with NEIGHBOURING
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

	chaos::box2 ComputeTileCollisionAndReaction(TMLevelInstance* level_instance, chaos::box2 src_box, chaos::box2 dst_box, int collision_mask, chaos::ParticleAllocationBase* ignore_allocation, glm::vec2 const & box_extend,char const* wangset_name, std::function<void(TMParticle&, chaos::Edge)> func)
	{
		assert(level_instance != nullptr);

		// work on extended copy of the box
		chaos::box2 extended_box = src_box | dst_box;

		glm::vec2 delta = box_extend;
		extended_box.half_size += delta;

		TMTileCollisionIterator it = level_instance->GetTileCollisionIterator(extended_box, collision_mask, false);

		// for faster access, cache the wangset
		chaos::TiledMap::Wangset const* wangset = nullptr;
		chaos::TiledMap::TileSet const* tileset = nullptr;

		glm::vec2 delta_position = dst_box.position - src_box.position;

		// iterate over all particles
		while (it)
		{
			// ignore pawn allocation
			if (it->allocation == ignore_allocation)
			{
				it.NextAllocation();
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
				{
					++it;
					continue;
				}

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

			std::pair<glm::vec2, glm::vec2> particle_corners = chaos::GetBoxCorners(it->particle->bounding_box);
			std::pair<glm::vec2, glm::vec2> dst_corners = chaos::GetBoxCorners(dst_box);
			std::pair<glm::vec2, glm::vec2> src_corners = chaos::GetBoxCorners(src_box);

			// XXX : an edge with wang value 
			//         0 -> the tile does not use the wangset at all
			//         1 -> this is the empty wang value
			//        +2 -> good
		

			chaos::Edge best_edge = chaos::Edge::LEFT;
			glm::vec2 best_position = dst_box.position;
			float best_distance = std::numeric_limits<float>::max();

			// LEFT EDGE
			bool left_collision_candidate = (wangset != nullptr) ? (wangtile.GetEdgeValue(chaos::Edge::LEFT) > 1) : ((particle_flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_LEFT) == 0);
			
			// check whether the EDGE is valid and whether the distance between the objects does no increase
			if (left_collision_candidate && delta_position.x >= 0.0f) 
			{
				// check whether EDGE/BOX collision may happen
				if (chaos::MathTools::IsInRange(particle_corners.first.x, dst_box.position.x, dst_corners.second.x + delta.x) && RangeOverlaps(dst_corners, particle_corners, 1))
				{
					// check whether the collision is at least in ZONE 2
					if (particle_corners.first.x < dst_corners.second.x + delta.x)
					{
						// in ZONE 1 ?
						if (particle_corners.first.x < dst_corners.second.x + delta.x * 0.5f)
						{
							float new_x = particle_corners.first.x - dst_box.half_size.x - delta.x * 0.5f;

							float distance = std::abs(dst_box.position.x - new_x);

							// check for best distance among other edges
							if (distance < best_distance)
							{
								best_position.x = new_x;
								best_position.y = dst_box.position.y;
								best_distance = distance;
							}
						}
						// ZONE 1 or 2 : indicates to caller that there is a touch
						func(*it->particle, chaos::Edge::LEFT);
					}
				}
			}

			// RIGHT EDGE
			bool right_collision_candidate = (wangset != nullptr) ? (wangtile.GetEdgeValue(chaos::Edge::RIGHT) > 1) : ((particle_flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_RIGHT) == 0);

			// check whether the EDGE is valid and whether the distance between the objects does no increase
			if (right_collision_candidate && delta_position.x <= 0.0f) 
			{
				// check whether EDGE/BOX collision may happen
				if (chaos::MathTools::IsInRange(particle_corners.second.x, dst_corners.first.x - delta.x, dst_box.position.x) && RangeOverlaps(dst_corners, particle_corners, 1))
				{
					// check whether the collision is at least in ZONE 2
					if (particle_corners.second.x > dst_corners.first.x - delta.x)
					{
						// in ZONE 1 ?
						if (particle_corners.second.x > dst_corners.first.x - delta.x * 0.5f)
						{
							float new_x = particle_corners.second.x + dst_box.half_size.x + delta.x * 0.5f;

							float distance = std::abs(dst_box.position.x - new_x);

							// check for best distance among other edges
							if (distance < best_distance)
							{
								best_position.x = new_x;
								best_position.y = dst_box.position.y;
								best_distance = distance;
							}
						}
						func(*it->particle, chaos::Edge::RIGHT);
					}
				}
			}

			// BOTTOM EDGE
			bool bottom_collision_candidate = (wangset != nullptr) ? (wangtile.GetEdgeValue(chaos::Edge::BOTTOM) > 1) : ((particle_flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_BOTTOM) == 0);

			// check whether the EDGE is valid and whether the distance between the objects does no increase
			if (bottom_collision_candidate && delta_position.y >= 0.0f) 
			{
				// check whether EDGE/BOX collision may happen
				if (chaos::MathTools::IsInRange(particle_corners.first.y, dst_box.position.y, dst_corners.second.y + delta.y) && RangeOverlaps(dst_corners, particle_corners, 0))
				{
					// check whether the collision is at least in ZONE 2
					if (particle_corners.first.y < dst_corners.second.y + delta.y)
					{
						// in ZONE 1 ?
						if (particle_corners.first.y < dst_corners.second.y + delta.y * 0.5f)
						{
							float new_y = particle_corners.first.y - dst_box.half_size.y - delta.y * 0.5f;

							float distance = std::abs(dst_box.position.y - new_y);

							// check for best distance among other edges
							if (distance < best_distance)
							{
								best_position.x = dst_box.position.x;
								best_position.y = new_y;
								best_distance = distance;								
							}
						}
						func(*it->particle, chaos::Edge::BOTTOM);
					}
				}
			}

			// TOP EDGE
			bool top_collision_candidate = (wangset != nullptr) ? (wangtile.GetEdgeValue(chaos::Edge::TOP) > 1) : ((particle_flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_TOP) == 0);

			// check whether the EDGE is valid and whether the distance between the objects does no increase
			if (top_collision_candidate && delta_position.y <= 0.0f) 
			{
				// check whether EDGE/BOX collision may happen
				if (chaos::MathTools::IsInRange(particle_corners.second.y, dst_corners.first.y - delta.y, dst_box.position.y) && RangeOverlaps(dst_corners, particle_corners, 0))
				{
					// check whether the collision is at least in ZONE 2
					if (particle_corners.second.y > dst_corners.first.y - delta.y)
					{
						// in ZONE 1 ?
						if (particle_corners.second.y > dst_corners.first.y - delta.y * 0.5f)
						{
							float new_y = particle_corners.second.y + dst_box.half_size.y + delta.y * 0.5f;

							float distance = std::abs(dst_box.position.y - new_y);

							// check for best distance among other edges
							if (distance < best_distance)
							{
								best_position.x = dst_box.position.x;
								best_position.y = new_y;
								best_distance = distance;								
							}
						}
						func(*it->particle, chaos::Edge::TOP);
					}
				}
			}

			// displace the box among the best edge
			if (best_distance != std::numeric_limits<float>::max())
			{
				dst_box.position = best_position;
			}
			++it;
		}

		return dst_box;
	}

	// =====================================
	// TMCollisionIteratorBase implementation
	// =====================================

	TMCollisionIteratorBase::TMCollisionIteratorBase(TMLevelInstance* in_level_instance, chaos::box2 const& in_collision_box, uint64_t in_collision_mask, bool in_open_geometry) :
		level_instance(in_level_instance),
		collision_box(in_collision_box),
		collision_mask(in_collision_mask),
		open_geometry(in_open_geometry)
	{
		assert(in_level_instance != nullptr);
		assert(collision_mask != 0);
		assert(!chaos::IsGeometryEmpty(in_collision_box));
	}

	TMCollisionIteratorBase::operator bool() const
	{
		return (level_instance != nullptr);
	}

	// =====================================
	// TMTileCollisionIterator implementation
	// =====================================

	TMTileCollisionIterator::TMTileCollisionIterator(TMLevelInstance* in_level_instance, chaos::box2 const& in_collision_box, uint64_t in_collision_mask, bool in_open_geometry) :
		TMCollisionIteratorBase(in_level_instance, in_collision_box, in_collision_mask, in_open_geometry)
	{
		FindFirstCollision();
	}

	TileCollisionInfo const& TMTileCollisionIterator::operator *() const
	{
		assert(level_instance != nullptr); // end already reached. cannot indirect
		return cached_result;
	}

	TileCollisionInfo const* TMTileCollisionIterator::operator ->() const
	{
		assert(level_instance != nullptr); // end already reached. cannot indirect
		return &cached_result;
	}

	void TMTileCollisionIterator::FindFirstCollision()
	{
		assert(level_instance != nullptr); // end already reached. cannot go further

		if (level_instance != nullptr)
		{
			while (layer_instance_index < level_instance->layer_instances.size())
			{	
				TMLayerInstance* layer_instance = level_instance->layer_instances[layer_instance_index].get();

				if (layer_instance != nullptr && (layer_instance->collision_mask & collision_mask) != 0)
				{
					chaos::ParticleLayerBase* particle_layer = layer_instance->particle_layer.get();

					if (particle_layer != nullptr)
					{
						while (allocation_index < particle_layer->GetAllocationCount())
						{
							chaos::ParticleAllocationBase* allocation = particle_layer->GetAllocation(allocation_index);

							if (allocation != nullptr)
							{
								chaos::ParticleAccessor<TMParticle> accessor = allocation->GetParticleAccessor(0, 0);

								while (particle_index < accessor.GetDataCount())
								{
									TMParticle* particle = &accessor[particle_index];

									if (chaos::Collide(collision_box, particle->bounding_box, open_geometry))
									{
										cached_result.layer_instance = layer_instance;
										cached_result.allocation     = allocation;
										cached_result.particle       = particle;
										cached_result.tile_info      = level_instance->GetTiledMap()->FindTileInfo(particle->gid);
										return;
									}
									// next particle
									++particle_index;
								}
							}
							// next allocation
							++allocation_index;
							particle_index = 0;
						}
					}	
				}	
				// next layer
				if (ignore_other_layers)
					break;
				++layer_instance_index;
				allocation_index = 0;
				particle_index = 0;
			}
			// no collision found, end of the iterator
			EndIterator();
		}
	}

	void TMTileCollisionIterator::EndIterator()
	{
		level_instance = nullptr;
		layer_instance_index = 0;
		allocation_index = 0;
		particle_index = 0;
	}

	TMTileCollisionIterator& TMTileCollisionIterator::operator ++ ()
	{
		Next();
		return *this;
	}

	TMTileCollisionIterator TMTileCollisionIterator::operator ++ (int i)
	{
		TMTileCollisionIterator result = *this;
		++(*this);
		return result;
	}
	
	void TMTileCollisionIterator::NextLayer()
	{
		if (ignore_other_layers)
		{
			EndIterator();
		}
		else
		{
			++layer_instance_index;
			allocation_index = 0;
			particle_index = 0;
			FindFirstCollision();
		}
	}
	
	void TMTileCollisionIterator::NextAllocation()
	{
		++allocation_index;
		particle_index = 0;
		FindFirstCollision();
	}
	
	void TMTileCollisionIterator::Next()
	{
		++particle_index;
		FindFirstCollision();
	}


	// =====================================
	// TMTriggerCollisionIterator implementation
	// =====================================

	TMTriggerCollisionIterator::TMTriggerCollisionIterator(TMLevelInstance* in_level_instance, chaos::box2 const& in_collision_box, uint64_t in_collision_mask, bool in_open_geometry) :
		TMObjectCollisionIteratorBase<TMTrigger>(in_level_instance, in_collision_box, in_collision_mask, in_open_geometry)
	{
		FindFirstCollision();
	}

	void TMTriggerCollisionIterator::FindFirstCollision()
	{
		assert(level_instance != nullptr); // end already reached. cannot go further

		if (level_instance != nullptr)
		{
			while (layer_instance_index < level_instance->layer_instances.size())
			{
				TMLayerInstance* layer_instance = level_instance->layer_instances[layer_instance_index].get();

				if (layer_instance != nullptr && (layer_instance->collision_mask & collision_mask) != 0)
				{
					while (object_index < layer_instance->GetObjectCount())
					{
						TMTrigger* object = auto_cast(layer_instance->GetObject(object_index));
						if (object != nullptr)
						{
							if (chaos::Collide(collision_box, object->GetBoundingBox(true), open_geometry))
							{
								cached_result = object;
								return;
							}
						}
						++object_index;
					}
				}
				if (ignore_other_layers)
					break;
				++layer_instance_index;
				object_index = 0;
			}
			// no collision found, end of the iterator
			EndIterator();
		}
	}

	TMTriggerCollisionIterator& TMTriggerCollisionIterator::operator ++ ()
	{
		Next();
		return *this;
	}

	TMTriggerCollisionIterator TMTriggerCollisionIterator::operator ++ (int i)
	{
		TMTriggerCollisionIterator result = *this;
		++(*this);
		return result;
	}

	void TMTriggerCollisionIterator::NextLayer()
	{
		if (ignore_other_layers)
		{
			EndIterator();
		}
		else
		{
			++layer_instance_index;
			object_index = 0;
			FindFirstCollision();
		}
	}

	void TMTriggerCollisionIterator::Next()
	{
		++object_index;
		FindFirstCollision();
	}


	// =====================================
	// TMObjectCollisionIterator implementation
	// =====================================

	TMObjectCollisionIterator::TMObjectCollisionIterator(TMLevelInstance* in_level_instance, chaos::box2 const& in_collision_box, uint64_t in_collision_mask, bool in_open_geometry) :
		TMObjectCollisionIteratorBase<TMObject>(in_level_instance, in_collision_box, in_collision_mask, in_open_geometry)
	{
		FindFirstCollision();
	}

	void TMObjectCollisionIterator::FindFirstCollision()
	{
		assert(level_instance != nullptr); // end already reached. cannot go further

		if (level_instance != nullptr)
		{
			while (layer_instance_index < level_instance->layer_instances.size())
			{
				TMLayerInstance* layer_instance = level_instance->layer_instances[layer_instance_index].get();

				if (layer_instance != nullptr && (layer_instance->collision_mask & collision_mask) != 0)
				{
					while (object_index < layer_instance->GetObjectCount())
					{
						TMObject* object = layer_instance->GetObject(object_index);
						if (object != nullptr)
						{
							if (chaos::Collide(collision_box, object->GetBoundingBox(true), open_geometry))
							{
								cached_result = object;
								return;
							}
						}
						++object_index;
					}
				}
				if (ignore_other_layers)
					break;
				++layer_instance_index;
				object_index = 0;
			}
			// no collision found, end of the iterator
			EndIterator();
		}
	}

	TMObjectCollisionIterator& TMObjectCollisionIterator::operator ++ ()
	{
		Next();
		return *this;
	}

	TMObjectCollisionIterator TMObjectCollisionIterator::operator ++ (int i)
	{
		TMObjectCollisionIterator result = *this;
		++(*this);
		return result;
	}

	void TMObjectCollisionIterator::NextLayer()
	{
		if (ignore_other_layers)
		{
			EndIterator();
		}
		else
		{
			++layer_instance_index;
			object_index = 0;
			FindFirstCollision();
		}
	}

	void TMObjectCollisionIterator::Next()
	{
		++object_index;
		FindFirstCollision();
	}

}; // namespace death
