#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/ParticleManager.h>
#include <chaos/ParticleDefault.h>
#include <chaos/Class.h>


namespace death
{

	// ===========================================================================
	// Background particle system
	// ===========================================================================

	class ParticleBackground
	{
	public:

		glm::vec4 color;
	};

	class ParticleBackgroundTrait : public chaos::ParticleAllocationTrait<ParticleBackground, chaos::VertexDefault>
	{
	public:

        void ParticleToPrimitives(ParticleBackground const& particle, chaos::QuadOutput<chaos::VertexDefault> & output) const;

        void ParticleToPrimitives(ParticleBackground const& particle, chaos::TrianglePairOutput<chaos::VertexDefault>& output) const;
	};

	CHAOS_REGISTER_CLASS1(ParticleBackground);

}; //namespace death

