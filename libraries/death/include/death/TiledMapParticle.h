#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ParticleManager.h>
#include <chaos/ParticleDefault.h>
#include <chaos/EmptyClass.h>
#include <chaos/PrimitiveOutput.h>

namespace death
{

	// =====================================
	// TiledMapParticle
	// =====================================

	class TiledMapParticle : public chaos::ParticleDefault
	{
	public:

		int gid = 0;
		chaos::BitmapAtlas::BitmapInfo const* bitmap_info = nullptr; // shu46 !!! 
	};


	CHAOS_REGISTER_CLASS2(TiledMapParticle, chaos::ParticleDefault);

	// =====================================
	// TiledMapParticleTrait
	// =====================================

	class TiledMapParticleTrait : public chaos::ParticleAllocationTrait<TiledMapParticle, chaos::VertexDefault, false, false> // shuxxx set to false = optimization 1
	{
	public:


	};

}; // namespace death
