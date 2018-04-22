#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/VertexDeclaration.h>


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

	class ParticleTexcoords
	{
	public:
		glm::vec2 bottomleft;
		glm::vec2 topright;
		float     bitmap_index;
	};

	/**
	 * ParticleTools : an utility class that deserve to bring some services for particles system
	 */

	namespace ParticleTools
	{
		/** returns the 2 corners of a particle according to a given position, a size and an hotpoint */
		ParticleCorners GetParticleCorners(glm::vec2 const & position, glm::vec2 const & size, int hotpoint_type);

		/** returns the texture coordinates of a particle */
		ParticleTexcoords GetParticleTexcoords(BitmapAtlas::BitmapEntry const & entry, glm::vec2 const & atlas_size);

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

	}; // namespace ParticleTools

}; // namespace chaos

