#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ParticleManager.h>
#include <chaos/ParticleTools.h>
#include <chaos/GeometryFramework.h>
#include <chaos/GPUProgram.h>

namespace chaos
{
	namespace ParticleDefault
	{
		/** Particle : a default particle, with simpler data */
		class Particle
		{
		public:

			chaos::box2 bounding_box;
			chaos::ParticleTexcoords texcoords;
			glm::vec4 color;
		};

		/** Vertex : vertex for default particle */
		class Vertex
		{
		public:

			glm::vec2 position;
			glm::vec3 texcoord;
			glm::vec4 color;
		};

		/** ParticleVertexDefault : vertex for default particle*/
		class ParticleTrait : public ParticleLayerTrait<Particle, Vertex>
		{
		public:

			size_t ParticleToVertices(Particle const * particle, Vertex * vertices, size_t vertices_per_particle, chaos::ParticleAllocation * allocation) const;
		};

		/** the default vertex declaration */
		chaos::GPUVertexDeclaration GetTypedVertexDeclaration(boost::mpl::identity<Vertex>);

		/** generate a default shader */
		GPUProgram * GenDefaultParticleProgram();
		/** generate a default material */
		GPURenderMaterial * GenDefaultParticleMaterial();

	}; // namespace ParticleDefault

}; // namespace chaos

