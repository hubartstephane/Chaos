#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/GeometryFramework.h>

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

	/**
	 * ParticleTools : an utility class that deserve to bring some services for particles system
	 */

	namespace ParticleTools
	{
		/** conversion methods */
		box2 ParticleCornersToBox(ParticleCorners const & corners);
		/** conversion methods */
		ParticleCorners BoxToParticleCorners(box2 const & box);
		/** returns the 2 corners of a particle according to a given position, a size and an hotpoint */
		ParticleCorners GetParticleCorners(glm::vec2 const & position, glm::vec2 const & size, int hotpoint_type);
		/** returns the texture coordinates of a particle */
		ParticleTexcoords GetParticleTexcoords(BitmapAtlas::BitmapEntry const & entry, glm::vec2 const & atlas_size);

		/** considere the incoming texcoords represent a grid atlas, returns the sub image corresponding to given image */
		ParticleTexcoords MakeParticleTexcoordsAtlas(ParticleTexcoords texcoords, glm::ivec2 const & atlas_dimension, int skip_last, int image_id);
		/** considere the incoming texcoords represent a grid atlas, returns the sub image corresponding to given image */
		ParticleTexcoords MakeParticleTexcoordsAtlas(ParticleTexcoords texcoords, glm::ivec2 const & atlas_dimension, glm::ivec2 const & image_id);

		/** fill the vertices array with 6 vertices corresponding to 3 triangles */
		template<class VERTEX_TYPE>
		void GenerateBoxParticle(ParticleCorners const & corners, ParticleTexcoords const & texcoords, VERTEX_TYPE * vertices)
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

			vertices[0] = bl;
			vertices[1] = br;
			vertices[2] = tr;

			vertices[3] = bl;
			vertices[4] = tr;
			vertices[5] = tl;
		}

		template<class VERTEX_TYPE>
		void GenerateBoxParticle(chaos::box2 const & box, ParticleTexcoords const & texcoords, VERTEX_TYPE * vertices)
		{
			std::pair<glm::vec2, glm::vec2> corners = box.GetCorners();

			glm::vec2 const & bottomleft = corners.first;
			glm::vec2 const & topright = corners.second;

			VERTEX_TYPE bl;
			bl.position.x = bottomleft.x;
			bl.position.y = bottomleft.y;
			bl.texcoord.x = texcoords.bottomleft.x;
			bl.texcoord.y = texcoords.bottomleft.y;
			bl.texcoord.z = texcoords.bitmap_index;

			VERTEX_TYPE tr;
			tr.position.x = topright.x;
			tr.position.y = topright.y;
			tr.texcoord.x = texcoords.topright.x;
			tr.texcoord.y = texcoords.topright.y;
			tr.texcoord.z = texcoords.bitmap_index;

			VERTEX_TYPE tl;
			tl.position.x = bottomleft.x;
			tl.position.y = topright.y;
			tl.texcoord.x = texcoords.bottomleft.x;
			tl.texcoord.y = texcoords.topright.y;
			tl.texcoord.z = texcoords.bitmap_index;

			VERTEX_TYPE br;
			br.position.x = topright.x;
			br.position.y = bottomleft.y;
			br.texcoord.x = texcoords.topright.x;
			br.texcoord.y = texcoords.bottomleft.y;
			br.texcoord.z = texcoords.bitmap_index;

			vertices[0] = bl;
			vertices[1] = br;
			vertices[2] = tr;

			vertices[3] = bl;
			vertices[4] = tr;
			vertices[5] = tl;
		}

	}; // namespace ParticleTools

}; // namespace chaos

