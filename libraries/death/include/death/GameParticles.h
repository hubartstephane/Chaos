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

	class ParticleBackgroundLayerTrait : public chaos::ParticleLayerTrait<ParticleBackground, chaos::VertexDefault>
	{
	public:

		/** constructor */
		ParticleBackgroundLayerTrait(){ dynamic_particles = dynamic_vertices = false;}
		/** copy constructor */
		ParticleBackgroundLayerTrait(ParticleBackgroundLayerTrait const& src) = default;

		/** output primitive */
        void ParticleToPrimitives(ParticleBackground const& particle, chaos::QuadOutput<chaos::VertexDefault> & output) const;
		/** output primitive */
        void ParticleToPrimitives(ParticleBackground const& particle, chaos::TrianglePairOutput<chaos::VertexDefault>& output) const;
	};

	CHAOS_REGISTER_CLASS1(ParticleBackground);

}; //namespace death

