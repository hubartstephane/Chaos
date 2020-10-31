#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	struct ParticleToPrimitive_ImplementationFlags;
	struct UpdateParticle_ImplementationFlags;

}; // namespace chaos

#else 

namespace chaos
{
	// function detection 
	CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(Tick);
	CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(UpdateParticle);
	CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(BeginUpdateParticles);
	CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(UpdateRenderingStates);

	CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(ParticleToPrimitives);
	CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(BeginParticlesToPrimitives);

	// ==============================================================
	// The kind of ParticleToPrimitive to do
	// ==============================================================

// XXX: no enum so this can be implicitly converted to int
// XXX: no namespace so we can use 'using' to have more concise code
struct ParticleToPrimitive_ImplementationFlags
{
	// the kind of primitive emitted
	static constexpr int NONE = 0;
	static constexpr int TRIANGLE = 1;
	static constexpr int TRIANGLE_PAIR = 2;
	static constexpr int QUAD = 3;
	static constexpr int TRIANGLE_STRIP = 4;
	static constexpr int TRIANGLE_FAN = 5;
	// a mask for the primitive type
	static constexpr int PRIMITIVE_MASK = 7;

	// whether the trait has the implementation to use
	static constexpr int TRAIT_IMPLEMENTATION = 8;
	// whether the particle class itself has the implementation to use
	static constexpr int PARTICLE_IMPLEMENTATION = 16;
	// whether nor the particle class nor the trait class has an implementation. So whether to use the default implementation method
	static constexpr int DEFAULT_IMPLEMENTATION = 32;

	// for trait implementation, whether there is a ALLOCATION TRAIT to use in the call
	static constexpr int WITH_ALLOCATION_TRAIT = 64;
	// for trait implementation, whether there is a BEGIN to call before
	static constexpr int WITH_BEGIN_CALL = 128;
};

	// ==============================================================
	// The kind of UpdateParticle to do
	// ==============================================================

// XXX: no enum so this can be implicitly converted to int
// XXX: no namespace so we can use 'using' to have more concise code
struct UpdateParticle_ImplementationFlags 
{
	// no update required
	static constexpr int NONE = 0;
	// whether the trait has the implementation to use
	static constexpr int TRAIT_IMPLEMENTATION = 1;
	// whether the particle class itself has the implementation to use
	static constexpr int PARTICLE_IMPLEMENTATION = 2;
	// whether nor the particle class nor the trait class has an implementation. So whether to use the default implementation method
	static constexpr int DEFAULT_IMPLEMENTATION = 4;
	// for trait implementation, whether there is a ALLOCATION TRAIT to use in the call
	static constexpr int WITH_ALLOCATION_TRAIT = 8;
	// for trait implementation, whether there is a BEGIN to call before
	static constexpr int WITH_BEGIN_CALL = 16;
};

	// ==============================================================
	// ParticleTraitTools
	// ==============================================================

namespace ParticleTraitTools
{
	/** returns the kind of implementation required for the particle rendering */
	template<typename TRAIT_TYPE>
	constexpr int GetParticleToPrimitivesImplementationType()
	{
		// the types used 
		using trait = TRAIT_TYPE;

		using particle = typename trait::particle_type;
		using vertex = typename trait::vertex_type;
		using accessor = typename ParticleConstAccessor<particle>;

		using triangle_output = TriangleOutput<vertex>;
		using trianglepair_output = TrianglePairOutput<vertex>;
		using quad_output = QuadOutput<vertex>;
		using trianglestrip_output = TriangleStripOutput<vertex>;
		using trianglefan_output = TriangleFanOutput<vertex>;

		using Flags = ParticleToPrimitive_ImplementationFlags;

		// ============================== use implementation from TRAIT_TYPE ==============================
		if constexpr (has_AllocationTrait_v<trait>)
		{
			using allocation_trait = typename trait::AllocationTrait;

			// AllocationTrait + BeginParticlesToPrimitive
			if constexpr (check_method_BeginParticlesToPrimitives_v<trait const, accessor &, allocation_trait const &>)
			{
				using begin_result = typeof_method_BeginParticlesToPrimitives<trait const, accessor&, allocation_trait const &>;

				int base_flags = Flags::TRAIT_IMPLEMENTATION | Flags::WITH_BEGIN_CALL | Flags::WITH_ALLOCATION_TRAIT;

				if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, triangle_output&, begin_result, allocation_trait const &>)
					return Flags::TRIANGLE | base_flags;
				if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglepair_output&, begin_result, allocation_trait const &>)
					return Flags::TRIANGLE_PAIR | base_flags;
				if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, quad_output&, begin_result, allocation_trait const &>)
					return Flags::QUAD | base_flags;
				if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglestrip_output&, begin_result, allocation_trait const &>)
					return Flags::TRIANGLE_STRIP | base_flags;
				if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglefan_output&, begin_result, allocation_trait const &>)
					return Flags::TRIANGLE_FAN | base_flags;
			}

			// AllocationTrait - NO BEGIN
			{
				int base_flags = Flags::TRAIT_IMPLEMENTATION | Flags::WITH_ALLOCATION_TRAIT;

				if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, triangle_output&, allocation_trait const &>)
					return Flags::TRIANGLE | base_flags;
				if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglepair_output&, allocation_trait const &>)
					return Flags::TRIANGLE_PAIR | base_flags;
				if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, quad_output&, allocation_trait const &>)
					return Flags::QUAD | base_flags;
				if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglestrip_output&, allocation_trait const &>)
					return Flags::TRIANGLE_STRIP | base_flags;
				if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglefan_output&, allocation_trait const &>)
					return Flags::TRIANGLE_FAN | base_flags;
			}
		}

		// NO ALLOCATION TRAIT + BeginParticlesToPrimitive
		if constexpr (check_method_BeginParticlesToPrimitives_v<trait const, accessor&>)
		{
			using begin_result = typeof_method_BeginParticlesToPrimitives<trait const, accessor&>;

			int base_flags = Flags::TRAIT_IMPLEMENTATION | Flags::WITH_BEGIN_CALL;

			if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, triangle_output&, begin_result>)
				return Flags::TRIANGLE | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglepair_output&, begin_result>)
				return Flags::TRIANGLE_PAIR | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, quad_output&, begin_result>)
				return Flags::QUAD | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglestrip_output&, begin_result>)
				return Flags::TRIANGLE_STRIP | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglefan_output&, begin_result>)
				return Flags::TRIANGLE_FAN | base_flags;
		}

		// NO ALLOCATION TRAIT - NO BEGIN
		{
			int base_flags = Flags::TRAIT_IMPLEMENTATION;

			if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, triangle_output&>)
				return Flags::TRIANGLE | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglepair_output&>)
				return Flags::TRIANGLE_PAIR | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, quad_output&>)
				return Flags::QUAD | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglestrip_output&>)
				return Flags::TRIANGLE_STRIP | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglefan_output&>)
				return Flags::TRIANGLE_FAN | base_flags;
		}

		// ============================== use implementation from PARTICLE ITSELF ==============================
		{
			int base_flags = Flags::PARTICLE_IMPLEMENTATION;

			if constexpr (check_method_ParticleToPrimitives_v<particle const&, triangle_output&>)
				return Flags::TRIANGLE | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<particle const&, trianglepair_output&>)
				return Flags::TRIANGLE_PAIR | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<particle const&, quad_output&>)
				return Flags::QUAD | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<particle const&, trianglestrip_output&>)
				return Flags::TRIANGLE_STRIP | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<particle const&, trianglefan_output&>)
				return Flags::TRIANGLE_FAN | base_flags;
		}

		// ============================== use implementation DEFAULT ==============================
		{
			int base_flags = Flags::DEFAULT_IMPLEMENTATION;

			if constexpr (check_function_ParticleToPrimitives_v<particle const&, triangle_output&>)
				return Flags::TRIANGLE | base_flags;
			if constexpr (check_function_ParticleToPrimitives_v<particle const&, trianglepair_output&>)
				return Flags::TRIANGLE_PAIR | base_flags;
			if constexpr (check_function_ParticleToPrimitives_v<particle const&, quad_output&>)
				return Flags::QUAD | base_flags;
			if constexpr (check_function_ParticleToPrimitives_v<particle const&, trianglestrip_output&>)
				return Flags::TRIANGLE_STRIP | base_flags;
			if constexpr (check_function_ParticleToPrimitives_v<particle const&, trianglefan_output&>)
				return Flags::TRIANGLE_FAN | base_flags;
		}
		return Flags::NONE;
	}

	/** returns the primitive type used for the rendering */
	template<typename TRAIT_TYPE>
	constexpr PrimitiveType GetPrimitiveType()
	{
		using Flags = ParticleToPrimitive_ImplementationFlags;

		constexpr int implementation_type = GetParticleToPrimitivesImplementationType<TRAIT_TYPE>();
		constexpr int primitive_type = (implementation_type & Flags::PRIMITIVE_MASK);

		if constexpr (primitive_type == Flags::TRIANGLE_PAIR)
			return PrimitiveType::TRIANGLE_PAIR;
		if constexpr (primitive_type == Flags::QUAD)
			return PrimitiveType::QUAD;
		if constexpr (primitive_type == Flags::TRIANGLE)
			return PrimitiveType::TRIANGLE;
		if ((implementation_type & Flags::PRIMITIVE_MASK) == Flags::TRIANGLE_STRIP)
			return PrimitiveType::TRIANGLE_STRIP;
		if ((implementation_type & Flags::PRIMITIVE_MASK) == Flags::TRIANGLE_FAN)
			return PrimitiveType::TRIANGLE_FAN;	

		return PrimitiveType::NONE;
	}

	/** returns the kind of implementation required for the particle update */
	template<typename TRAIT_TYPE>
	constexpr int GetUpdateParticleImplementationFlags()
	{
		using Flags = UpdateParticle_ImplementationFlags;

		// the types used 
		using trait = TRAIT_TYPE;

		using particle = typename trait::particle_type;
		using vertex = typename trait::vertex_type;
		using accessor = typename ParticleAccessor<particle>;

		// ============================== use implementation from TRAIT_TYPE ==============================
		if constexpr (has_AllocationTrait_v<trait>)
		{
			using allocation_trait = typename trait::AllocationTrait;

			// AllocationTrait + BeginUpdateParticle
			if constexpr (check_method_BeginUpdateParticles_v<trait const, float, accessor&, allocation_trait const &>)
			{
				using begin_result = typeof_method_BeginUpdateParticles<trait const, float, accessor&, allocation_trait const &>;

				if constexpr (check_method_UpdateParticle_v<trait const, float, particle &, begin_result, allocation_trait const &>)
					return Flags::TRAIT_IMPLEMENTATION | Flags::WITH_BEGIN_CALL | Flags::WITH_ALLOCATION_TRAIT;
			}
			// AllocationTrait - NO BEGIN
			if constexpr (check_method_UpdateParticle_v<trait const, float, particle&, allocation_trait const &>)
			{
				return Flags::TRAIT_IMPLEMENTATION | Flags::WITH_ALLOCATION_TRAIT;
			}
		}

		// NO ALLOCATION TRAIT + BeginUpdateParticle
		if constexpr (check_method_BeginUpdateParticles_v<trait const, float, accessor&>)
		{
			using begin_result = typeof_method_BeginUpdateParticles<trait const, float, accessor&>;

			if constexpr (check_method_UpdateParticle_v<trait const, float, particle&, begin_result>)
			{
				return Flags::TRAIT_IMPLEMENTATION | Flags::WITH_BEGIN_CALL;
			}
		}
		// NOLAYERTRAIT - NO BEGIN
		if constexpr (check_method_UpdateParticle_v<trait const, float, particle&>)
		{
			return Flags::TRAIT_IMPLEMENTATION;
		}

		// ============================== use implementation from PARTICLE ITSELF ==============================
		if constexpr (check_method_UpdateParticle_v<particle, float>)
		{
			return Flags::PARTICLE_IMPLEMENTATION;
		}

		// ============================== use implementation DEFAULT ==============================
		if constexpr (check_function_UpdateParticle_v<float, particle&>)
		{
			return Flags::DEFAULT_IMPLEMENTATION;
		}

		return Flags::NONE;
	}
};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
