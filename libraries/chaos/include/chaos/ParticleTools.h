#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/BitmapAtlas.h>

namespace chaos
{
	/**
	* ParticleCorners : represents 2 corners of a particle
	*/

	class ParticleCorners
	{
	public:

		glm::vec2 bottomleft_position;
		glm::vec2 topright_position;
	};

	/**
	 * ParticleTools : an utility class that deserve to bring some services for particles system
	 */

	class ParticleTools
	{
	public:

		/** returns the 2 corners of a particle according to a given position, a size and an hotpoint */
		static ParticleCorners GetParticleCorners(glm::vec2 const & position, glm::vec2 const & size, int hotpoint_type);

		/** fill the vertices array with 6 vertices corresponding to 3 triangles */
		template<class VERTEX_TYPE>
		static void GenerateBoxParticle(ParticleCorners const & corners, BitmapAtlas::BitmapTexcoords const & texcoords, VERTEX_TYPE * vertices)
		{
			VERTEX_TYPE bl;
			bl.position.x = corners.bottomleft_position.x;
			bl.position.y = corners.bottomleft_position.y;
			bl.texcoord.x = texcoords.bottomleft_texcoord.x;
			bl.texcoord.y = texcoords.bottomleft_texcoord.y;
			bl.texcoord.z = texcoords.bitmap_index;

			VERTEX_TYPE tr;
			tr.position.x = corners.topright_position.x;
			tr.position.y = corners.topright_position.y;
			tr.texcoord.x = texcoords.topright_texcoord.x;
			tr.texcoord.y = texcoords.topright_texcoord.y;
			tr.texcoord.z = texcoords.bitmap_index;

			VERTEX_TYPE tl;
			tl.position.x = corners.bottomleft_position.x;
			tl.position.y = corners.topright_position.y;
			tl.texcoord.x = texcoords.bottomleft_texcoord.x;
			tl.texcoord.y = texcoords.topright_texcoord.y;
			tl.texcoord.z = texcoords.bitmap_index;

			VERTEX_TYPE br;
			br.position.x = corners.topright_position.x;
			br.position.y = corners.bottomleft_position.y;
			br.texcoord.x = texcoords.topright_texcoord.x;
			br.texcoord.y = texcoords.bottomleft_texcoord.y;
			br.texcoord.z = texcoords.bitmap_index;

			vertices[0] = bl;
			vertices[1] = br;
			vertices[2] = tr;

			vertices[3] = bl;
			vertices[4] = tr;
			vertices[5] = tl;
		}
	};

}; // namespace chaos

