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

	class TiledMapParticle : public chaos::ParticleDefault::Particle
	{
	public:

		int gid = 0;
		chaos::BitmapAtlas::BitmapInfo const* bitmap_info = nullptr; // shu46 !!! 


		bool horizontal_flip = false;
		bool vertical_flip = false;
		bool diagonal_flip = false;

	};

	// =====================================
	// TiledMapParticleTrait
	// =====================================

	class TiledMapParticleTrait : public chaos::ParticleAllocationTrait<TiledMapParticle, chaos::ParticleDefault::Vertex, false, false> // shuxxx set to false = optimization 1
	{
	public:


	};

}; // namespace death
