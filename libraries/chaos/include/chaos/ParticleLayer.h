#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUClasses.h>
#include <chaos/Object.h>
#include <chaos/GPURenderMaterial.h>
#include <chaos/GPUBuffer.h>
#include <chaos/GPUBufferCache.h>
#include <chaos/Object.h>
#include <chaos/GPUVertexDeclaration.h>
#include <chaos/GPUVertexArrayCache.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/GPUDrawPrimitive.h>
#include <chaos/GPUDynamicMesh.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/Class.h>
#include <chaos/ParticleTools.h>
#include <chaos/GPURenderable.h>
#include <chaos/ParticleAccessor.h>
#include <chaos/ParticleLayerTrait.h>
#include <chaos/PrimitiveOutput.h>
#include <chaos/ParticleSpawner.h>
#include <chaos/DataOwner.h>
#include <chaos/LogTools.h>

// ==============================================================
// ParticleAllocation<...>
// ==============================================================
//
// XXX : remark on ParticleAllocation
//
//         The ParticleAllocation life time is special. see ReferenceCount remark in .cpp !!! 
//
//
// ParticleAllocation<...> use a template class to define particles behavior
//
//   - The class of the PARTICLES
//   - The class of the VERTICES
//   - Ticking the PARTICLES
//   - Rendering PARTICLES (transforming PARTICLES into GPU primtives (quad, triangles ...))
//
//
// 1 - it can/must implement:
//
//    bool UpdateParticle(...) const;    => returns true if the particle must be destroyed
//
// and 
//
//    void ParticleToPrimitives(...) const;  
//
//
// ... but you could simply have
//
//
// 1.a - have the particle implements its own functions
//
//    particle.UpdateParticle(...)
//
//    particle.ParticleToPrimitives(...)
//
//
// 1.b - have simple independant function 
//
//    ParticleToPrimitive(... particle ...)
//
//    ParticleToPrimitives(... particle ...)
//
//
//
// 2 - If we have to make a computation on ALL particles before, we can implement the two following functions (just for the 1.1 cases)
//
//	  TYPE_XXX BeginUpdateParticles(...)
//
//	  TYPE_YYY BeginParticlesToPrimitives(...)
//
// in that case, the previous functions have an additionnal argument (just for 1.1 cases)
//
//    UpdateParticle(... TYPE_XXX)
//
//    ParticleToPrimitives(... TYPE_YYY)
//
// Example : we can compute a transform for the whole allocation (single call) and apply it to each particle
//
//
//
// 3 - if we may have a nested class AllocationTrait, so that the allocation has an instance of that (just for 1.1 cases)
//
//    class AllocationTrait { ... }
//
// in that case, the previous functions have an additionnal argument (just for 1.1 cases)
//
//    UpdateParticle(... TYPE_XXX, AllocationTrait) 
//
//    ParticleToPrimitives(... TYPE_YYY, AllocationTrait)
//
//	  TYPE_XXX BeginUpdateParticles(...AllocationTrait)
//
//	  TYPE_YYY BeginParticlesToPrimitives(...AllocationTrait)
//
//
// There are several rendering mode
//
//  - QUAD (transformed as triangle pair)
//  - INDEXED QUAD
//  - TRIANGLE_PAIR

namespace chaos
{
	// ==============================================================
	// FORWARD DECLARATION / FRIENDSHIP MACROS
	// ==============================================================

	// all classes in this file
#define CHAOS_PARTICLE_CLASSES \
(ParticleAllocationBase) \
(ParticleLayerBase) \
(ParticleManager)\
(AutoCastedParticleAccessor)\
(AutoCastedParticleConstAccessor)\
(ParticleSpawner)
	
	// forward declaration
#define CHAOS_PARTICLE_FORWARD_DECL(r, data, elem) class elem;
	BOOST_PP_SEQ_FOR_EACH(CHAOS_PARTICLE_FORWARD_DECL, _, CHAOS_PARTICLE_CLASSES)

		// friendship macro
#define CHAOS_PARTICLE_FRIEND_DECL(r, data, elem) friend class elem;
#define CHAOS_PARTICLE_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(CHAOS_PARTICLE_FRIEND_DECL, _, CHAOS_PARTICLE_CLASSES)

// detect whether classes have some functions
CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(Tick)
CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(UpdateParticle)
CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(BeginUpdateParticles)
CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(UpdateRenderingStates)

CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(ParticleToPrimitives)
CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(BeginParticlesToPrimitives)

// detect whether class have a nested class
CHAOS_GENERATE_HAS_TRAIT(AllocationTrait)

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

		// ==============================================================
		// ParticleAllocationBase
		// ==============================================================

	class ParticleAllocationBase : public Object
	{
		CHAOS_PARTICLE_ALL_FRIENDS

	public:

		/** constructor */
		ParticleAllocationBase(ParticleLayerBase * in_layer);
		/** destructor */
		virtual ~ParticleAllocationBase();

		/** Reference count updated : 2 use cases (external reference or not) */
		virtual void SubReference(SharedPointerPolicy policy) override;

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
		virtual AutoCastedParticleAccessor Resize(size_t new_count) { return AutoCastedParticleAccessor(this, 0, 0); }

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
			return Class::InheritsFrom(GetParticleClass(), Class::FindClass<PARTICLE_TYPE>(), true) == InheritanceType::YES;
		}

        /** get an AutoCasting particle accessor */
        AutoCastedParticleAccessor GetParticleAccessor(size_t start = 0, size_t count = 0)
        {
            return AutoCastedParticleAccessor(this, start, count);
        }
        /** get an AutoCasting particle accessor */
        AutoCastedParticleConstAccessor GetParticleAccessor(size_t start = 0, size_t count = 0) const
        {
            return AutoCastedParticleConstAccessor(this, start, count);
        }
        /** get an AutoCasting particle accessor */
        AutoCastedParticleConstAccessor GetParticleConstAccessor(size_t start = 0, size_t count = 0) const
        {
            return AutoCastedParticleConstAccessor(this, start, count);
        }

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

    // forward declaration (required for friendship)
    template<typename LAYER_TRAIT>
    class ParticleLayer;

	template<typename LAYER_TRAIT>
	class ParticleAllocation : public ParticleAllocationBase, public DataOwner<typename get_AllocationTrait<LAYER_TRAIT>::type>
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
			assert(Class::FindClass<particle_type>() != nullptr); // ensure class is declared
        }
		/** override */
		virtual Class const * GetParticleClass() const override
		{
			return Class::FindClass<particle_type>();
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
        template<typename PRIMITIVE_OUTPUT_TYPE>
        void ParticlesToPrimitives(PRIMITIVE_OUTPUT_TYPE& output, layer_trait_type const* layer_trait) const
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
						DoParticlesToPrimitivesLoop_LayerTraitImplementation(
							layer_trait,
							output,
							layer_trait->BeginParticlesToPrimitives(GetParticleConstAccessor<particle_type>(), data), // do not use a temp variable, so it can be a left-value reference
							data);
					}
					else
					{
						DoParticlesToPrimitivesLoop_LayerTraitImplementation(layer_trait, output, data);
					}
				}
				else if constexpr (with_begin_call != 0)
				{
					DoParticlesToPrimitivesLoop_LayerTraitImplementation(
						layer_trait,
						output,
						layer_trait->BeginParticlesToPrimitives(GetParticleConstAccessor<particle_type>()) // do not use a temp variable, so it can be a left-value reference
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
							layer_trait->BeginUpdateParticles(delta_time, particle_accessor, allocation_trait), // do not use a temp variable, so it can be a left-value reference
							allocation_trait);
					}
					else
					{
						remaining_particles = DoUpdateParticlesLoop(delta_time, layer_trait, particle_accessor, allocation_trait);
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
		size_t DoUpdateParticlesLoop(float delta_time, layer_trait_type const* layer_trait, ParticleAccessor<particle_type> particle_accessor, PARAMS... params)
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
					destroy_particle = layer_trait->UpdateParticle(delta_time, particle, params...);
				else if constexpr (particle_implementation != 0)
					destroy_particle = particle.UpdateParticle(delta_time, params ...);
				else if constexpr (default_implementation != 0)
					destroy_particle = UpdateParticle(delta_time, particle, params ...);
				
				if (!destroy_particle)
				{
					if (i != j)
						particle_accessor[j] = particle;
					++j;
				}
			}
			return j; // final number of particles
		}

        template<typename PRIMITIVE_OUTPUT_TYPE, typename ...PARAMS>
		void DoParticlesToPrimitivesLoop_LayerTraitImplementation(layer_trait_type const * layer_trait, PRIMITIVE_OUTPUT_TYPE& output, PARAMS... params) const
        {
            ParticleConstAccessor<particle_type> particle_accessor = GetParticleAccessor();

			for (particle_type const & particle : particle_accessor)
				layer_trait->ParticleToPrimitives(particle, output, params...);
        }

		template<typename PRIMITIVE_OUTPUT_TYPE, typename ...PARAMS>
		void DoParticlesToPrimitivesLoop_ParticleImplementation(PRIMITIVE_OUTPUT_TYPE& output, PARAMS... params) const
		{
			ParticleConstAccessor<particle_type> particle_accessor = GetParticleAccessor();

			for (particle_type const& particle : particle_accessor)
				particle.ParticleToPrimitives(output, params...);
		}

		template<typename PRIMITIVE_OUTPUT_TYPE, typename ...PARAMS>
		void DoParticlesToPrimitivesLoop_DefaultImplementation(PRIMITIVE_OUTPUT_TYPE& output, PARAMS... params) const
		{
			ParticleConstAccessor<particle_type> particle_accessor = GetParticleAccessor();

			for (particle_type const& particle : particle_accessor)
				ParticleToPrimitives(particle, output, params...);
		}

	protected:

		/** the particles buffer */
		std::vector<particle_type> particles;
	};

	// ==============================================================
	// ParticleLayerBase
	// ==============================================================

	class ParticleLayerBase : public GPURenderable
	{
		CHAOS_PARTICLE_ALL_FRIENDS

	public:

		/** constructor */
        ParticleLayerBase() = default;
		/** destructor */
		virtual ~ParticleLayerBase();

		/** returns true whether the class required is compatible with the one store in the buffer */
		template<typename PARTICLE_TYPE>
		bool IsParticleClassCompatible() const
		{
			return Class::InheritsFrom(GetParticleClass(), Class::FindClass<PARTICLE_TYPE>(), true) == InheritanceType::YES;
		}

		/** returns the number of particle count */
		size_t ComputeMaxParticleCount() const;

		/** returns the size in memory of a particle */
		virtual size_t GetParticleSize() const { return 0; }
        /** returns the number of particles */
        virtual size_t GetParticleCount() const { return 0; }
		/** returns the size in memory of a vertex */
		virtual size_t GetVertexSize() const { return 0; }
		/** returns the number of vertices required for each particles */
        virtual size_t GetRealVerticesPerParticle() const { return 0; }

		/** returns true whether vertices need to be updated */
		virtual bool AreVerticesDynamic() const { return true; }
		/** returns true whether particles need to be updated */
		virtual bool AreParticlesDynamic() const { return true; }

		/** get the particle ID for this system */
		virtual Class const * GetParticleClass() const { return nullptr; }

		/** returns true whether the particle type is the one given as template parameter */
		template<typename T>
		bool IsParticleType() const
		{
			return (GetParticleClass() == Class::FindClass<T>());
		}

        /** change the atlas */
        void SetTextureAtlas(BitmapAtlas::TextureArrayAtlas * in_atlas) { atlas = in_atlas; }
		/** change the material */
		void SetRenderMaterial(GPURenderMaterial * in_render_material) { render_material = in_render_material; }

		/** get the atlas const method */
		BitmapAtlas::TextureArrayAtlas const * GetTextureAtlas() const { return atlas.get(); }
		/** get the material const method */
		GPURenderMaterial const * GetRenderMaterial() const { return render_material.get(); }

		/** spawn a given number of particles */
		ParticleAllocationBase * SpawnParticles(size_t count, bool new_allocation = true);

		/** spawn + user initialization methods */
		template<typename INIT_PARTICLE_FUNC>
		ParticleAllocationBase* SpawnParticles(size_t count, bool new_allocation, INIT_PARTICLE_FUNC init_func)
		{
			ParticleAllocationBase* result = SpawnParticles(count, new_allocation);
			// call user initialization function
			if (result != nullptr)
			{
				size_t allocation_count = result->GetParticleCount();
				init_func(result->GetParticleAccessor(allocation_count - count, count));  // partial accessor, take the last particles in the array
			}
			return result;
		}

        /** create a particle spawner */
		template<typename ...PARAMS>
		ParticleSpawner* CreateParticleSpawner(PARAMS... params)
		{
			return new ParticleSpawner(this, params...);
		}

		/** create a particle spawner */
		template<typename ...PARAMS>
		ParticleSpawner GetParticleSpawner(PARAMS... params)
		{
			return ParticleSpawner(this, params...);
		}

		/** get the number of allocations */
		size_t GetAllocationCount() const;
		/** get the allocation by index */
		ParticleAllocationBase * GetAllocation(size_t index);
		/** get the allocation by index */
		ParticleAllocationBase const * GetAllocation(size_t index) const;
		/** clear all allocations */
		void ClearAllAllocations();

		/** get the vertex declaration */
		virtual GPUVertexDeclaration * GetVertexDeclaration() const { return nullptr; }

		/** get the trait */
		virtual AutoCastable<ParticleLayerTraitBase> GetLayerTrait() { return nullptr;  }
		/** get the trait */
		virtual AutoConstCastable<ParticleLayerTraitBase> GetLayerTrait() const { return nullptr; }

        /** returns the OpenGL primitive type */
        virtual GLenum GetGLPrimitiveType() const { return GL_NONE; }

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

		/** ticking the particle system */
		virtual bool DoTick(float delta_time) override;
		/** draw the layer */
		virtual int DoDisplay(GPURenderer * renderer, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params) override;

		/** change the GL rendering state */
		virtual void UpdateRenderingStates(GPURenderer * renderer, bool begin) const;

		/** unlink all particles allocations */
		void DetachAllParticleAllocations();
		/** internal method to remove a range from the layer */
		void RemoveParticleAllocation(ParticleAllocationBase * allocation);

		/** creation of an allocation */
		virtual ParticleAllocationBase* DoCreateParticleAllocation() { return nullptr; }

		/** update the vertex declaration */
		void UpdateVertexDeclaration();
		/** the effective rendering */
		int DoDisplayHelper(GPURenderer * renderer, GPURenderMaterial const * final_material, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params);

		/** internal method to update particles (returns true whether there was real changes) */
		bool TickAllocations(float delta_time);
		/** internal method to only update one allocation */
		virtual bool TickAllocation(float delta_time, ParticleAllocationBase* allocation) { return false; } // do not destroy the allocation

		/** override */
		virtual bool DoUpdateGPUResources(GPURenderer * renderer) override;

        /** select the PrimitiveOutput and update the rendering GPU resources */
        virtual void DoUpdateGPUBuffers(GPURenderer* renderer, size_t previous_frame_vertices_count) {}

        /** returns the number of vertices used in a dynamic mesh */
        size_t GetDynamicMeshVertexCount(GPUDynamicMesh const& mesh) const;
        /** evaluate how much memory will be required for GPUDynamicMesh (returns number of vertices) */
        size_t EvaluateGPUVertexMemoryRequirement() const;

	protected:

        /** the manager */
        ParticleManager * particle_manager = nullptr;
		/** the texture atlas */
		shared_ptr<BitmapAtlas::TextureArrayAtlas> atlas;
        /** particles allocations */
        std::vector<shared_ptr<ParticleAllocationBase>> particles_allocations;

		/** the material used to render the layer */
		shared_ptr<GPURenderMaterial> render_material;
        /** the vertex declaration */
        shared_ptr<GPUVertexDeclaration> vertex_declaration;

        /** the vertex cache (used when the layer is NOT in a manager) */
        shared_ptr<GPUVertexArrayCache> vertex_array_cache;
        /** the vertex cache (used when the layer is NOT in a manager) */
        GPUBufferCache buffer_cache;
        /** the corresponding dynamic mesh */
        GPUDynamicMesh dynamic_mesh;
        /** whether there was changes in particles, and a vertex array need to be recomputed */
        bool require_GPU_update = false;
	};

	// ==============================================================
	// ParticleLayer
	// ==============================================================

	template<typename LAYER_TRAIT>
	class ParticleLayer : public ParticleLayerBase, public DataOwner<LAYER_TRAIT>
	{
		static_assert(std::is_base_of_v<ParticleLayerTraitBase, LAYER_TRAIT>);

	public:

		using layer_trait_type = LAYER_TRAIT;
		using particle_type = typename layer_trait_type::particle_type;
		using vertex_type = typename layer_trait_type::vertex_type;

		using allocation_trait_type = typename get_AllocationTrait<layer_trait_type>::type;

		/** constructor */
		ParticleLayer(layer_trait_type const & in_layer_trait = {}) :
			DataOwner<layer_trait_type>(in_layer_trait)
		{
			assert(Class::FindClass<particle_type>() != nullptr); // ensure class is declared		
		}

		/** returns the size in memory of a particle */
		virtual size_t GetParticleSize() const override { return sizeof(particle_type); }
        /** override */        
        virtual size_t GetParticleCount() const override
        { 
            size_t result = 0;
			for (auto const& allocation : particles_allocations)
				if (allocation != nullptr)
					result += allocation->GetParticleCount();				
            return result;
        }
        /** override */
		virtual size_t GetVertexSize() const override { return sizeof(vertex_type); }
		/** override */
        virtual size_t GetRealVerticesPerParticle() const override
        {
            return chaos::GetRealVerticesPerParticle(ParticleTraitTools::GetPrimitiveType<layer_trait_type>()); // see PrimitiveOutput.h
        }
		/** override */
		virtual bool AreVerticesDynamic() const override 
		{ 			
			return data.dynamic_vertices;
		}
		/** override */
		virtual bool AreParticlesDynamic() const override 
		{ 			
			return data.dynamic_particles;
		}
		/** override */
		virtual Class const * GetParticleClass() const override { return Class::FindClass<particle_type>(); }
		/** override */
		virtual GPUVertexDeclaration * GetVertexDeclaration() const override 
		{
			GPUVertexDeclaration* result = new GPUVertexDeclaration;
			if (result != nullptr)
				GetTypedVertexDeclaration(result, boost::mpl::identity<vertex_type>()); 
			return result;
		}
        /** override */
        virtual GLenum GetGLPrimitiveType() const override 
		{
			return chaos::GetGLPrimitiveType(ParticleTraitTools::GetPrimitiveType<layer_trait_type>()); // see PrimitiveOutput.h
		}
		/** override */
		virtual AutoCastable<ParticleLayerTraitBase> GetLayerTrait() override { return &data; }
		/** override */
		virtual AutoConstCastable<ParticleLayerTraitBase> GetLayerTrait() const override { return &data; }

	protected:

		/** override */
		virtual ParticleAllocationBase * DoCreateParticleAllocation() override { return new ParticleAllocation<layer_trait_type>(this); }

		/** override */
		virtual bool TickAllocation(float delta_time, ParticleAllocationBase* in_allocation) 
		{ 
			ParticleAllocation<layer_trait_type>* allocation = auto_cast(in_allocation);
			if (allocation != nullptr)
				return allocation->TickAllocation(delta_time, &data);
			return false; // do not destroy the allocation
		} 

		/** override */
		virtual void UpdateRenderingStates(GPURenderer* renderer, bool begin) const override
		{
			if constexpr (check_method_UpdateRenderingStates_v<layer_trait_type const, GPURenderer *, bool>)
				data.UpdateRenderingStates(renderer, begin);
			else
				ParticleLayerBase::UpdateRenderingStates(renderer, begin);
		}

        /** override */
        virtual void DoUpdateGPUBuffers(GPURenderer* renderer, size_t vertex_requirement_evaluation) override
        {
            // some layers are in a manager, some not (see TiledMap)
            GPUBufferCache* cache = (particle_manager == nullptr) ? &buffer_cache : &particle_manager->GetBufferCache();

			constexpr PrimitiveType primitive_type = ParticleTraitTools::GetPrimitiveType<LAYER_TRAIT>();

            // select PrimitiveOutput and collect vertices
            if constexpr (primitive_type == PrimitiveType::TRIANGLE)
            {
                TriangleOutput<vertex_type> output(&dynamic_mesh, cache, vertex_declaration.get(), renderer, vertex_requirement_evaluation);
                ParticlesToPrimitivesLoop(output);
            }
            else if constexpr (primitive_type == PrimitiveType::TRIANGLE_PAIR)
            {
                TrianglePairOutput<vertex_type> output(&dynamic_mesh, cache, vertex_declaration.get(), renderer, vertex_requirement_evaluation);
                ParticlesToPrimitivesLoop(output);
            }
            else if constexpr (primitive_type == PrimitiveType::QUAD)
            {
                QuadOutput<vertex_type> output(&dynamic_mesh, cache, vertex_declaration.get(), renderer, vertex_requirement_evaluation);
                ParticlesToPrimitivesLoop(output);
            }
            else if constexpr (primitive_type == PrimitiveType::TRIANGLE_STRIP)
            {
                TriangleStripOutput<vertex_type> output(&dynamic_mesh, cache, vertex_declaration.get(), renderer, vertex_requirement_evaluation);
                ParticlesToPrimitivesLoop(output);
            }
            else if constexpr (primitive_type == PrimitiveType::TRIANGLE_FAN)
            {
                TriangleFanOutput<vertex_type> output(&dynamic_mesh, cache, vertex_declaration.get(), renderer, vertex_requirement_evaluation);
                ParticlesToPrimitivesLoop(output);
            }
        }

        // convert particles into vertices
        template<typename PRIMITIVE_OUTPUT_TYPE>
        void ParticlesToPrimitivesLoop(PRIMITIVE_OUTPUT_TYPE& output)
        {
            size_t count = particles_allocations.size();
            for (size_t i = 0; i < count; ++i)
            {
                // get the allocation, ignore if invisible
                ParticleAllocation<layer_trait_type> * allocation = auto_cast(particles_allocations[i].get());
                if (!allocation->IsVisible())
                    continue;
                // transform particles into vertices
                allocation->ParticlesToPrimitives(output, &data);
            }
            output.Flush();
        }
	};

	// undefine macros
	//#undef CHAOS_PARTICLE_CLASSES
	//#undef CHAOS_PARTICLE_FORWARD_DECL
	//#undef CHAOS_PARTICLE_FRIEND_DECL
	//#undef CHAOS_PARTICLE_ALL_FRIENDS

}; // namespace chaos

