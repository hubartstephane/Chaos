#pragma once

#include <death/TM.h>

namespace death
{

	// =====================================
	// TMParticle
	// =====================================

	class TMParticle : public chaos::ParticleDefault
	{
	public:

		// the ID for the "tile" (0 if the particle is not a tile)
		int gid = 0;
		// the bitmap for the particle
		chaos::BitmapAtlas::BitmapInfo const* bitmap_info = nullptr; 
	};

	CHAOS_REGISTER_CLASS2(TMParticle, chaos::ParticleDefault);

	// =====================================
	// TMParticleLayerTrait
	// =====================================

	class TMParticleLayerTrait : public chaos::ParticleLayerTrait<TMParticle, chaos::VertexDefault, TMObject *>
	{
	public:

		glm::mat4 BeginParticlesToPrimitives(chaos::ParticleConstAccessor<TMParticle>& accessor, TMObject * const * allocation_trait) const
		{
			return {};
		}

		void ParticleToPrimitives(TMParticle const& particle, chaos::QuadOutput<chaos::VertexDefault>& output, glm::mat4 const &  m, TMObject * const * allocation_trait) const
		{
			int i = 0;
			++i;
		}
	};

}; // namespace death
