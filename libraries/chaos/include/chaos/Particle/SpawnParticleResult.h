namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class SpawnParticleResult;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class SpawnParticleResult
	{
	public:

		/** constructor */
		SpawnParticleResult(ParticleAllocationBase* in_allocation = nullptr, size_t in_start = 0, size_t in_count = 0) :
			allocation(in_allocation),
			start(in_start),
			count(in_count) {}
		/** copy constructor */
		SpawnParticleResult(SpawnParticleResult const& src) = default;

		/** conversion operator */
		operator ParticleAllocationBase* ()
		{
			return allocation;
		}
		/** conversion operator */
		operator ParticleAllocationBase const* () const
		{
			return allocation;
		}
		/** conversion operator */
		ParticleAllocationBase* operator ->()
		{
			return allocation;
		}
		/** conversion operator */
		ParticleAllocationBase const* operator -> () const
		{
			return allocation;
		}
		/** access all particles */
		template<typename FUNC>
		SpawnParticleResult& Process(FUNC func)
		{
			if (allocation != nullptr && count != 0)
				func(allocation->GetParticleAccessor(start, count));
			return *this; // so we can chain:   result = SpawnParticle(...).Process([](){...});
		}

		/** gets the start for this result */
		size_t GetAllocationStart() const { return start; }
		/** gets the count for this result */
		size_t GetAllocationCount() const { return count; }

	protected:

		/** the allocation corresponding to the result */
		ParticleAllocationBase* allocation = nullptr;
		/** the offset of the allocation where the allocation begins */
		size_t start = 0;
		/** the number of particles for this result */
		size_t count = 0;
	};


#endif

}; // namespace chaos