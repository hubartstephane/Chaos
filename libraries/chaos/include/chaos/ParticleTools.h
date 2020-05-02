#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/GeometryFramework.h>
#include <chaos/ClassTools.h>
#include <chaos/PrimitiveOutput.h>
#include <chaos/ParticleDefault.h>
#include <chaos/Hotpoint.h>

namespace chaos
{


	/**
	* ParticleTools : an utility class that deserve to bring some services for particles system
	*/

	namespace ParticleTools
	{
        /** get particle by index */
        ParticleDefault* GetParticle(ParticleAllocationBase* allocation, size_t index);
        ParticleDefault const* GetParticle(ParticleAllocationBase const* allocation, size_t index);
        /** get particle position */
        glm::vec2 GetParticlePosition(ParticleAllocationBase const* allocation, size_t index);
        /** get particle box */
        box2 GetParticleBox(ParticleAllocationBase const* allocation, size_t index);
        /** set the particle position */
        bool SetParticlePosition(ParticleAllocationBase* allocation, size_t index, glm::vec2 const& position);
        /** set the particle box */
        bool SetParticleBox(ParticleAllocationBase* allocation, size_t index, box2 const& box);

		/** conversion methods */
		box2 ParticleCornersToBox(ParticleCorners const & corners);
		/** conversion methods */
		ParticleCorners BoxToParticleCorners(box2 const & box);
		/** returns the 2 corners of a particle according to a given position, a size and an hotpoint */
		ParticleCorners GetParticleCorners(glm::vec2 const & position, glm::vec2 const & size, Hotpoint hotpoint);
		/** returns the texture coordinates of a particle */
		ParticleTexcoords GetParticleTexcoords(BitmapAtlas::BitmapLayout const & layout);

		/** considere the incoming texcoords represent a grid atlas, returns the sub image corresponding to given image */
		ParticleTexcoords MakeParticleTexcoordsAtlas(ParticleTexcoords texcoords, glm::ivec2 const & atlas_dimension, int skip_last, int image_id);
		/** considere the incoming texcoords represent a grid atlas, returns the sub image corresponding to given image */
		ParticleTexcoords MakeParticleTexcoordsAtlas(ParticleTexcoords texcoords, glm::ivec2 const & atlas_dimension, glm::ivec2 const & image_id);

        /** fill the vertices array with 4 vertices corresponding to 1 QUAD */
        void GenerateBoxParticle(QuadPrimitive<VertexDefault>& primitive, ParticleCorners const& corners, ParticleTexcoords const& texcoords, int flags = 0, float rotation = 0.0f);

        void GenerateBoxParticle(QuadPrimitive<VertexDefault>& primitive, box2 const& box, ParticleTexcoords const& texcoords, int flags = 0, float rotation = 0.0f);

        /** fill the vertices array with 6 vertices corresponding to 1 TRIANGLE PAIR */
        void GenerateBoxParticle(TrianglePairPrimitive<VertexDefault>& primitive, ParticleCorners const& corners, ParticleTexcoords const& texcoords, int flags = 0, float rotation = 0.0f);

        void GenerateBoxParticle(TrianglePairPrimitive<VertexDefault>& primitive, box2 const& box, ParticleTexcoords const& texcoords, int flags = 0, float rotation = 0.0f);

		/** an utility method to generate the 4 vertex attributes for a Particle */
		void GenerateBoxParticleAttributes(glm::vec2* vertex_positions, glm::vec3* vertex_texcoords, ParticleCorners const& corners, ParticleTexcoords const& texcoords, int flags, float rotation);

		template<typename PARTICLE_TYPE>
		bool IsParticleClassCompatible(ClassTools::ClassRegistration const * particle_class, size_t particle_size, bool accept_bigger_particle)
		{
			ClassTools::ClassRegistration const * wanted_class = ClassTools::GetClassRegistration<PARTICLE_TYPE>();

			// strict equality
			if (particle_class == wanted_class)
				return true;
			// smaller size => failure
			if (particle_size < sizeof(PARTICLE_TYPE))
				return false;
			// bigger size => success only if accepted
			if (particle_size > sizeof(PARTICLE_TYPE) && !accept_bigger_particle)
				return false;
			// ensure we have not declared class as incompatible
			if (ClassTools::InheritsFrom(particle_class, wanted_class) == InheritanceType::NO)
				return false;
			// success
			return true;
		}

	}; // namespace ParticleTools

}; // namespace chaos

