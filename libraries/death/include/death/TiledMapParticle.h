#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ParticleManager.h>
#include <chaos/ParticleDefault.h>

namespace death
{

	namespace TiledMap
	{

		// =====================================
		// TileParticle
		// =====================================

		class TileParticle : public chaos::ParticleDefault::Particle
		{
		public:

			int gid = 0;
		};

		class TileParticleTrait : public chaos::ParticleLayerTrait<TileParticle, chaos::ParticleDefault::Vertex>
		{
		public:

			static size_t ParticleToVertices(TileParticle const * particle, chaos::ParticleDefault::Vertex * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation)
			{
				return chaos::ParticleDefault::ParticleTrait::ParticleToVertices(particle, vertices, vertices_per_particle, allocation);
			}
		};

	}; // namespace TiledMap

}; // namespace death
