#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ParticleManager.h>
#include <chaos/ParticleDefault.h>
#include <chaos/EmptyClass.h>

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

		class TileParticleTrait : public chaos::ParticleLayerTrait<TileParticle, chaos::ParticleDefault::Vertex, chaos::EmptyClass, false, false> // shuxxx set to false = optimization 1
		{
		public:

			static size_t ParticleToVertices(TileParticle const * particle, chaos::ParticleDefault::Vertex * vertices, size_t vertices_per_particle)
			{
				return chaos::ParticleDefault::ParticleTrait::ParticleToVertices(particle, vertices, vertices_per_particle);
			}
		};

	}; // namespace TiledMap

}; // namespace death
