#ifdef CHAOS_FORWARD_DECLARATION

#else

namespace chaos
{
	// =====================================
	// TileCollisionInfo
	// =====================================

	class TileCollisionInfo
	{
	public:

		/** the layer instance concerned by this collision */
		TMLayerInstance* layer_instance = nullptr;
		/** the allocation concerned by this collision */
		ParticleAllocationBase* allocation = nullptr;
		/** the particle which with the collision is detected */
		TMParticle* particle = nullptr;
		/** some information about the tile information */
		TiledMap::TileInfo tile_info;
	};










	// =====================================
	// TMCollisionIteratorBase
	// =====================================

	class TMCollisionIteratorBase
	{

	public:

		/** the default constructor */
		TMCollisionIteratorBase() = default;
		/** the copy constructor */
		TMCollisionIteratorBase(TMCollisionIteratorBase const& src) = default;
		/** the constructor with initialization */
		TMCollisionIteratorBase(TMLevelInstance* in_level_instance, box2 const& in_collision_box, uint64_t in_collision_mask, bool in_open_geometry);

		/** returns whether the iterator is still valid */
		operator bool() const;

	protected:

		/** the collision box */
		box2 collision_box;
		/** the collision mask for iterating over layers */
		uint64_t collision_mask = 0;
		/** whether we want to check open geometries */
		bool open_geometry = false;


		/** the level instance of concern */
		TMLevelInstance* level_instance = nullptr;
		/** index of the layer */
		size_t layer_instance_index = 0;
		/** whether the iterator finishs with the current layer */
		bool ignore_other_layers = false;
	};

	// =====================================
	// TMTileCollisionIterator
	// =====================================

	class TMTileCollisionIterator : public TMCollisionIteratorBase
	{
	public:

		/** the default constructor */
		TMTileCollisionIterator() = default;
		/** the copy constructor */
		TMTileCollisionIterator(TMTileCollisionIterator const& src) = default;
		/** the constructor with initialization */
		TMTileCollisionIterator(TMLevelInstance* in_level_instance, box2 const& in_collision_box, uint64_t in_collision_mask, bool in_open_geometry);

		/** next layer */
		void NextLayer();
		/** next allocation */
		void NextAllocation();
		/** next particle (i.e operator ++) */
		void Next();

		// indirection
		TileCollisionInfo const& operator *() const;
		// indirection
		TileCollisionInfo const* operator ->() const;
		// pre increment iterator
		TMTileCollisionIterator& operator ++ ();
		// post increment iterator
		TMTileCollisionIterator operator ++ (int i);

	protected:

		/** find the very first collision from given conditions */
		void FindFirstCollision();
		/** called to set the iterator at its end */
		void EndIterator();

	protected:

		/** allocation index in that layer */
		size_t allocation_index = 0;
		/** index of the particle in that layer */
		size_t particle_index = 0;

		/** the collision data */
		TileCollisionInfo cached_result;
	};

	// =====================================
	// TMObjectCollisionIteratorBase
	// =====================================

	template<typename T>
	class TMObjectCollisionIteratorBase : public TMCollisionIteratorBase
	{
	public:

		/** the default constructor */
		TMObjectCollisionIteratorBase() = default;
		/** the copy constructor */
		TMObjectCollisionIteratorBase(TMObjectCollisionIteratorBase const& src) = default;
		/** the constructor with initialization */
		TMObjectCollisionIteratorBase(TMLevelInstance* in_level_instance, box2 const& in_collision_box, uint64_t in_collision_mask, bool in_open_geometry) :
			TMCollisionIteratorBase(in_level_instance, in_collision_box, in_collision_mask, in_open_geometry)
		{
			FindFirstCollision();
		}
		// indirection
		T& operator *() const
		{
			assert(level_instance != nullptr); // end already reached. cannot indirect
			return *cached_result;
		}
		// indirection
		T* operator ->() const
		{
			assert(level_instance != nullptr); // end already reached. cannot indirect
			return cached_result;
		}

		/** go to next layer */
		void NextLayer()
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

		/** go to next element */
		void Next()
		{
			++object_index;
			FindFirstCollision();
		}
	
		// pre increment iterator
		TMObjectCollisionIteratorBase<T>& operator ++ ()
		{
			Next();
			return *this;
		}

		// post increment iterator
		TMObjectCollisionIteratorBase<T> operator ++ (int i)
		{
			TMObjectCollisionIteratorBase<T> result = *this;
			++(*this);
			return result;
		}

	protected:
		
		/** find the very first collision from given conditions */
		void FindFirstCollision()
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
							T * object = auto_cast(layer_instance->GetObject(object_index));
							if (object != nullptr)
							{
								if (Collide(collision_box, object->GetBoundingBox(true), open_geometry))
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

		/** called to set the iterator at its end */
		void EndIterator()
		{
			level_instance = nullptr;
			layer_instance_index = 0;
			object_index = 0;
		}

	protected:

		/** index of the layer */
		size_t layer_instance_index = 0;
		/** trigger index in that layer */
		size_t object_index = 0;

		/** the current result of the research */
		T* cached_result = nullptr;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION