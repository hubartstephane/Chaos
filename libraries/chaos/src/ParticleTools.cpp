#include <chaos/ParticleTools.h>
#include <chaos/Hotpoint.h>
#include <chaos/MathTools.h>

namespace chaos
{

	box2 ParticleTools::ParticleCornersToBox(ParticleCorners const & corners)
	{
		return box2(std::make_pair(corners.bottomleft, corners.topright));	
	}
	
	ParticleCorners ParticleTools::BoxToParticleCorners(box2 const & box)
	{	
		std::pair<glm::vec2, glm::vec2> tmp = box.GetCorners();
	
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

	ParticleTexcoords ParticleTools::GetParticleTexcoords(BitmapAtlas::BitmapEntry const & entry, glm::vec2 const & atlas_size)
	{
		// XXX : for OpenGL, texture 0, 0 is top-level
		//       in image space (0, 0) is bottom-left
		//       => explains why (1. - Y)
		//

		ParticleTexcoords result;
		result.bottomleft.x  = MathTools::CastAndDiv<float>(entry.x, atlas_size.x);
		result.bottomleft.y  = 1.0f - MathTools::CastAndDiv<float>(entry.y + entry.height, atlas_size.y);
		result.topright.x    = MathTools::CastAndDiv<float>(entry.x + entry.width, atlas_size.x);
		result.topright.y    = 1.0f - MathTools::CastAndDiv<float>(entry.y, atlas_size.y);
		result.bitmap_index  = (float)entry.bitmap_index;
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


}; // namespace chaos

