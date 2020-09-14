#include <death/TM.h>

namespace death
{
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
									death::TMParticle* particle = &accessor[particle_index];

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
