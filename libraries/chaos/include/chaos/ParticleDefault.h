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


		/** get particle by index */
		chaos::ParticleDefault::Particle * GetParticle(chaos::ParticleAllocationBase * allocation, size_t index);
		chaos::ParticleDefault::Particle const * GetParticle(chaos::ParticleAllocationBase const * allocation, size_t index);
		/** get particle position */
		glm::vec2 GetParticlePosition(chaos::ParticleAllocationBase const * allocation, size_t index);
		/** get particle box */
		chaos::box2 GetParticleBox(chaos::ParticleAllocationBase const * allocation, size_t index);
		/** set the particle position */
		bool SetParticlePosition(chaos::ParticleAllocationBase * allocation, size_t index, glm::vec2 const & position);
		/** set the particle box */
		bool SetParticleBox(chaos::ParticleAllocationBase * allocation, size_t index, chaos::box2 const & box);

		/** ParticleVertexDefault : vertex for default particle */
		class ParticleTrait : public ParticleAllocationTrait<Particle, Vertex>
		{
		public:

			template<typename VERTEX_TYPE>
			static size_t ParticleToVertices(Particle const * particle, VERTEX_TYPE * vertices, size_t vertices_per_particle)
			{
				// generate particle corners and texcoords
				chaos::ParticleTools::GenerateBoxParticle(particle->bounding_box, particle->texcoords, vertices);
				// copy the color in all triangles vertex
				for (size_t i = 0; i < 6; ++i)
					vertices[i].color = particle->color;

				return vertices_per_particle;
			}
		};

		/** the default vertex declaration */
		chaos::GPUVertexDeclaration GetTypedVertexDeclaration(boost::mpl::identity<Vertex>);

		/** generate a default shader */
		GPUProgram * GenDefaultParticleProgram();
		/** generate a default material */
		GPURenderMaterial * GenDefaultParticleMaterial();

	}; // namespace ParticleDefault

}; // namespace chaos

