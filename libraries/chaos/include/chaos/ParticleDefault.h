#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/GPUClasses.h>
#include <chaos/PrimitiveOutput.h>
#include <chaos/GeometryFramework.h>
#include <chaos/ParticleAllocationTrait.h>

namespace chaos
{

    /**
    * ParticleCorners : represents 2 corners of a particle
    */

    class ParticleCorners
    {
    public:

        glm::vec2 bottomleft;
        glm::vec2 topright;
    };

    /**
    * ParticleTexcoords : an object usefull for getting the texture coordinates of a sprite
    */

    class ParticleTexcoords : public ParticleCorners
    {
    public:

        float bitmap_index;
    };

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

		/** ParticleVertexDefault : vertex for default particle */
		class ParticleTrait : public ParticleAllocationTrait<Particle, Vertex>
		{
		public:

            /** generates 1 quad from one particle */
            static void ParticleToVertices(Particle const& particle, QuadOutput<Vertex> & output);
            /** generates 1 triangle pair from one particle */
            static void ParticleToVertices(Particle const& particle, TrianglePairOutput<Vertex>& output);

            /** generates 1 quad from one particle */
            static void ParticleToPrimitive(Particle const& particle, QuadPrimitive<Vertex>& primitive);
            /** generates 1 triangle pair from one particle */
            static void ParticleToPrimitive(Particle const& particle, TrianglePairPrimitive<Vertex>& primitive);
		};

		/** the default vertex declaration */
		void GetTypedVertexDeclaration(GPUVertexDeclaration * result, boost::mpl::identity<Vertex>);
		/** generate a default shader */
		GPUProgram * GenDefaultParticleProgram();
		/** generate a default material */
		GPURenderMaterial * GenDefaultParticleMaterial();

	}; // namespace ParticleDefault

}; // namespace chaos

