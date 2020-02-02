#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ParticleManager.h>
#include <chaos/ParticleDefault.h>
#include <chaos/EmptyClass.h>
#include <chaos/PrimitiveOutput.h>

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
			chaos::BitmapAtlas::BitmapInfo const * bitmap_info = nullptr;
		};

        // =====================================
        // TileParticleTrait
        // =====================================

		class TileParticleTrait : public chaos::ParticleAllocationTrait<TileParticle, chaos::ParticleDefault::Vertex, false, false> // shuxxx set to false = optimization 1
		{
		public:

            static void ParticleToPrimitives(TileParticle const& particle, chaos::QuadOutput<chaos::ParticleDefault::Vertex> & output)
            {
                chaos::ParticleDefault::ParticleTrait::ParticleToPrimitives(particle, output);
            }

            static void ParticleToPrimitives(TileParticle const& particle, chaos::TrianglePairOutput<chaos::ParticleDefault::Vertex>& output)
            {
                chaos::ParticleDefault::ParticleTrait::ParticleToPrimitives(particle, output);
            }
		};

	}; // namespace TiledMap

}; // namespace death
