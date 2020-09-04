#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	// ==============================================================
	// ParticleLayerTrait
	// ==============================================================

	template<typename PARTICLE_TYPE, typename VERTEX_TYPE, bool DYNAMIC_PARTICLES = true, bool DYNAMIC_VERTICES = true>
	class ParticleLayerTrait
	{
	public:

		/** the type for one particle */
		using particle_type = PARTICLE_TYPE;
		/** the type for one vertex */
		using vertex_type = VERTEX_TYPE;

		/** whether the particles are dynamic */
		bool dynamic_particles = DYNAMIC_PARTICLES;
		/** whether the vertices are dynamic */
		bool dynamic_vertices = DYNAMIC_VERTICES;
	};

}; // namespace chaos

