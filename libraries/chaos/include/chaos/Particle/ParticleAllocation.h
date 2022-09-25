namespace chaos
{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

		// ==============================================================
		// ParticleAllocationBase
		// ==============================================================

	class CHAOS_API ParticleAllocationBase : public Object
	{
		CHAOS_PARTICLE_ALL_FRIENDS

	public:

		/** constructor */
		ParticleAllocationBase(ParticleLayerBase * in_layer);
		/** destructor */
		virtual ~ParticleAllocationBase();

		/** Reference count updated : 2 use cases (external reference or not) */
		virtual void SubReference() override;

		/** returns true whether the allocation is attached to a layer */
		bool IsAttachedToLayer() const;

		/** pause/resume the layer */
		void Pause(bool in_paused = true);
		/** returns whether the layer is paused */
		bool IsPaused() const;

		/** show/hide the layer */
		void Show(bool in_visible = true);
		/** returns whether the layer is visible */
		bool IsVisible() const;

		/** returns the ID representing the class of the particle */
		virtual Class const * GetParticleClass() const { return nullptr; }

		/** get the size of one particle */
		virtual size_t GetParticleSize() const { return 0; }
		/** get the number of particles */
		virtual size_t GetParticleCount() const { return 0; }
		/** get the particles */
		virtual void * GetParticleBuffer() { return nullptr; }
		/** get the particles */
		virtual void const * GetParticleBuffer() const { return nullptr; }
		/** resize the particles */
		virtual AutoCastedParticleAccessor Resize(size_t new_count);

		/** increment number of particles */
        AutoCastedParticleAccessor AddParticles(size_t extra_count);

		/** set the empty callback */
		void SetDestroyWhenEmpty(bool in_destroy_when_empty) { destroy_when_empty = in_destroy_when_empty; }
		/** returns whether the allocation is to be destroyed when empty */
		bool GetDestroyWhenEmpty() const { return destroy_when_empty; }

		/** get the layer for this allocation */
		ParticleLayerBase* GetLayer() { return layer; }
		/** get the layer for this allocation */
		ParticleLayerBase const* GetLayer() const { return layer; }

		/** remove the allocation from its layer */
		void RemoveFromLayer();

		/** returns true whether the class required is compatible with the one store in the buffer */
		template<typename PARTICLE_TYPE>
		bool IsParticleClassCompatible() const
		{
			return Class::InheritsFrom(GetParticleClass(), ClassManager::GetDefaultInstance()->FindCPPClass<PARTICLE_TYPE>(), true) == InheritanceType::YES;
		}

        /** get an AutoCasting particle accessor */
		AutoCastedParticleAccessor GetParticleAccessor(size_t start = 0, size_t count = 0);
        /** get an AutoCasting particle accessor */
		AutoCastedParticleConstAccessor GetParticleAccessor(size_t start = 0, size_t count = 0) const;
        /** get an AutoCasting particle accessor */
		AutoCastedParticleConstAccessor GetParticleConstAccessor(size_t start = 0, size_t count = 0) const;

		/** get an accessor for the particles */
		template<typename PARTICLE_TYPE>
		ParticleAccessor<PARTICLE_TYPE> GetParticleAccessor(size_t start = 0, size_t count = 0)
		{
			// check for compatibility => returns failure accessor
			if (!IsParticleClassCompatible<PARTICLE_TYPE>())
				return {};
			// compute result
			size_t particle_size = 0;
			void* buffer = const_cast<void*>(GetAccessorEffectiveRanges(start, count, particle_size));
			if (buffer == nullptr)
				return {};
			return ParticleAccessor<PARTICLE_TYPE>(buffer, count, particle_size);
		}

		/** get an accessor for the particles */
		template<typename PARTICLE_TYPE>
		ParticleConstAccessor<PARTICLE_TYPE> GetParticleConstAccessor(size_t start = 0, size_t count = 0) const
		{
			// check for compatibility => returns failure accessor
			if (!IsParticleClassCompatible<PARTICLE_TYPE>())
				return {};
			// compute result
			size_t particle_size = 0;
			void const* buffer = GetAccessorEffectiveRanges(start, count, particle_size);
			if (buffer == nullptr)
				return {};
			return ParticleConstAccessor<PARTICLE_TYPE>(buffer, count, particle_size);
		}

		/** get an accessor for the particles */
		template<typename PARTICLE_TYPE>
		ParticleConstAccessor<PARTICLE_TYPE> GetParticleAccessor(size_t start = 0, size_t count = 0) const
		{
			return GetParticleConstAccessor<PARTICLE_TYPE>();
		}

		/** getter on the extra data */
		template<typename T>
		T* GetOwnedData()
		{
			return chaos::GetOwnedData<T>(this);
		}

		/** getter on the extra data */
		template<typename T>
		T const* GetOwnedData() const
		{
			return chaos::GetOwnedData<T>(this);
		}

	protected:

        /** compute the ranges for accessor (returns false in case of failure) */
        void const * GetAccessorEffectiveRanges(size_t& start, size_t& count, size_t& particle_size) const;

		/** called whenever the allocation is removed from the layer */
		void OnRemovedFromLayer();
		/** require the layer to update the GPU buffer */
		void ConditionalRequireGPUUpdate(bool skip_if_invisible, bool skip_if_empty);

	protected:

		/** the particle layer that contains the range */
		ParticleLayerBase * layer = nullptr;
		/** whether the allocation is paused */
		bool paused = false;
		/** whether the allocation is visible */
		bool visible = true;
		/** a callback called whenever the allocation becomes empty */
		bool destroy_when_empty = false;
	};


	// ==============================================================
	// ParticleAllocation
	// ==============================================================

	template<typename LAYER_TRAIT>
	class /*CHAOS_API*/ ParticleAllocation : public ParticleAllocationBase, public DataOwner<typename get_AllocationTrait<LAYER_TRAIT>::type>
	{
        friend class ParticleLayer<LAYER_TRAIT>;

	public:

		using layer_trait_type = LAYER_TRAIT;
		using particle_type = typename layer_trait_type::particle_type;
		using vertex_type = typename layer_trait_type::vertex_type;
		using allocation_trait_type = typename get_AllocationTrait<layer_trait_type>::type;

		/** constructor */
		ParticleAllocation(ParticleLayerBase* in_layer, allocation_trait_type const & in_allocation_trait = {}) :
            ParticleAllocationBase(in_layer),
			DataOwner<allocation_trait_type>(in_allocation_trait)
        {
			assert(ClassManager::GetDefaultInstance()->FindCPPClass<particle_type>() != nullptr); // ensure class is declared
        }
		/** override */
		virtual Class const * GetParticleClass() const override
		{
			return ClassManager::GetDefaultInstance()->FindCPPClass<particle_type>();
		}

        /** override */
        virtual void* GetParticleBuffer() override
        {
            return (particles.size() == 0) ? nullptr : &particles[0];
        }
        /** override */
        virtual void const* GetParticleBuffer() const override
        {
            return (particles.size() == 0) ? nullptr : &particles[0];
        }
		/** override */
		virtual size_t GetParticleCount() const override
		{
			return particles.size();
		}
		/** override */
		virtual size_t GetParticleSize() const override
		{
			return sizeof(particle_type);
		}

		/** override */
		virtual AutoCastedParticleAccessor Resize(size_t new_count) override
		{
			// test whether this is valid
			if (!IsAttachedToLayer())
				return AutoCastedParticleAccessor(this, 0, 0);
			// early exit
            size_t old_count = particles.size();
			if (new_count == old_count)
				return AutoCastedParticleAccessor(this, 0, 0);

			// increment the number of particles
			particles.resize(new_count);
			// notify the layer
			ConditionalRequireGPUUpdate(true, false);
            // get the accessor on the new particles if any
            if (new_count < old_count)
                return AutoCastedParticleAccessor(this, 0, 0);
            return AutoCastedParticleAccessor(this, old_count, new_count - old_count);
		}

        /** transforms the particles into vertices in the buffer */
        void ParticlesToPrimitives(PrimitiveOutput<vertex_type>& output, layer_trait_type const* layer_trait) const
        {
			using Flags = ParticleToPrimitive_ImplementationFlags;

			constexpr int implementation_type = ParticleTraitTools::GetParticleToPrimitivesImplementationType<layer_trait_type>();

			constexpr int trait_implementation    = (implementation_type & Flags::TRAIT_IMPLEMENTATION);
			constexpr int default_implementation  = (implementation_type & Flags::DEFAULT_IMPLEMENTATION);
			constexpr int particle_implementation = (implementation_type & Flags::PARTICLE_IMPLEMENTATION);
			constexpr int with_begin_call         = (implementation_type & Flags::WITH_BEGIN_CALL);
			constexpr int with_allocation_trait   = (implementation_type & Flags::WITH_ALLOCATION_TRAIT);

			if constexpr (trait_implementation != 0)
			{
				if constexpr (with_allocation_trait != 0) // the member 'data' owns the information per allocation
				{
					if constexpr (with_begin_call != 0)
					{
						auto accessor = GetParticleConstAccessor<particle_type>();

						DoParticlesToPrimitivesLoop_LayerTraitImplementation(
							layer_trait,
							output,
							layer_trait->BeginParticlesToPrimitives(accessor, this->data), // do not use a temp variable, so it can be a left-value reference
							this->data);
					}
					else
					{
						DoParticlesToPrimitivesLoop_LayerTraitImplementation(layer_trait, output, this->data);
					}
				}
				else if constexpr (with_begin_call != 0)
				{
					auto accessor = GetParticleConstAccessor<particle_type>();

					DoParticlesToPrimitivesLoop_LayerTraitImplementation(
						layer_trait,
						output,
						layer_trait->BeginParticlesToPrimitives(accessor) // do not use a temp variable, so it can be a left-value reference
					);
				}
				else
				{
					DoParticlesToPrimitivesLoop_LayerTraitImplementation(layer_trait, output);
				}
			}
			else if constexpr (particle_implementation != 0)
			{
				DoParticlesToPrimitivesLoop_ParticleImplementation(output);
			}
			else if constexpr (default_implementation != 0)
			{
				DoParticlesToPrimitivesLoop_DefaultImplementation(output);
			}
        }

    protected:

		bool TickAllocation(float delta_time, layer_trait_type const * layer_trait)
		{
            bool destroy_allocation = false;
			if (particles.size() > 0)
				destroy_allocation = UpdateParticles(delta_time, layer_trait);
            return destroy_allocation;
		}

		bool UpdateParticles(float delta_time, layer_trait_type const * layer_trait)
		{
			using Flags = UpdateParticle_ImplementationFlags;

			constexpr int implementation_type = ParticleTraitTools::GetUpdateParticleImplementationFlags<layer_trait_type>();

			constexpr int trait_implementation		= (implementation_type & Flags::TRAIT_IMPLEMENTATION);
			constexpr int default_implementation	= (implementation_type & Flags::DEFAULT_IMPLEMENTATION);
			constexpr int particle_implementation	= (implementation_type & Flags::PARTICLE_IMPLEMENTATION);
			constexpr int with_begin_call			= (implementation_type & Flags::WITH_BEGIN_CALL);
			constexpr int with_allocation_trait		= (implementation_type & Flags::WITH_ALLOCATION_TRAIT);

			size_t particle_count = GetParticleCount();

			size_t remaining_particles = particle_count; // by default, no particle destruction

			ParticleAccessor<particle_type> particle_accessor = GetParticleAccessor();

			if constexpr (trait_implementation != 0)
			{
				if constexpr (with_allocation_trait != 0)
				{
					if constexpr (with_begin_call != 0)
					{
						remaining_particles = DoUpdateParticlesLoop(
							delta_time,
							layer_trait,
							particle_accessor,
							layer_trait->BeginUpdateParticles(delta_time, particle_accessor, this->data), // do not use a temp variable, so it can be a left-value reference
							this->allocation_trait);
					}
					else
					{
						remaining_particles = DoUpdateParticlesLoop(delta_time, layer_trait, particle_accessor, this->data);
					}
				}
				else if constexpr (with_begin_call != 0)
				{
					remaining_particles = DoUpdateParticlesLoop(
						delta_time,
						layer_trait,
						particle_accessor,
						layer_trait->BeginUpdateParticles(delta_time, particle_accessor)); // do not use a temp variable, so it can be a left-value reference
				}
				else
				{
					remaining_particles = DoUpdateParticlesLoop(delta_time, layer_trait, particle_accessor);
				}
			}
			else if constexpr (particle_implementation != 0)
			{
				remaining_particles = DoUpdateParticlesLoop(delta_time, layer_trait, particle_accessor);
			}
			else if constexpr (default_implementation != 0)
			{
				remaining_particles = DoUpdateParticlesLoop(delta_time, layer_trait, particle_accessor);
			}

			if (remaining_particles == 0 && GetDestroyWhenEmpty())
				return true; // destroy allocation
			else if (remaining_particles != particle_count) // clean buffer of all particles that have been destroyed
				Resize(remaining_particles);
			return false; // do not destroy allocation
		}

		template<typename ...PARAMS>
		size_t DoUpdateParticlesLoop(float delta_time, layer_trait_type const* layer_trait, ParticleAccessor<particle_type> particle_accessor, PARAMS && ...params)
		{
			using Flags = UpdateParticle_ImplementationFlags;

			constexpr int implementation_type = ParticleTraitTools::GetUpdateParticleImplementationFlags<layer_trait_type>();

			constexpr int trait_implementation    = (implementation_type & Flags::TRAIT_IMPLEMENTATION);
			constexpr int default_implementation  = (implementation_type & Flags::DEFAULT_IMPLEMENTATION);
			constexpr int particle_implementation = (implementation_type & Flags::PARTICLE_IMPLEMENTATION);

            size_t particle_count = particle_accessor.GetDataCount();

			// tick all particles. overide all particles that have been destroyed by next on the array
			size_t j = 0;
			for (size_t i = 0; i < particle_count; ++i)
			{
				particle_type& particle = particle_accessor[i];

				bool destroy_particle = false;
				if constexpr (trait_implementation != 0)
					destroy_particle = layer_trait->UpdateParticle(delta_time, particle, std::forward<PARAMS>(params)...);
				else if constexpr (particle_implementation != 0)
					destroy_particle = particle.UpdateParticle(delta_time, std::forward<PARAMS>(params)...);
				else if constexpr (default_implementation != 0)
					destroy_particle = UpdateParticle(delta_time, particle, std::forward<PARAMS>(params)...);

				if (!destroy_particle)
				{
					if (i != j)
						particle_accessor[j] = particle;
					++j;
				}
			}
			return j; // final number of particles
		}

        template<typename ...PARAMS>
		void DoParticlesToPrimitivesLoop_LayerTraitImplementation(layer_trait_type const * layer_trait, PrimitiveOutput<vertex_type>& output, PARAMS && ...params) const
        {
            ParticleConstAccessor<particle_type> particle_accessor = GetParticleAccessor();

			for (particle_type const & particle : particle_accessor)
				layer_trait->ParticleToPrimitives(particle, output, std::forward<PARAMS>(params)...);
        }

		template<typename ...PARAMS>
		void DoParticlesToPrimitivesLoop_ParticleImplementation(PrimitiveOutput<vertex_type>& output, PARAMS && ...params) const
		{
			ParticleConstAccessor<particle_type> particle_accessor = GetParticleAccessor();

			for (particle_type const& particle : particle_accessor)
				particle.ParticleToPrimitives(output, std::forward<PARAMS>(params)...);
		}

		template<typename ...PARAMS>
		void DoParticlesToPrimitivesLoop_DefaultImplementation(PrimitiveOutput<vertex_type>& output, PARAMS && ...params) const
		{
			ParticleConstAccessor<particle_type> particle_accessor = GetParticleAccessor();

			for (particle_type const& particle : particle_accessor)
				ParticleToPrimitives(particle, output, std::forward<PARAMS>(params)...);
		}

	protected:

		/** the particles buffer */
		std::vector<particle_type> particles;
	};

#endif

}; // namespace chaos