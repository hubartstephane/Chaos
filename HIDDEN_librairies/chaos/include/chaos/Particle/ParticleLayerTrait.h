namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ParticleLayerTraitBase;

	template<typename PARTICLE_TYPE, typename VERTEX_TYPE, typename ALLOCATION_TYPE = EmptyClass>
	class ParticleLayerTrait;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// ==============================================================
	// ParticleLayerTraitBase
	// ==============================================================

	class CHAOS_API ParticleLayerTraitBase
	{
	public:

		/** ensure the trait can be auto_casted() */
		virtual ~ParticleLayerTraitBase() = default;

	public:

		/** whether the particles are dynamic */
		bool dynamic_particles = true;
		/** whether the vertices are dynamic */
		bool dynamic_vertices = true;
	};

	// ==============================================================
	// ParticleLayerTrait
	// ==============================================================

	// The default template
	template<typename PARTICLE_TYPE, typename VERTEX_TYPE, typename ALLOCATION_TYPE>
	class ParticleLayerTrait : public ParticleLayerTraitBase
	{
	public:

		using AllocationTrait = ALLOCATION_TYPE;

		/** the type for one particle */
		using particle_type = PARTICLE_TYPE;
		/** the type for one vertex */
		using vertex_type = VERTEX_TYPE;
	};

	// the specialization
	template<typename PARTICLE_TYPE, typename VERTEX_TYPE>
	class ParticleLayerTrait<PARTICLE_TYPE, VERTEX_TYPE, EmptyClass> : public ParticleLayerTraitBase
	{
	public:

		// XXX : no AllocationTrait for this specialization !!!

		/** the type for one particle */
		using particle_type = PARTICLE_TYPE;
		/** the type for one vertex */
		using vertex_type = VERTEX_TYPE;
	};

	// XXX : it is still possible to use a hand-made AllocationTrait by using the specialized version
	//
	//  class MyTrait : public ParticleLayerTrait<P, V>
	//  {
	//  public:
	//     class AllocationTrait { ... };
	//  };
	//

#endif

}; // namespace chaos