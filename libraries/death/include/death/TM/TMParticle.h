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
#if 0	
	public:

		template<typename T>
		glm::mat4 BeginParticlesToPrimitives(chaos::ParticleConstAccessor<TMParticle>& accessor, T allocation_data) const
		{
			if (allocation_data == nullptr)
				return glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
			return {};
		}

		template<typename T>
		void ParticleToPrimitives(TMParticle const& particle, chaos::QuadOutput<chaos::VertexDefault>& output, glm::mat4 const& m, T allocation_trait) const
		{
			int i = 0;
			++i;
		}


	public:




		void ParticleToPrimitives(TMParticle const& particle, chaos::QuadOutput<chaos::VertexDefault>& output, glm::mat4 const& m, TMObject* const* allocation_trait) const
		{
			int i = 0;
			++i;
		}

	
		glm::mat4 BeginParticlesToPrimitives(chaos::ParticleConstAccessor<TMParticle>& accessor, TMObject * const * allocation_trait) const
		{
			return {};
		}

#endif
	};

}; // namespace death
