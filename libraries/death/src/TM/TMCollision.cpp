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
	// -------------------
	// Collision Artifacts
	// -------------------
	//
	// 1. alternating collision/no collision state
	// -------------------------------------------
	//
	// For a falling PAWN, as soon as the PAWN interpenetrate the ground, it is pushed upward. This leads to situation where the pawn is falling for one frame and grounded the other
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
	// To solve with issue we use extended collision
	//
	//   +--------------+  The Pawn bounding box is slightly extended
	//   |    ZONE 2    |  -Whenever a collision happens inside ZONE 2, the PAWN is not pushed at all
	//   |  +--------+  |  -Whenever a collision happens inside ZONE 1, the PAWN is pushed (but not too far. The collision after happens in middle of ZONE 2)
	//   |  | ZONE 1 |  |  
	//   |  |        |  |
	//   |  +--------+  |
	//   |     +---------------+
	//   +-----|--------+      |
	//         |               |
	//
	//
	// 2. Non really stopping collisions
	// ---------------------------------
	//
	// +----------------+ PAWN         We are now using 2 zones collisions
	// |     ZONE 2     |              Whenever there is a collision in ZONE 2, it is still possible for the PAWN to go near and near the wall (the two objets are going near one another, by slowly)
	// |                |
	// |     +----+     | move         to solve this issue, we check for collision for the previous frame position. If there already was a collision in zone 2 in previous frame,
	// |     |    |     | ---->        we refuse the PAWN to move. We keep its position to the one in previous frame
	// |     +----+     |
	//               +---------  
	//               |
	//
	// 3. Slickery collision
	// ---------------------
	//
	// +----------------+ PAWN         The previous changes, can lead to situation where the two objets want to go further one another
	// |     ZONE 2     |              but the collision become slickery
	// |                |
	// |     +----+     |  move        to solve this issue we check for relative velocity of both objects (we store PAWN previous position)
	// |     |    |     | <----        if the 2 objects are naturally going further one another, we ignore collisions
	// |     +----+     |
	//               +-----------
	//               |

	static bool RangeOverlaps(float Min1, float Max1, float Min2, float Max2)
	{
		if (Min1 > Max2 || Max1 < Min2)
			return false;
		return true;
	}

	static bool RangeOverlaps(std::pair<glm::vec2, glm::vec2> const & range1, std::pair<glm::vec2, glm::vec2> const& range2, int component)
	{
		return RangeOverlaps(range1.first[component], range1.second[component], range2.first[component], range2.second[component]);
	}

	chaos::box2 ComputeTileCollisionAndReaction(TMLevelInstance* level_instance, chaos::box2 src_box, chaos::box2 dst_box, int collision_mask, chaos::ParticleAllocationBase* ignore_allocation, char const* wangset_name, std::function<bool(TMParticle&, chaos::Edge)> func)
	{
		assert(level_instance != nullptr);

		// work on extended copy of the box
		chaos::box2 extended_box = src_box | dst_box;

		glm::vec2 delta = glm::vec2(15.0f, 15.0f);
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

			if (left_collision_candidate && delta_position.x >= 0.0f)
			{
				if ((particle_corners.first.x < dst_corners.second.x + delta.x * 0.5f) && (src_corners.second.x < particle_corners.first.x) && RangeOverlaps(dst_corners, particle_corners, 1))
				{
					float new_x = particle_corners.first.x - dst_box.half_size.x - delta.x * 0.5f;

					float distance = std::abs(dst_box.position.x - new_x);
					if (distance < best_distance)
					{
						best_position.x = new_x;
						best_position.y = dst_box.position.y;
						best_distance = distance;
					}
				}
			}

			// RIGHT EDGE
			bool right_collision_candidate = (wangset != nullptr) ? (wangtile.GetEdgeValue(chaos::Edge::RIGHT) > 1) : ((particle_flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_RIGHT) == 0);

			if (right_collision_candidate && delta_position.x <= 0.0f)
			{


				if ((particle_corners.second.x > dst_corners.first.x - delta.x * 0.5f) && (src_corners.first.x > particle_corners.second.x) && RangeOverlaps(dst_corners, particle_corners, 1))
				{
					float new_x = particle_corners.second.x + dst_box.half_size.x + delta.x * 0.5f;

					float distance = std::abs(dst_box.position.x - new_x);
					if (distance < best_distance)
					{
						best_position.x = new_x;
						best_position.y = dst_box.position.y;
						best_distance = distance;
					}
				}




			}

			// BOTTOM EDGE
			bool bottom_collision_candidate = (wangset != nullptr) ? (wangtile.GetEdgeValue(chaos::Edge::BOTTOM) > 1) : ((particle_flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_BOTTOM) == 0);

			if (bottom_collision_candidate && delta_position.y >= 0.0f)
			{



				if ((particle_corners.first.y < dst_corners.second.y + delta.y * 0.5f) && (src_corners.second.y < particle_corners.first.y) && RangeOverlaps(dst_corners, particle_corners, 0))
				{
					float new_y = particle_corners.first.y - dst_box.half_size.y - delta.y * 0.5f;

					float distance = std::abs(dst_box.position.y - new_y);
					if (distance < best_distance)
					{
						best_position.x = dst_box.position.x;
						best_position.y = new_y;
						best_distance = distance;
					}
				}




			}

			// TOP EDGE
			bool top_collision_candidate = (wangset != nullptr) ? (wangtile.GetEdgeValue(chaos::Edge::TOP) > 1) : ((particle_flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_TOP) == 0);

			if (top_collision_candidate && delta_position.y <= 0.0f)
			{





				if ((particle_corners.second.y > dst_corners.first.y - delta.y * 0.5f) && (src_corners.first.y > particle_corners.second.y) && RangeOverlaps(dst_corners, particle_corners, 0))
				{
					float new_y = particle_corners.second.y + dst_box.half_size.y + delta.y * 0.5f;

					float distance = std::abs(dst_box.position.y - new_y);
					if (distance < best_distance)
					{
						best_position.x = dst_box.position.x;
						best_position.y = new_y;
						best_distance = distance;
					}
				}







			}

			// displace the box
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
