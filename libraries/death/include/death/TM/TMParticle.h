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

	class TMParticleLayerTrait : public chaos::ParticleLayerTrait<TMParticle, chaos::VertexDefault, GameEntity const *>
	{
	public:

#if 0

		void ParticleToPrimitives(TMParticle const& p, chaos::QuadOutput<chaos::VertexDefault>& output) const
		{

	
		}

		void ParticleToPrimitives(TMParticle const& p, chaos::TrianglePairOutput<chaos::VertexDefault>& output) const
		{
			auto primitive = output.AddPrimitive();
			chaos::ParticleToPrimitive(p, primitive);
			if ((p.flags & chaos::TiledMap::TileParticleFlags::NEIGHBOUR_LEFT) != 0)
				for (auto & v : primitive)
					v.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		}

	public:

		//template<typename T>
		glm::mat4 BeginParticlesToPrimitives(chaos::ParticleConstAccessor<TMParticle>& accessor, GameEntity const * allocation_data) const
		{
			if (allocation_data == nullptr)
				return glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
			return {};
		}

		//template<typename T>
		void ParticleToPrimitives(TMParticle const & particle, chaos::QuadOutput<chaos::VertexDefault>& output, glm::mat4 const& m, GameEntity const * allocation_trait) const
		{
			chaos::QuadPrimitive<chaos::VertexDefault> primitive = output.AddPrimitive();
			chaos::ParticleToPrimitive(particle, primitive);

			for (auto& v : primitive)
				v.color.r = 0.0f;

		}
#endif
	};

}; // namespace death
