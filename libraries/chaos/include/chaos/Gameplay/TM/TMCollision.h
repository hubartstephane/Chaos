namespace chaos
{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// =====================================
	// TileCollisionInfo
	// =====================================

	template<typename CONSTNESS_OPERATOR>
	class TileCollisionInfoBase
	{
	public:

		using layer_type = typename boost::mpl::apply<CONSTNESS_OPERATOR, TMLayerInstance>::type;
		using allocation_type = typename boost::mpl::apply<CONSTNESS_OPERATOR, ParticleAllocationBase>::type;
		using particle_type = typename boost::mpl::apply<CONSTNESS_OPERATOR, TMParticle>::type;

		/** the layer instance concerned by this collision */
		layer_type* layer_instance = nullptr;
		/** the allocation concerned by this collision */
		allocation_type* allocation = nullptr;
		/** the particle which with the collision is detected */
		particle_type* particle = nullptr;
		/** some information about the tile information */
		TiledMap::TileInfo tile_info;
	};

	// =====================================
	// TMCollisionIteratorBase
	// =====================================

	template<typename CONSTNESS_OPERATOR>
	class TMCollisionIteratorBase
	{
	public:

		using layer_type = typename boost::mpl::apply<CONSTNESS_OPERATOR, TMLayerInstance>::type;
		using level_type = typename boost::mpl::apply<CONSTNESS_OPERATOR, TMLevelInstance>::type;

		/** default constructor */
		TMCollisionIteratorBase() = default;
		/** copy constructor */
		TMCollisionIteratorBase(TMCollisionIteratorBase const& src) = default;
		/** constructor from a level instance */
		TMCollisionIteratorBase(level_type* in_level_instance, uint64_t in_collision_mask, box2 const& in_collision_box, bool in_open_geometry) :
			li_iterator(in_level_instance, in_collision_mask),
			collision_box(in_collision_box),
			open_geometry(in_open_geometry)
		{
			level_instance = in_level_instance;
		}

		/** constructor from a level instance */
		TMCollisionIteratorBase(layer_type* in_layer_instance, uint64_t in_collision_mask, box2 const& in_collision_box, bool in_open_geometry) :
			li_iterator(in_layer_instance, in_collision_mask),
			collision_box(in_collision_box),
			open_geometry(in_open_geometry)
		{
			level_instance = in_layer_instance->GetLevelInstance();
		}

		/** returns whether the iterator is still valid */
		operator bool() const
		{
			return li_iterator;
		}

	protected:

		/** the level instance */
		level_type* level_instance = nullptr;
		/** the collision box */
		box2 collision_box;
		/** whether we want to check open geometries */
		bool open_geometry = false;
		/** the iterator over layer instances */
		TMLayerInstanceIteratorBase<CONSTNESS_OPERATOR> li_iterator;
	};


	// =====================================
	// TMTileCollisionIteratorBase
	// =====================================

	template<typename CONSTNESS_OPERATOR>
	class TMTileCollisionIteratorBase : public TMCollisionIteratorBase<CONSTNESS_OPERATOR>
	{
	public:

		using layer_type = typename TMCollisionIteratorBase<CONSTNESS_OPERATOR>::layer_type;
		using level_type = typename TMCollisionIteratorBase<CONSTNESS_OPERATOR>::level_type;

		using collision_info = TileCollisionInfoBase<CONSTNESS_OPERATOR>;


		/** the default constructor */
		TMTileCollisionIteratorBase() = default;
		/** the copy constructor */
		TMTileCollisionIteratorBase(TMTileCollisionIteratorBase const& src) = default;
		/** the constructor with initialization */
		TMTileCollisionIteratorBase(layer_type* in_layer_instance, uint64_t in_collision_mask, box2 const& in_collision_box, bool in_open_geometry) :
			TMCollisionIteratorBase<CONSTNESS_OPERATOR>(in_layer_instance, in_collision_mask, in_collision_box, in_open_geometry)
		{
			FindElement(false);
		}
		/** the constructor with initialization */
		TMTileCollisionIteratorBase(level_type* in_level_instance, uint64_t in_collision_mask, box2 const& in_collision_box, bool in_open_geometry) :
			TMCollisionIteratorBase<CONSTNESS_OPERATOR>(in_level_instance, in_collision_mask, in_collision_box, in_open_geometry)
		{
			FindElement(false);
		}

		/** comparaison operator */
		friend bool operator == (TMTileCollisionIteratorBase const& src1, TMTileCollisionIteratorBase const& src2)
		{
			if (src1.li_iterator != src2.li_iterator)
				return false;
			if (src1.allocation_index != src2.allocation_index)
				return false;
			if (src1.particle_index != src2.particle_index)
				return false;
			return false;
		}

		/** comparaison operator */
		friend bool operator != (TMTileCollisionIteratorBase const& src1, TMTileCollisionIteratorBase const& src2)
		{
			return !(src1 == src2);
		}

		/** indirection method */
		collision_info const& operator *() const
		{
			assert(this->li_iterator); // end not reached
			return cached_result;
		}

		/** indirection method */
		collision_info const* operator ->() const
		{
			assert(this->li_iterator); // end not reached
			return &cached_result;
		}

		/** pre increment iterator */
		TMTileCollisionIteratorBase& operator ++ ()
		{
			assert(this->li_iterator); // end not reached
			FindElement(true);
			return *this;
		}

		/** post increment iterator */
		TMTileCollisionIteratorBase operator ++ (int i)
		{
			TMTileCollisionIteratorBase result = *this;
			++(*this);
			return result;
		}

		/** skip to next layer */
		void NextLayer()
		{
			assert(this->li_iterator); // end not reached
			++this->li_iterator;
			allocation_index = 0;
			particle_index = 0;
			FindElement(false);
		}

		/** skip to next allocation */
		void NextAllocation()
		{
			assert(this->li_iterator); // end not reached
			++allocation_index;
			particle_index = 0;
			FindElement(false);
		}

		/** skip to next particle */
		void NextParticle()
		{
			assert(this->li_iterator); // end not reached
			++particle_index;
			FindElement(false);
		}

	protected:

		/** find the very first collision from given conditions */
		void FindElement(bool ignore_first)
		{
			while (this->li_iterator)
			{
				auto* particle_layer = this->li_iterator->particle_layer.get();

				if (particle_layer != nullptr)
				{
					while (allocation_index < particle_layer->GetAllocationCount())
					{
						auto* allocation = particle_layer->GetAllocation(allocation_index);

						if (allocation != nullptr)
						{
							while (particle_index < allocation->GetParticleCount())
							{
								// same for both ParticleAccessor<...> and ParticleConstAccessor<...>
								RawDataBufferAccessorBase<boost::mpl::apply<CONSTNESS_OPERATOR, TMParticle>::type> accessor = allocation->GetParticleAccessor(0, 0);

								auto * particle = &accessor[particle_index];

								if (Collide(this->collision_box, particle->bounding_box, this->open_geometry))
								{
									if (!ignore_first)
									{
										cached_result.layer_instance = &(*this->li_iterator);
										cached_result.allocation = allocation;
										cached_result.particle = particle;
										cached_result.tile_info = this->level_instance->GetTiledMap()->FindTileInfo(particle->gid);
										return;
									}
									ignore_first = false;
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
				// next layer instance
				++this->li_iterator;
				allocation_index = 0;
				particle_index = 0;
			}
		}

	protected:

		/** allocation index in that layer */
		size_t allocation_index = 0;
		/** index of the particle in that layer */
		size_t particle_index = 0;
		/** the collision data */
		collision_info cached_result;
	};

	// =====================================
	// TMObjectCollisionIteratorBase
	// =====================================

	template<typename T, typename CONSTNESS_OPERATOR>
	class TMObjectCollisionIteratorBase : public TMCollisionIteratorBase<CONSTNESS_OPERATOR>
	{
	public:

		using layer_type  = typename TMCollisionIteratorBase<CONSTNESS_OPERATOR>::layer_type;
		using level_type  = typename TMCollisionIteratorBase<CONSTNESS_OPERATOR>::level_type;
		using object_type = typename boost::mpl::apply<CONSTNESS_OPERATOR, T>::type;

		/** the default constructor */
		TMObjectCollisionIteratorBase() = default;
		/** the copy constructor */
		TMObjectCollisionIteratorBase(TMObjectCollisionIteratorBase const& src) = default;
		/** the constructor with initialization */
		TMObjectCollisionIteratorBase(layer_type * in_layer_instance, uint64_t in_collision_mask, box2 const& in_collision_box, bool in_open_geometry) :
			TMCollisionIteratorBase<CONSTNESS_OPERATOR>(in_layer_instance, in_collision_mask, in_collision_box, in_open_geometry)
		{
			FindElement(false);
		}
		/** the constructor with initialization */
		TMObjectCollisionIteratorBase(level_type * in_level_instance, uint64_t in_collision_mask, box2 const& in_collision_box, bool in_open_geometry) :
			TMCollisionIteratorBase<CONSTNESS_OPERATOR>(in_level_instance, in_collision_mask, in_collision_box, in_open_geometry)
		{
			FindElement(false);
		}


		/** comparaison operator */
		friend bool operator == (TMObjectCollisionIteratorBase const& src1, TMObjectCollisionIteratorBase const& src2)
		{
			if (src1.li_iterator != src2.li_iterator)
				return false;
			if (src1.object_index != src2.object_index)
				return false;
			return false;
		}

		/** comparaison operator */
		friend bool operator != (TMObjectCollisionIteratorBase const& src1, TMObjectCollisionIteratorBase const& src2)
		{
			return !(src1 == src2);
		}

		/** indirection method */
		object_type & operator *() const
		{
			assert(this->li_iterator); // end already reached. cannot indirect
			return *cached_result;
		}

		/** indirection method */
		object_type* operator ->() const
		{
			assert(this->li_iterator); // end already reached. cannot indirect
			return cached_result;
		}

		/** pre increment iterator */
		TMObjectCollisionIteratorBase& operator ++ ()
		{
			assert(this->li_iterator); // end already reached. cannot indirect
			FindElement(true);
			return *this;
		}

		/** post increment iterator */
		TMObjectCollisionIteratorBase operator ++ (int i)
		{
			TMObjectCollisionIteratorBase result = *this;
			++(*this);
			return result;
		}

		/** skip to next layer */
		void NextLayer()
		{
			assert(this->li_iterator); // end not reached
			++this->li_iterator;
			object_index = 0;
			FindElement(false);
		}


		/** skip to next object */
		void NextObject()
		{
			assert(this->li_iterator); // end not reached
			++object_index;
			FindElement(false);
		}

	protected:

		/** find the very first collision from given conditions */
		void FindElement(bool ignore_first)
		{
			while (this->li_iterator)
			{
				while (object_index < this->li_iterator->GetObjectCount())
				{
					object_type * object = auto_cast(this->li_iterator->GetObject(object_index));
					if (object != nullptr)
					{
						if (Collide(this->collision_box, object->GetBoundingBox(true), this->open_geometry))
						{
							if (!ignore_first)
							{
								cached_result = object;
								return;
							}
							ignore_first = false;
						}
					}
					// next object
					++object_index;
				}
				// next layer
				++this->li_iterator;
				object_index = 0;
			}
		}

	protected:

		/** object index in current layer */
		size_t object_index = 0;
		/** the current result of the research */
		object_type * cached_result = nullptr;
	};

#endif

}; // namespace chaos