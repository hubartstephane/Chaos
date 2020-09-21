#include <death/TM.h>

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

	chaos::box2 ComputeTileCollisionAndReaction(TMLevelInstance* level_instance, chaos::box2 src_box, chaos::box2 dst_box, int collision_mask, chaos::ParticleAllocationBase* ignore_allocation, char const* wangset_name, std::function<bool(TMParticle&, chaos::Edge)> func)
	{
		assert(level_instance != nullptr);

		chaos::box2 result = dst_box;

		// work on Extended copy of the box
		glm::vec2 delta = glm::vec2(15.0f, 15.0f);
		src_box.half_size += delta;
		dst_box.half_size += delta;

		// collision over the extended bounding box

		auto bb = src_box | dst_box;
		bb.half_size *= 3.0f;

		TMTileCollisionIterator it = level_instance->GetTileCollisionIterator(bb, collision_mask, false);

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
			if (!chaos::Collide(dst_box, it->particle->bounding_box))
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

			// search the displacement that is the smallest so that pawn becomes outside the particle
			float best_distance = std::numeric_limits<float>::max();
			glm::vec2 best_center = dst_box.position;
			chaos::Edge best_edge = chaos::Edge::TOP;

			std::pair<glm::vec2, glm::vec2> particle_corners = chaos::GetBoxCorners(it->particle->bounding_box);
			std::pair<glm::vec2, glm::vec2> dst_corners = chaos::GetBoxCorners(dst_box);
			std::pair<glm::vec2, glm::vec2> src_corners = chaos::GetBoxCorners(src_box);

			// XXX : an edge with wang value 
			//         0 -> the tile does not use the wangset at all
			//         1 -> this is the empty wang value
			//        +2 -> good


















			// only test LEFT side if no neighbour
			bool left_collision_candidate = (wangset != nullptr) ?
				(wangtile.GetEdgeValue(chaos::Edge::LEFT) > 1) :
				((particle_flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_LEFT) == 0);

			if (left_collision_candidate && src_box.position.x <= dst_box.position.x)
			{
				if (chaos::MathTools::IsInRange(particle_corners.first.x, dst_corners.first.x, dst_corners.second.x))
				{
					if (chaos::MathTools::IsInRange(particle_corners.first.x, src_corners.second.x - delta.x, src_corners.second.x))
					{
						best_distance = 0.0f;
						best_center.x = src_box.position.x;
						best_edge = chaos::Edge::LEFT;
					}
					else if (particle_corners.first.x > dst_corners.second.x - delta.x)
					{
						best_distance = 0.0f;
						best_edge = chaos::Edge::LEFT;
					}
					else
					{
						float new_x = particle_corners.first.x - dst_box.half_size.x + delta.x * 0.5f;

						float d = std::abs(new_x - dst_box.position.x);
						if (d < best_distance)
						{
							best_distance = d;
							best_center.x = new_x;
							best_edge = chaos::Edge::LEFT;
						}
					}
				}
			}

			// only test RIGHT side if no neighbour
			bool right_collision_candidate = (wangset != nullptr) ?
				(wangtile.GetEdgeValue(chaos::Edge::RIGHT) > 1) :
				((particle_flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_RIGHT) == 0);

			if (right_collision_candidate && src_box.position.x >= dst_box.position.x)
			{
				if (chaos::MathTools::IsInRange(particle_corners.second.x, dst_corners.first.x, dst_corners.second.x))
				{
					if (chaos::MathTools::IsInRange(particle_corners.second.x, src_corners.first.x, src_corners.first.x + delta.x))
					{					
						best_distance = 0.0f;
						best_center.x = src_box.position.x;
						best_edge = chaos::Edge::RIGHT;
					}
					else if (particle_corners.second.x < dst_corners.first.x + delta.x)
					{
						best_distance = 0.0f;
						best_edge = chaos::Edge::RIGHT;
					}
					else
					{
						float new_x = particle_corners.second.x + dst_box.half_size.x - delta.x * 0.5f;

						float d = std::abs(new_x - dst_box.position.x);
						if (d < best_distance)
						{
							best_distance = d;
							best_center.x = new_x;
							best_edge = chaos::Edge::RIGHT;
						}
					}
				}
			}

			// only test BOTTOM side if no neighbour
			bool bottom_collision_candidate = (wangset != nullptr) ?
				(wangtile.GetEdgeValue(chaos::Edge::BOTTOM) > 1) :
				((particle_flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_BOTTOM) == 0);

			if (bottom_collision_candidate && src_box.position.y <= dst_box.position.y)
			{
				if (chaos::MathTools::IsInRange(particle_corners.first.y, dst_corners.first.y, dst_corners.second.y))
				{
					if (chaos::MathTools::IsInRange(particle_corners.first.y, src_corners.second.y - delta.y, src_corners.second.y))
					{
						best_distance = 0.0f;
						best_center.y = src_box.position.y;
						best_edge = chaos::Edge::BOTTOM;
					}
					else if (particle_corners.first.y > dst_corners.second.y - delta.y)
					{
						best_distance = 0.0f;
						best_edge = chaos::Edge::BOTTOM;
					}
					else
					{
						float new_y = particle_corners.first.y - dst_box.half_size.y + delta.y * 0.5f;

						float d = std::abs(new_y - dst_box.position.y);
						if (d < best_distance)
						{
							best_distance = d;
							best_center.y = new_y;
							best_edge = chaos::Edge::BOTTOM;
						}
					}
				}
			}






			// only test TOP side if no neighbour
			bool top_collision_candidate = (wangset != nullptr) ?
				(wangtile.GetEdgeValue(chaos::Edge::TOP) > 1) :
				((particle_flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_TOP) == 0);

			if (top_collision_candidate && src_box.position.y >= dst_box.position.y)
			{
				if (chaos::MathTools::IsInRange(particle_corners.second.y, dst_corners.first.y, dst_corners.second.y))
				{
					if (chaos::MathTools::IsInRange(particle_corners.second.y, src_corners.first.y, src_corners.first.y + delta.y))
					{
						best_distance = 0.0f; 
						best_center.y = src_box.position.y;
						best_edge = chaos::Edge::TOP;
					}
					else if (particle_corners.second.y < dst_corners.first.y + delta.y)
					{
						best_distance = 0.0f;
						best_edge = chaos::Edge::TOP;
					}
					else
					{
						float new_y = particle_corners.second.y + dst_box.half_size.y - delta.y * 0.5f;

						float d = std::abs(new_y - dst_box.position.y);
						if (d < best_distance)
						{
							best_distance = d;
							best_center.y = new_y;
							best_edge = chaos::Edge::TOP;
						}
					}
				}
			}




			// if a displacement is found to stop the collision, apply it
			if (best_distance < std::numeric_limits<float>::max())
			{
				if (func == nullptr || func(*it->particle, best_edge))
				{
					src_box = dst_box;
					dst_box.position = best_center;
				}
			}
			++it;
		}

		// get the position of the extended box
		result.position = dst_box.position;

		return result;
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
