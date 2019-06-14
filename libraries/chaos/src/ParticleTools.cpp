#include <chaos/ParticleTools.h>
#include <chaos/Hotpoint.h>
#include <chaos/MathTools.h>
#include <chaos/ParticleLayer.h>
#include <chaos/GPURenderMaterial.h>
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/GPUResourceManager.h>

namespace chaos
{
	box2 ParticleTools::ParticleCornersToBox(ParticleCorners const & corners)
	{
		return box2(std::make_pair(corners.bottomleft, corners.topright));	
	}

	ParticleCorners ParticleTools::BoxToParticleCorners(box2 const & box)
	{	
		std::pair<glm::vec2, glm::vec2> tmp = GetBoxExtremums(box);

		ParticleCorners result;
		result.bottomleft = tmp.first;
		result.topright   = tmp.second;
		return result;
	}

	ParticleCorners ParticleTools::GetParticleCorners(glm::vec2 const & position, glm::vec2 const & size, int hotpoint_type)
	{
		ParticleCorners result;
		result.bottomleft = Hotpoint::ConvertToBottomLeft(position, size, hotpoint_type);
		result.topright   = result.bottomleft + size;
		return result;
	}

	ParticleTexcoords ParticleTools::GetParticleTexcoords(BitmapAtlas::BitmapLayout const & layout, glm::vec2 const & atlas_size)
	{
		// XXX : for OpenGL, texture Y = 0 is bottom
		//       for bitmap,         Y = 0 is top
		//       => explains why (1. - Y)

		ParticleTexcoords result;
		result.bottomleft.x  = MathTools::CastAndDiv<float>(layout.x, atlas_size.x);
		result.bottomleft.y  = 1.0f - MathTools::CastAndDiv<float>(layout.y + layout.height, atlas_size.y);
		result.topright.x    = MathTools::CastAndDiv<float>(layout.x + layout.width, atlas_size.x);
		result.topright.y    = 1.0f - MathTools::CastAndDiv<float>(layout.y, atlas_size.y);
		result.bitmap_index  = (float)layout.bitmap_index;
		return result;
	}

	ParticleTexcoords ParticleTools::MakeParticleTexcoordsAtlas(ParticleTexcoords texcoords, glm::ivec2 const & atlas_dimension, int skip_last, int image_id)
	{
		// tweak particle texcoords to have a sub image
		int image_count = (atlas_dimension.x * atlas_dimension.y) - skip_last;
		if (image_count > 0)
		{
			image_id = image_id % image_count;

			glm::vec2 atlas_coord = glm::vec2(
				(float)(image_id % atlas_dimension.x),
				(float)(image_id / atlas_dimension.x)
			);

			glm::vec2 atlas_size = (texcoords.topright - texcoords.bottomleft) / chaos::GLMTools::RecastVector<glm::vec2>(atlas_dimension);

			texcoords.bottomleft = texcoords.bottomleft + atlas_coord * atlas_size;
			texcoords.topright = texcoords.bottomleft + atlas_size;
		}	
		return texcoords;
	}

	ParticleTexcoords ParticleTools::MakeParticleTexcoordsAtlas(ParticleTexcoords texcoords, glm::ivec2 const & atlas_dimension, glm::ivec2 const & image_id)
	{
		// tweak particle texcoords to have a sub image
		int image_count = (atlas_dimension.x * atlas_dimension.y);
		if (image_count > 0)
		{
			glm::vec2 atlas_coord = glm::vec2(
				(float)(image_id.x % atlas_dimension.x),
				(float)(image_id.y % atlas_dimension.y)
			);

			glm::vec2 atlas_size = (texcoords.topright - texcoords.bottomleft) / chaos::GLMTools::RecastVector<glm::vec2>(atlas_dimension);

			texcoords.bottomleft = texcoords.bottomleft + atlas_coord * atlas_size;
			texcoords.topright = texcoords.bottomleft + atlas_size;
		}	
		return texcoords;
	}

}; // namespace chaos

