#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUClasses.h>
#include <chaos/ReferencedObject.h>
#include <chaos/GPURenderMaterial.h>
#include <chaos/GPUBuffer.h>
#include <chaos/GPUBufferCache.h>
#include <chaos/ReferencedObject.h>
#include <chaos/GPUVertexDeclaration.h>
#include <chaos/GPUVertexArrayCache.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/GPUDrawPrimitive.h>
#include <chaos/GPUDynamicMesh.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/ClassTools.h>
#include <chaos/ParticleTools.h>
#include <chaos/GPURenderable.h>
#include <chaos/ParticleAccessor.h>
#include <chaos/ParticleAllocationTrait.h>
#include <chaos/PrimitiveOutput.h>
#include <chaos/ParticleSpawner.h>

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
// 3 - if we may have a nested class LayerTrait, so that the layer has an instance of that (just for 1.1 cases)
//
//    class LayerTrait { ... }
//
// in that case, the previous functions have an additionnal argument (just for 1.1 cases)
//
//    UpdateParticle(... LayerTrait, TYPE_XXX) 
//
//    ParticleToPrimitives(... LayerTrait, TYPE_YYY)
//
//	  TYPE_XXX BeginUpdateParticles(...LayerTrait)
//
//	  TYPE_YYY BeginParticlesToPrimitives(...LayerTrait)
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
(ParticleManager)

	// forward declaration
#define CHAOS_PARTICLE_FORWARD_DECL(r, data, elem) class elem;
	BOOST_PP_SEQ_FOR_EACH(CHAOS_PARTICLE_FORWARD_DECL, _, CHAOS_PARTICLE_CLASSES)

		// friendship macro
#define CHAOS_PARTICLE_FRIEND_DECL(r, data, elem) friend class elem;
#define CHAOS_PARTICLE_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(CHAOS_PARTICLE_FRIEND_DECL, _, CHAOS_PARTICLE_CLASSES)

// detect whether class has a member named XXX (use has_XXX<T>::value => bool => convert into boost::mpl::bool_)
CHAOS_GENERATE_HAS_MEMBER(dynamic_particles);
CHAOS_GENERATE_HAS_MEMBER(dynamic_vertices);

// detect whether classes have some functions
CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(Tick)
CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(UpdateParticle)
CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(BeginUpdateParticles)

CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(ParticleToPrimitives)
CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(BeginParticlesToPrimitives)

// detect whether class have a nested class
CHAOS_GENERATE_HAS_TRAIT(LayerTrait)

// ==============================================================
// The kind of ParticleToPrimitive to do
// ==============================================================

namespace ParticleToPrimitiveImplementationType  // XXXX: no enum, no class, so this can be implicitly converted to int
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

	// for trait implementation, whether there is a LAYER TRAIT to use in the call
	static constexpr int WITH_LAYER_TRAIT = 64;
	// for trait implementation, whether there is a BEGIN to call before
	static constexpr int WITH_BEGIN_CALL = 128;
};

// ==============================================================
// The kind of UpdateParticle to do
// ==============================================================

namespace UpdateParticleImplementationType // XXXX: no enum, no class, so this can be implicitly converted to int
{
	// no update required
	static constexpr int NONE = 0;
	// whether the trait has the implementation to use
	static constexpr int TRAIT_IMPLEMENTATION = 1;
	// whether the particle class itself has the implementation to use
	static constexpr int PARTICLE_IMPLEMENTATION = 2;
	// whether nor the particle class nor the trait class has an implementation. So whether to use the default implementation method
	static constexpr int DEFAULT_IMPLEMENTATION = 4;
	// for trait implementation, whether there is a LAYER TRAIT to use in the call
	static constexpr int WITH_LAYER_TRAIT = 8;
	// for trait implementation, whether there is a BEGIN to call before
	static constexpr int WITH_BEGIN_CALL = 16;
};

// ==============================================================
// ParticleTraitTools
// ==============================================================

class ParticleTraitTools
{

public:

	/** returns whether the vertices are dynamic */
	template<typename TRAIT_TYPE>
	static bool AreVerticesDynamic(TRAIT_TYPE const & trait)
	{
        if constexpr (has_dynamic_vertices_v<TRAIT_TYPE>)
            return trait.dynamic_vertices;
        return true;
	}
	/** returns whether the particles are dynamic */
	template<typename TRAIT_TYPE>
	static bool AreParticlesDynamic(TRAIT_TYPE const & trait)
	{
        if constexpr (has_dynamic_particles_v<TRAIT_TYPE>)
            return trait.dynamic_particles;
        return true;
	}

	/** returns whether the vertices are dynamic (without an instance to read) */
	template<typename TRAIT_TYPE>
	static bool AreVerticesDynamicStatic()
	{
        if constexpr (has_dynamic_vertices_v<TRAIT_TYPE>)
            return TRAIT_TYPE::dynamic_vertices;
        return true;
    }
	/** returns whether the particles are dynamic (without an instance to read) */
	template<typename TRAIT_TYPE>
	static bool AreParticlesDynamicStatic()
	{
        if constexpr (has_dynamic_particles_v<TRAIT_TYPE>)
            return TRAIT_TYPE::dynamic_particles;
        return true;
	}

    /** returns the primitive type used for rendering (OpenGL point of view) */
    template<typename TRAIT_TYPE>
    static constexpr GLenum GetGLPrimitiveType()
    {
        return chaos::GetGLPrimitiveType(GetPrimitiveType<TRAIT_TYPE>()); // see PrimitiveOutput.h
    }

	/** returns the kind of implementation required for the particle rendering */
	template<typename TRAIT_TYPE>
	static constexpr int GetParticleToPrimitivesImplementationType()
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

		// ============================== use implementation from TRAIT_TYPE ==============================
		if constexpr (has_LayerTrait_v<trait>)
		{
			using layer_trait = typename trait::LayerTrait;

			// LayerTrait + BeginParticlesToPrimitive
			if constexpr (check_method_BeginParticlesToPrimitives_v<trait const, accessor &, layer_trait const*>)
			{
				using begin_result = typeof_method_BeginParticlesToPrimitives<trait const, accessor&, layer_trait const*>;

				int base_flags = ParticleToPrimitiveImplementationType::TRAIT_IMPLEMENTATION | ParticleToPrimitiveImplementationType::WITH_BEGIN_CALL | ParticleToPrimitiveImplementationType::WITH_LAYER_TRAIT;

				if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, triangle_output&, layer_trait const*, begin_result>)
					return ParticleToPrimitiveImplementationType::TRIANGLE | base_flags;
				if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglepair_output&, layer_trait const*, begin_result>)
					return ParticleToPrimitiveImplementationType::TRIANGLE_PAIR | base_flags;
				if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, quad_output&, layer_trait const*, begin_result>)
					return ParticleToPrimitiveImplementationType::QUAD | base_flags;
				if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglestrip_output&, layer_trait const*, begin_result>)
					return ParticleToPrimitiveImplementationType::TRIANGLE_STRIP | base_flags;
				if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglefan_output&, layer_trait const*, begin_result>)
					return ParticleToPrimitiveImplementationType::TRIANGLE_FAN | base_flags;
			}

			// LayerTrait - NOBEGIN
			{
				int base_flags = ParticleToPrimitiveImplementationType::TRAIT_IMPLEMENTATION | ParticleToPrimitiveImplementationType::WITH_LAYER_TRAIT;

				if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, triangle_output&, layer_trait const*>)
					return ParticleToPrimitiveImplementationType::TRIANGLE | base_flags;
				if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglepair_output&, layer_trait const*>)
					return ParticleToPrimitiveImplementationType::TRIANGLE_PAIR | base_flags;
				if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, quad_output&, layer_trait const*>)
					return ParticleToPrimitiveImplementationType::QUAD | base_flags;
				if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglestrip_output&, layer_trait const*>)
					return ParticleToPrimitiveImplementationType::TRIANGLE_STRIP | base_flags;
				if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglefan_output&, layer_trait const*>)
					return ParticleToPrimitiveImplementationType::TRIANGLE_FAN | base_flags;
			}
		}

		// NOLAYERTRAIT + BeginParticlesToPrimitive
		if constexpr (check_method_BeginParticlesToPrimitives_v<trait const, accessor&>)
		{
			using begin_result = typeof_method_BeginParticlesToPrimitives<trait const, accessor&>;

			int base_flags = ParticleToPrimitiveImplementationType::TRAIT_IMPLEMENTATION | ParticleToPrimitiveImplementationType::WITH_BEGIN_CALL;

			if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, triangle_output&, begin_result>)
				return ParticleToPrimitiveImplementationType::TRIANGLE | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglepair_output&, begin_result>)
				return ParticleToPrimitiveImplementationType::TRIANGLE_PAIR | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, quad_output&, begin_result>)
				return ParticleToPrimitiveImplementationType::QUAD | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglestrip_output&, begin_result>)
				return ParticleToPrimitiveImplementationType::TRIANGLE_STRIP | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglefan_output&, begin_result>)
				return ParticleToPrimitiveImplementationType::TRIANGLE_FAN | base_flags;
		}

		// NOLAYERTRAIT - NOBEGIN
		{
			int base_flags = ParticleToPrimitiveImplementationType::TRAIT_IMPLEMENTATION;

			if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, triangle_output&>)
				return ParticleToPrimitiveImplementationType::TRIANGLE | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglepair_output&>)
				return ParticleToPrimitiveImplementationType::TRIANGLE_PAIR | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, quad_output&>)
				return ParticleToPrimitiveImplementationType::QUAD | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglestrip_output&>)
				return ParticleToPrimitiveImplementationType::TRIANGLE_STRIP | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<trait const, particle const&, trianglefan_output&>)
				return ParticleToPrimitiveImplementationType::TRIANGLE_FAN | base_flags;
		}

		// ============================== use implementation from PARTICLE ITSELF ==============================
		{
			int base_flags = ParticleToPrimitiveImplementationType::PARTICLE_IMPLEMENTATION;

			if constexpr (check_method_ParticleToPrimitives_v<particle const&, triangle_output&>)
				return ParticleToPrimitiveImplementationType::TRIANGLE | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<particle const&, trianglepair_output&>)
				return ParticleToPrimitiveImplementationType::TRIANGLE_PAIR | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<particle const&, quad_output&>)
				return ParticleToPrimitiveImplementationType::QUAD | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<particle const&, trianglestrip_output&>)
				return ParticleToPrimitiveImplementationType::TRIANGLE_STRIP | base_flags;
			if constexpr (check_method_ParticleToPrimitives_v<particle const&, trianglefan_output&>)
				return ParticleToPrimitiveImplementationType::TRIANGLE_FAN | base_flags;
		}

		// ============================== use implementation DEFAULT ==============================
		{
			int base_flags = ParticleToPrimitiveImplementationType::DEFAULT_IMPLEMENTATION;

			if constexpr (check_function_ParticleToPrimitives_v<particle const&, triangle_output&>)
				return ParticleToPrimitiveImplementationType::TRIANGLE | base_flags;
			if constexpr (check_function_ParticleToPrimitives_v<particle const&, trianglepair_output&>)
				return ParticleToPrimitiveImplementationType::TRIANGLE_PAIR | base_flags;
			if constexpr (check_function_ParticleToPrimitives_v<particle const&, quad_output&>)
				return ParticleToPrimitiveImplementationType::QUAD | base_flags;
			if constexpr (check_function_ParticleToPrimitives_v<particle const&, trianglestrip_output&>)
				return ParticleToPrimitiveImplementationType::TRIANGLE_STRIP | base_flags;
			if constexpr (check_function_ParticleToPrimitives_v<particle const&, trianglefan_output&>)
				return ParticleToPrimitiveImplementationType::TRIANGLE_FAN | base_flags;
		}
		return ParticleToPrimitiveImplementationType::NONE;
	}

	/** returns the primitive type used for the rendering */
	template<typename TRAIT_TYPE>
	static constexpr PrimitiveType GetPrimitiveType()
	{
		constexpr int implementation_type = GetParticleToPrimitivesImplementationType<TRAIT_TYPE>();
		constexpr int primitive_type = (implementation_type & ParticleToPrimitiveImplementationType::PRIMITIVE_MASK);

		if constexpr (primitive_type == ParticleToPrimitiveImplementationType::TRIANGLE_PAIR)
			return PrimitiveType::TRIANGLE_PAIR;
		if constexpr (primitive_type == ParticleToPrimitiveImplementationType::QUAD)
			return PrimitiveType::QUAD;
		if constexpr (primitive_type == ParticleToPrimitiveImplementationType::TRIANGLE)
			return PrimitiveType::TRIANGLE;
		if ((implementation_type & ParticleToPrimitiveImplementationType::PRIMITIVE_MASK) == ParticleToPrimitiveImplementationType::TRIANGLE_STRIP)
			return PrimitiveType::TRIANGLE_STRIP;
		if ((implementation_type & ParticleToPrimitiveImplementationType::PRIMITIVE_MASK) == ParticleToPrimitiveImplementationType::TRIANGLE_FAN)
			return PrimitiveType::TRIANGLE_FAN;	

		return PrimitiveType::NONE;
	}

	/** returns the kind of implementation required for the particle update */
	template<typename TRAIT_TYPE>
	static constexpr int GetUpdateParticleImplementationType()
	{

		// the types used 
		using trait = TRAIT_TYPE;
		using particle = typename trait::particle_type;
		using vertex = typename trait::vertex_type;
		using accessor = typename ParticleAccessor<particle>;

		// ============================== use implementation from TRAIT_TYPE ==============================
		if constexpr (has_LayerTrait_v<trait>)
		{
			using layer_trait = typename trait::LayerTrait;

			// LayerTrait + BeginUpdateParticle
			if constexpr (check_method_BeginUpdateParticles_v<trait const, float, accessor&, layer_trait const*>)
			{
				using begin_result = typeof_method_BeginUpdateParticles<trait const, float, accessor&, layer_trait const*>;

				if constexpr (check_method_UpdateParticle_v<trait const, float, particle &, begin_result, layer_trait const*>)
					return UpdateParticleImplementationType::TRAIT_IMPLEMENTATION | UpdateParticleImplementationType::WITH_BEGIN_CALL | UpdateParticleImplementationType::WITH_LAYER_TRAIT;
			}
			// LayerTrait - NOBEGIN
			if constexpr (check_method_UpdateParticle_v<trait const, float, particle&, layer_trait const*>)
				return UpdateParticleImplementationType::TRAIT_IMPLEMENTATION | UpdateParticleImplementationType::WITH_LAYER_TRAIT;
		}

		// NOLAYERTRAIT + BeginUpdateParticle
		if constexpr (check_method_BeginUpdateParticles_v<trait const, float, accessor&>)
		{
			using begin_result = typeof_method_BeginUpdateParticles<trait const, float, accessor&>;

			if constexpr (check_method_UpdateParticle_v<trait const, float, particle&, begin_result>)
				return UpdateParticleImplementationType::TRAIT_IMPLEMENTATION | UpdateParticleImplementationType::WITH_BEGIN_CALL;
		}
		// NOLAYERTRAIT - NOBEGIN
		if constexpr (check_method_UpdateParticle_v<trait const, float, particle&>)
			return UpdateParticleImplementationType::TRAIT_IMPLEMENTATION;

		// ============================== use implementation from PARTICLE ITSELF ==============================
		{
			if constexpr (check_method_UpdateParticle_v<particle const&, float>)
				return UpdateParticleImplementationType::PARTICLE_IMPLEMENTATION;
		}

		// ============================== use implementation DEFAULT ==============================
		{
			if constexpr (check_function_UpdateParticle_v<float, particle const&>)
				return UpdateParticleImplementationType::DEFAULT_IMPLEMENTATION;
		}
		return UpdateParticleImplementationType::NONE;
	}
};

		// ==============================================================
		// ParticleAllocationBase
		// ==============================================================

	class ParticleAllocationBase : public ReferencedObject
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
		virtual ClassTools::ClassRegistration const * GetParticleClass() const { return nullptr; }

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

		/** returns true whether the class required is compatible with the one store in the buffer */
		template<typename PARTICLE_TYPE>
		bool IsParticleClassCompatible(bool accept_bigger_particle) const
		{
			return ParticleTools::IsParticleClassCompatible<PARTICLE_TYPE>(GetParticleClass(), GetParticleSize(), accept_bigger_particle);
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
			assert(IsParticleClassCompatible<PARTICLE_TYPE>(true));
            
            size_t particle_size = 0;
            void* buffer = const_cast<void *>(GetAccessorEffectiveRanges(start, count, particle_size));
            if (buffer == nullptr)
                return ParticleAccessor<PARTICLE_TYPE>();
			return ParticleAccessor<PARTICLE_TYPE>(buffer, count, particle_size);
		}

		/** get an accessor for the particles */
		template<typename PARTICLE_TYPE>
		ParticleConstAccessor<PARTICLE_TYPE> GetParticleConstAccessor(size_t start = 0, size_t count = 0) const
		{
			assert(IsParticleClassCompatible<PARTICLE_TYPE>(true));

            size_t particle_size = 0;
            void const * buffer = GetAccessorEffectiveRanges(start, count, particle_size);
            if (buffer == nullptr)
                return ParticleConstAccessor<PARTICLE_TYPE>();
            return ParticleConstAccessor<PARTICLE_TYPE>(buffer, count, particle_size);
		}

		/** get an accessor for the particles */
		template<typename PARTICLE_TYPE>
		ParticleConstAccessor<PARTICLE_TYPE> GetParticleAccessor(size_t start = 0, size_t count = 0) const
		{
			return GetParticleConstAccessor<PARTICLE_TYPE>();
		}
		
		/** get the layer for this allocation */
		ParticleLayerBase * GetLayer() { return layer; }
		/** get the layer for this allocation */
		ParticleLayerBase const * GetLayer() const { return layer; }

		/** remove the allocation from its layer */
		void RemoveFromLayer();

	protected:

        /** compute the ranges for accessor (returns false in case of failure) */
        void const * GetAccessorEffectiveRanges(size_t& start, size_t& count, size_t& particle_size) const;
		/** tick the allocation (returns true whether the allocation is to be destroyed) */
		virtual bool TickAllocation(float delta_time, void const * layer_trait) { return false; }
		/** transforms the particles into vertices in the buffer */
		virtual size_t ParticlesToPrimitives(void * vertices, void const * layer_trait) const { return 0; }

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

    // forward declaration (required for friendship)
    template<typename ALLOCATION_TRAIT>
    class ParticleLayer;

	template<typename ALLOCATION_TRAIT>
	class ParticleAllocation : public ParticleAllocationBase
	{
        friend class ParticleLayer<ALLOCATION_TRAIT>;

	public:

		using allocation_trait_type = ALLOCATION_TRAIT;
		using particle_type = typename allocation_trait_type::particle_type;
		using vertex_type = typename allocation_trait_type::vertex_type;
		using layer_trait_type = typename get_LayerTrait<allocation_trait_type>::type;

		/** constructor */
		ParticleAllocation(ParticleLayerBase* in_layer) : 
            ParticleAllocationBase(in_layer)
        {
        }
		/** override */
		virtual ClassTools::ClassRegistration const * GetParticleClass() const override
		{
			return ClassTools::GetClassRegistration<particle_type>();
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
        void ParticlesToPrimitives(PRIMITIVE_OUTPUT_TYPE& output, void const* layer_trait) const
        {		
			constexpr int implementation_type = ParticleTraitTools::GetParticleToPrimitivesImplementationType<ALLOCATION_TRAIT>();

			constexpr int trait_implementation    = (implementation_type & ParticleToPrimitiveImplementationType::TRAIT_IMPLEMENTATION);
			constexpr int default_implementation  = (implementation_type & ParticleToPrimitiveImplementationType::DEFAULT_IMPLEMENTATION);
			constexpr int particle_implementation = (implementation_type & ParticleToPrimitiveImplementationType::PARTICLE_IMPLEMENTATION);
			constexpr int with_begin_call         = (implementation_type & ParticleToPrimitiveImplementationType::WITH_BEGIN_CALL);
			constexpr int with_layer_trait        = (implementation_type & ParticleToPrimitiveImplementationType::WITH_LAYER_TRAIT);

			if constexpr (trait_implementation != 0)
			{
				if constexpr (with_layer_trait != 0)
				{
					layer_trait_type const* typed_layer_trait = (layer_trait_type const*)layer_trait;

					if constexpr (with_begin_call != 0)
					{
						DoParticlesToPrimitivesLoop(
							output,
							allocation_trait.BeginParticlesToPrimitives(GetParticleConstAccessor<particle_type>(), typed_layer_trait), // do not use a temp variable, so it can be a left-value reference
							typed_layer_trait);
					}
					else
					{
						DoParticlesToPrimitivesLoop(output, typed_layer_trait);
					}
				}
				else if constexpr (with_begin_call != 0)
				{
					DoParticlesToPrimitivesLoop(
						output,
						allocation_trait.BeginParticlesToPrimitives(GetParticleConstAccessor<particle_type>()) // do not use a temp variable, so it can be a left-value reference
					);
				}
				else
				{
					DoParticlesToPrimitivesLoop(output);
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

		/** override */
		virtual bool TickAllocation(float delta_time, void const * layer_trait) override
		{ 
            layer_trait_type const* typed_layer_trait = (layer_trait_type const*)layer_trait;

            bool destroy_allocation = false;

#if 0


            // tick the allocation (if the trait has a Tick(...) function)
            if constexpr (has_function_Tick_v<allocation_trait_type>)
            {
                if constexpr (has_LayerTrait_v<allocation_trait_type>)
                    destroy_allocation = allocation_trait.Tick(delta_time, this, typed_layer_trait); // let the trait decide whether the allocation is to be destroyed
                else
                    destroy_allocation = allocation_trait.Tick(delta_time, this); // let the trait decide whether the allocation is to be destroyed
            }
            else
            {
                destroy_allocation = false; // no Tick(...), no destruction
            }

            // Tick the particles
            if (!destroy_allocation)      
            {
                // has_function_UpdateParticle<allocation_trait_type>::type()); // shuxxx FIXME : this template does not detect template function ! => see ParticleDefault that require a template

                if constexpr (boost::mpl::true_().value)
                {
                    if (particles.size() > 0)
                        destroy_allocation = UpdateParticles(delta_time, typed_layer_trait);
                }
            }

#endif
			if (particles.size() > 0)
				destroy_allocation = UpdateParticles(delta_time, typed_layer_trait);

            return destroy_allocation;
		}

		bool UpdateParticles(float delta_time, layer_trait_type const * layer_trait)
		{
			constexpr int implementation_type = ParticleTraitTools::GetUpdateParticleImplementationType<ALLOCATION_TRAIT>();

			constexpr int trait_implementation		= (implementation_type & UpdateParticleImplementationType::TRAIT_IMPLEMENTATION);
			constexpr int default_implementation	= (implementation_type & UpdateParticleImplementationType::DEFAULT_IMPLEMENTATION);
			constexpr int particle_implementation	= (implementation_type & UpdateParticleImplementationType::PARTICLE_IMPLEMENTATION);
			constexpr int with_begin_call			= (implementation_type & UpdateParticleImplementationType::WITH_BEGIN_CALL);
			constexpr int with_layer_trait			= (implementation_type & UpdateParticleImplementationType::WITH_LAYER_TRAIT);

			size_t particle_count = GetParticleCount();

			size_t remaining_particles = 0;

			ParticleAccessor<particle_type> particle_accessor = GetParticleAccessor();

			if constexpr (trait_implementation != 0)
			{
				if constexpr (with_layer_trait != 0)
				{
					layer_trait_type const* typed_layer_trait = (layer_trait_type const*)layer_trait;

					if constexpr (with_begin_call != 0)
					{
						remaining_particles = DoUpdateParticlesLoop(
							delta_time,
							particle_accessor,
							allocation_trait.BeginUpdateParticles(delta_time, particle_accessor, layer_trait), // do not use a temp variable, so it can be a left-value reference
							layer_trait);
					}
					else
					{
						remaining_particles = DoUpdateParticlesLoop(delta_time, particle_accessor, layer_trait);
					}
				}
				else if constexpr (with_begin_call != 0)
				{
					remaining_particles = DoUpdateParticlesLoop(
						delta_time,
						particle_accessor,
						allocation_trait.BeginUpdateParticles(delta_time, particle_accessor)); // do not use a temp variable, so it can be a left-value reference
				}
				else
				{
					remaining_particles = DoUpdateParticlesLoop(delta_time, particle_accessor);
				}
			}
			else if constexpr (particle_implementation != 0)
			{
				remaining_particles = DoUpdateParticlesLoop(delta_time, particle_accessor);
			}
			else if constexpr (default_implementation != 0)
			{
				remaining_particles = DoUpdateParticlesLoop(delta_time, particle_accessor);
			}

			if (remaining_particles == 0 && GetDestroyWhenEmpty())
				return true; // destroy allocation
			else if (remaining_particles != particle_count) // clean buffer of all particles that have been destroyed
				Resize(remaining_particles);
			return false; // do not destroy allocation
		}

		template<typename ...PARAMS>
		size_t DoUpdateParticlesLoop(float delta_time, ParticleAccessor<particle_type> particle_accessor, PARAMS... params)
		{
			constexpr int implementation_type = ParticleTraitTools::GetUpdateParticleImplementationType<ALLOCATION_TRAIT>();

			constexpr int trait_implementation    = (implementation_type & UpdateParticleImplementationType::TRAIT_IMPLEMENTATION);
			constexpr int default_implementation  = (implementation_type & UpdateParticleImplementationType::DEFAULT_IMPLEMENTATION);
			constexpr int particle_implementation = (implementation_type & UpdateParticleImplementationType::PARTICLE_IMPLEMENTATION);

            size_t particle_count = particle_accessor.GetCount();

			// tick all particles. overide all particles that have been destroyed by next on the array
			size_t j = 0;
			for (size_t i = 0; i < particle_count; ++i)
			{			
				particle_type& particle = particle_accessor[i];

				bool destroy_particle = false;
				if constexpr (trait_implementation != 0)
					destroy_particle = allocation_trait.UpdateParticle(delta_time, particle, params...);
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
        void DoParticlesToPrimitivesLoop(PRIMITIVE_OUTPUT_TYPE& output, PARAMS... params) const
        {
            ParticleConstAccessor<particle_type> particle_accessor = GetParticleAccessor();

			for (particle_type const & particle : particle_accessor)
				allocation_trait.ParticleToPrimitives(particle, output, params...);
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
		/** the trait of the allocation (some extra data + some functions) */
		allocation_trait_type allocation_trait;
	};

	// ==============================================================
	// ParticleLayerBase
	// ==============================================================

	class ParticleSpawner; // required for compilation

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
		bool IsParticleClassCompatible(bool accept_bigger_particle) const
		{
			return ParticleTools::IsParticleClassCompatible<PARTICLE_TYPE>(GetParticleClass(), GetParticleSize(), accept_bigger_particle);
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
		virtual ClassTools::ClassRegistration const * GetParticleClass() const { return nullptr; }

		/** returns true whether the particle type is the one given as template parameter */
		template<typename T>
		bool IsParticleType() const
		{
			return (GetParticleClass() == ClassTools::GetClassRegistration<T>());
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
		ParticleAllocationBase * SpawnParticles(size_t count);

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

		/** get the vertex declaration */
		virtual GPUVertexDeclaration * GetVertexDeclaration() const { return nullptr; }

        /** returns the OpenGL primitive type */
        virtual GLenum GetGLPrimitiveType() const { return GL_NONE; }

	public:

		/** creation of an allocation */
		virtual ParticleAllocationBase * DoSpawnParticles() { return nullptr; }
		/** returns the layer trait */
		virtual void * GetLayerTrait() { return nullptr; }
		/** returns the layer trait */
		virtual void const * GetLayerTrait() const { return nullptr; }

		/** clear all allocations */
		void ClearAllAllocations();	

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

		/** update the vertex declaration */
		void UpdateVertexDeclaration();
		/** the effective rendering */
		int DoDisplayHelper(GPURenderer * renderer, GPURenderMaterial const * final_material, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params);

		/** internal method to update particles (returns true whether there was real changes) */
		virtual bool TickAllocations(float delta_time);

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

	template<typename ALLOCATION_TRAIT>
	class ParticleLayer : public ParticleLayerBase
	{

	public:

		using allocation_trait_type = ALLOCATION_TRAIT;
		using particle_type = typename allocation_trait_type::particle_type;
		using vertex_type = typename allocation_trait_type::vertex_type;
		using layer_trait_type = typename get_LayerTrait<allocation_trait_type>::type;

		/** constructor */
		ParticleLayer(layer_trait_type in_layer_trait = layer_trait_type()):
			layer_trait(in_layer_trait){}

		/** returns the size in memory of a particle */
		virtual size_t GetParticleSize() const override { return sizeof(particle_type); }
        /** override */        
        virtual size_t GetParticleCount() const override
        { 
            size_t result = 0;
            size_t count = particles_allocations.size();
            for (size_t i = 0; i < count; ++i)
            {
                ParticleAllocation<allocation_trait_type> const* allocation = auto_cast(particles_allocations[i].get());
                if (allocation == nullptr)
                    continue;
                result += allocation->particles.size();
            }
            return result;
        }
        /** override */
		virtual size_t GetVertexSize() const override { return sizeof(vertex_type); }
		/** override */
        virtual size_t GetRealVerticesPerParticle() const override
        {
            return chaos::GetRealVerticesPerParticle(ParticleTraitTools::GetPrimitiveType<allocation_trait_type>()); // see PrimitiveOutput.h
        }
		/** override */
		virtual bool AreVerticesDynamic() const override 
		{ 
			// read the layer property 'dynamic_vertices' if any
			if (!ParticleTraitTools::AreVerticesDynamic(layer_trait))
				return false;
			// read a static allocation value
			if (!ParticleTraitTools::AreVerticesDynamicStatic<allocation_trait_type>()) // AreVerticesDynamic() is used to manage all allocations of the whole layer => we cannot afford to read one instance value
				return false;
			return true;
		}
		/** override */
		virtual bool AreParticlesDynamic() const override 
		{ 
			// read the layer property 'dynamic_vertices' if any
			if (!ParticleTraitTools::AreParticlesDynamic(layer_trait))
				return false;
			// read a static allocation value
			if (!ParticleTraitTools::AreParticlesDynamicStatic<allocation_trait_type>())  // AreParticlesDynamic() is used to manage all allocations of the whole layer => we cannot afford to read one instance value
				return false;
			return true;
		}
		/** override */
		virtual ClassTools::ClassRegistration const * GetParticleClass() const override { return ClassTools::GetClassRegistration<particle_type>(); }
		/** override */
		virtual GPUVertexDeclaration * GetVertexDeclaration() const override 
		{
			GPUVertexDeclaration* result = new GPUVertexDeclaration;
			if (result != nullptr)
				GetTypedVertexDeclaration(result, boost::mpl::identity<vertex_type>()); 
			return result;
		}
        /** override */
        virtual GLenum GetGLPrimitiveType() const override { return ParticleTraitTools::GetGLPrimitiveType<allocation_trait_type>(); }

	protected:

		/** override */
		virtual ParticleAllocationBase * DoSpawnParticles() override { return new ParticleAllocation<allocation_trait_type>(this); }
		/** override */
		virtual void * GetLayerTrait() { return &layer_trait; }
		/** override */
		virtual void const * GetLayerTrait() const { return &layer_trait; }

        /** override */
        virtual void DoUpdateGPUBuffers(GPURenderer* renderer, size_t vertex_requirement_evaluation) override
        {
            // some layers are in a manager, some not (see TiledMap)
            GPUBufferCache* cache = (particle_manager == nullptr) ? &buffer_cache : &particle_manager->GetBufferCache();

			constexpr PrimitiveType primitive_type = ParticleTraitTools::GetPrimitiveType<ALLOCATION_TRAIT>();

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
                ParticleAllocation<allocation_trait_type> * allocation = auto_cast(particles_allocations[i].get());
                if (!allocation->IsVisible())
                    continue;
                // transform particles into vertices
                allocation->ParticlesToPrimitives(output, GetLayerTrait());
            }
            output.Flush();
        }

	protected:

		/** the trait of the layer */
		layer_trait_type layer_trait;
	};

	// undefine macros
	//#undef CHAOS_PARTICLE_CLASSES
	//#undef CHAOS_PARTICLE_FORWARD_DECL
	//#undef CHAOS_PARTICLE_FRIEND_DECL
	//#undef CHAOS_PARTICLE_ALL_FRIENDS

}; // namespace chaos

