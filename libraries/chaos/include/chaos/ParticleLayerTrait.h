#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	// ==============================================================
	// ParticleLayerTraitBase
	// ==============================================================

	class ParticleLayerTraitBase
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

	template<typename PARTICLE_TYPE, typename VERTEX_TYPE>
	class ParticleLayerTrait : public ParticleLayerTraitBase
	{
	public:

		/** the type for one particle */
		using particle_type = PARTICLE_TYPE;
		/** the type for one vertex */
		using vertex_type = VERTEX_TYPE;
	};

}; // namespace chaos

