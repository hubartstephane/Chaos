#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/GPUClasses.h>
#include <chaos/PrimitiveOutput.h>
#include <chaos/GeometryFramework.h>
#include <chaos/ParticleAllocationTrait.h>


namespace chaos
{
	// XXX : given a rectangle particle and a 4 textures coordinates, there are 8 possibilities to make them match (without twisting the geometry itself)
	//
	//       -4 rotations
	//       -1 optional symetry
	//
	//       these is given with the 3 bitfields

	namespace ParticleDefaultFlags
	{
		static constexpr int TEXTURE_HORIZONTAL_FLIP = (1 << 1);
		static constexpr int TEXTURE_VERTICAL_FLIP   = (1 << 2);
		static constexpr int TEXTURE_DIAGONAL_FLIP   = (1 << 3);
	};

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

		int bitmap_index = 0;
    };

	/** Particle : a default particle, with simpler data */
	class ParticleDefault
	{
	public:

		/** the bounding box of the particle */
		chaos::box2 bounding_box;
		/** the four texcoords to apply to the particle vertices */
		chaos::ParticleTexcoords texcoords;
		/** the color of the particle */
		glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		/** some flags (more or less custom) */
		int flags = 0;
	};

	CHAOS_REGISTER_CLASS1(ParticleDefault);

	/** Vertex : vertex for default particle */
	class VertexDefault
	{
	public:

		/** the position of the vertex */
		glm::vec2 position;
		/** the texcoord of the vertex */
		glm::vec3 texcoord;
		/** the color of the vertex */
		glm::vec4 color;
	};

	/** ParticleTrait : the default trait */
	using ParticleDefaultTrait = ParticleAllocationTrait<ParticleDefault, VertexDefault>;

	/** generates 1 quad from one particle */
	void ParticleToPrimitives(ParticleDefault const& particle, QuadOutput<VertexDefault>& output);
	/** generates 1 triangle pair from one particle */
	void ParticleToPrimitives(ParticleDefault const& particle, TrianglePairOutput<VertexDefault>& output);

	/** generates 1 quad from one particle */
	void ParticleToPrimitive(ParticleDefault const& particle, QuadPrimitive<VertexDefault>& primitive);
	/** generates 1 triangle pair from one particle */
	void ParticleToPrimitive(ParticleDefault const& particle, TrianglePairPrimitive<VertexDefault>& primitive);

	/** the default vertex declaration */
	void GetTypedVertexDeclaration(GPUVertexDeclaration* result, boost::mpl::identity<VertexDefault>);
	/** generate a default shader */
	GPUProgram* GenDefaultParticleProgram();
	/** generate a default material */
	GPURenderMaterial* GenDefaultParticleMaterial();

}; // namespace chaos

