namespace chaos
{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ParticleAllocationBase: an object used to handle a bunch of particles
	 */

	class CHAOS_API ParticleAllocationBase : public Object, public SupervisableInterface
	{
		CHAOS_PARTICLE_ALL_FRIENDS

	public:

		/** constructor */
		ParticleAllocationBase(ParticleLayerBase * in_layer);
		/** destructor */
		virtual ~ParticleAllocationBase();

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
			return Class::InheritsFrom(GetParticleClass(), ClassManager::GetDefaultInstance()->FindCPPClass<PARTICLE_TYPE>(), true) == InheritanceType::Yes;
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

	protected:

		/** override */
		virtual void SubReference() override; // necessary to get rid of some warning ('dominance')
		/** override */
		virtual void RemoveFromSuperviser() override;

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

#endif

}; // namespace chaos