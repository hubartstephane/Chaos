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

#ifdef CHAOS_FORWARD_DECLARATION

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

// friendship macro
#define CHAOS_PARTICLE_FRIEND_DECL(r, data, elem) friend class elem;
#define CHAOS_PARTICLE_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(CHAOS_PARTICLE_FRIEND_DECL, _, CHAOS_PARTICLE_CLASSES)

namespace chaos
{
	// detect whether class have a nested class
	CHAOS_GENERATE_HAS_TRAIT(AllocationTrait);

	BOOST_PP_SEQ_FOR_EACH(CHAOS_PARTICLE_FORWARD_DECL, _, CHAOS_PARTICLE_CLASSES);

	template<typename LAYER_TRAIT>
	class ParticleLayer;

	template<typename LAYER_TRAIT>
	class ParticleAllocation;

}; // namespace chaos

#endif

#include <chaos/Particle/ParticleLayerTrait.h>
#include <chaos/Particle/ParticleDefault.h>
#include <chaos/Particle/ParticleAccessor.h>
#include <chaos/Particle/ParticleTraitTools.h>
#include <chaos/Particle/ParticleAllocation.h>
#include <chaos/Particle/ParticleLayer.h>
#include <chaos/Particle/ParticleManager.h>
#include <chaos/Particle/ParticleSpawner.h>
#include <chaos/Particle/ParticleTextGenerator.h>
#include <chaos/Particle/ParticleTools.h>
#include <chaos/Particle/SpawnParticleResult.h>