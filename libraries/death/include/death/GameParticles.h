#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/ParticleManager.h>
#include <chaos/ParticleDefault.h>


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

	class ParticleBackgroundTrait : public chaos::ParticleAllocationTrait<ParticleBackground, chaos::ParticleDefault::Vertex>
	{
	public:

        void ParticleToVertices(ParticleBackground const* particle, chaos::VertexOutput<chaos::ParticleDefault::Vertex> & vertices) const;

		size_t ParticleToVertices(ParticleBackground const * particle, chaos::ParticleDefault::Vertex * vertices, size_t vertices_per_particle) const;
	};


}; //namespace death

