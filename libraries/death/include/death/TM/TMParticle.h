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

		// the ID for the tile
		int gid = 0;
		chaos::BitmapAtlas::BitmapInfo const* bitmap_info = nullptr; 
	};


	CHAOS_REGISTER_CLASS2(TMParticle, chaos::ParticleDefault);

	// =====================================
	// TMParticleLayerTrait
	// =====================================

	class TMParticleLayerTrait : public chaos::ParticleLayerTrait<TMParticle, chaos::VertexDefault>
	{
	public:

		TMParticleLayerTrait()
		{
			dynamic_particles = false;
			dynamic_vertices = false;
		}
	};

}; // namespace death
