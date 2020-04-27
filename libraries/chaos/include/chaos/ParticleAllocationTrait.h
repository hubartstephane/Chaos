#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	// ==============================================================
	// ParticleAllocationTrait
	// ==============================================================

	template<typename PARTICLE_TYPE, typename VERTEX_TYPE, bool DYNAMIC_PARTICLES = true, bool DYNAMIC_VERTICES = true>
	class ParticleAllocationTrait
	{
	public:

		/** the type for one particle */
		using particle_type = PARTICLE_TYPE;
		/** the type for one vertex */
		using vertex_type = VERTEX_TYPE;

		/** whether the particles are dynamic */
		static bool const dynamic_particles = DYNAMIC_PARTICLES;
		/** whether the vertices are dynamic */
		static bool const dynamic_vertices = DYNAMIC_VERTICES;
	};

	// ==============================================================
	// ParticleAllocationTrait
	// ==============================================================




}; // namespace chaos

