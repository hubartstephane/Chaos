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

		int gid = 0;
		chaos::BitmapAtlas::BitmapInfo const* bitmap_info = nullptr; // shu46 !!! 
	};


	CHAOS_REGISTER_CLASS2(TMParticle, chaos::ParticleDefault);

	// =====================================
	// TMParticleTrait
	// =====================================

	class TMParticleTrait : public chaos::ParticleAllocationTrait<TMParticle, chaos::VertexDefault, false, false> // shuxxx set to false = optimization 1
	{
	};

}; // namespace death
