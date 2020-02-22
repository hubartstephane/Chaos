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
        ParticleDefault::Particle* GetParticle(ParticleAllocationBase* allocation, size_t index);
        ParticleDefault::Particle const* GetParticle(ParticleAllocationBase const* allocation, size_t index);
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
		ParticleCorners GetParticleCorners(glm::vec2 const & position, glm::vec2 const & size, HotpointType hotpoint_type);
		/** returns the texture coordinates of a particle */
		ParticleTexcoords GetParticleTexcoords(BitmapAtlas::BitmapLayout const & layout);

		/** considere the incoming texcoords represent a grid atlas, returns the sub image corresponding to given image */
		ParticleTexcoords MakeParticleTexcoordsAtlas(ParticleTexcoords texcoords, glm::ivec2 const & atlas_dimension, int skip_last, int image_id);
		/** considere the incoming texcoords represent a grid atlas, returns the sub image corresponding to given image */
		ParticleTexcoords MakeParticleTexcoordsAtlas(ParticleTexcoords texcoords, glm::ivec2 const & atlas_dimension, glm::ivec2 const & image_id);

		/** fill the vertices array with 6 vertices corresponding to 3 triangles */
		template<typename VERTEX_TYPE>
		void GenerateBoxParticle(ParticleCorners const & corners, ParticleTexcoords const & texcoords, VERTEX_TYPE * vertices, float rotation = 0.0f)
		{
			VERTEX_TYPE bl;
			bl.position.x = corners.bottomleft.x;
			bl.position.y = corners.bottomleft.y;
			bl.texcoord.x = texcoords.bottomleft.x;
			bl.texcoord.y = texcoords.bottomleft.y;
			bl.texcoord.z = texcoords.bitmap_index;

			VERTEX_TYPE tr;
			tr.position.x = corners.topright.x;
			tr.position.y = corners.topright.y;
			tr.texcoord.x = texcoords.topright.x;
			tr.texcoord.y = texcoords.topright.y;
			tr.texcoord.z = texcoords.bitmap_index;

			VERTEX_TYPE tl;
			tl.position.x = corners.bottomleft.x;
			tl.position.y = corners.topright.y;
			tl.texcoord.x = texcoords.bottomleft.x;
			tl.texcoord.y = texcoords.topright.y;
			tl.texcoord.z = texcoords.bitmap_index;

			VERTEX_TYPE br;
			br.position.x = corners.topright.x;
			br.position.y = corners.bottomleft.y;
			br.texcoord.x = texcoords.topright.x;
			br.texcoord.y = texcoords.bottomleft.y;
			br.texcoord.z = texcoords.bitmap_index;

			if (rotation != 0.0f)
			{
				glm::vec2 center_position = (corners.bottomleft + corners.topright) * 0.5f;

				float c = std::cos(rotation);
				float s = std::sin(rotation);

				bl.position = GLMTools::Rotate(bl.position - center_position, c, s) + center_position;
				br.position = GLMTools::Rotate(br.position - center_position, c, s) + center_position;
				tr.position = GLMTools::Rotate(tr.position - center_position, c, s) + center_position;
				tl.position = GLMTools::Rotate(tl.position - center_position, c, s) + center_position;
			}

			vertices[0] = bl;
			vertices[1] = br;
			vertices[2] = tr;

			vertices[3] = bl;
			vertices[4] = tr;
			vertices[5] = tl;
		}

		template<typename VERTEX_TYPE>
		void GenerateBoxParticle(box2 const & box, ParticleTexcoords const & texcoords, VERTEX_TYPE * vertices, float rotation = 0.0f)
		{
			std::pair<glm::vec2, glm::vec2> corners = GetBoxExtremums(box);

			ParticleCorners particle_corners;
			particle_corners.bottomleft = corners.first;
			particle_corners.topright   = corners.second;
			GenerateBoxParticle(particle_corners, texcoords, vertices, rotation);
		}

        /** fill the vertices array with 4 vertices corresponding to 1 QUAD */
        void GenerateBoxParticle(ParticleCorners const& corners, ParticleTexcoords const& texcoords, QuadPrimitive<ParticleDefault::Vertex>& primitive, float rotation = 0.0f);

        void GenerateBoxParticle(box2 const& box, ParticleTexcoords const& texcoords, QuadPrimitive<ParticleDefault::Vertex>& primitive, float rotation = 0.0f);

        /** fill the vertices array with 6 vertices corresponding to 1 TRIANGLE PAIR */
        void GenerateBoxParticle(ParticleCorners const& corners, ParticleTexcoords const& texcoords, TrianglePairPrimitive<ParticleDefault::Vertex>& primitive, float rotation = 0.0f);

        void GenerateBoxParticle(box2 const& box, ParticleTexcoords const& texcoords, TrianglePairPrimitive<ParticleDefault::Vertex>& primitive, float rotation = 0.0f);



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
			if (ClassTools::InheritsFrom(particle_class, wanted_class) == InheritanceType::INHERITANCE_NO)
				return false;
			// success
			return true;
		}

		/** apply some symetries to ParticleTexcoords or ParticleCorners */
		template<typename T>
		T ApplySymetriesToTexcoords(T const & src, bool horizontal_flip, bool vertical_flip)
		{
			T result = src;
			if (horizontal_flip)
				std::swap(result.bottomleft.x, result.topright.x);
			if (vertical_flip)
				std::swap(result.bottomleft.y, result.topright.y);
			return result;
		}

	}; // namespace ParticleTools

}; // namespace chaos

