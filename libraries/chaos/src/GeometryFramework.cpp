#include <chaos/GeometryFramework.h>

namespace chaos
{
	box2 ShrinkBoxToAspect(box2 src, float aspect)
	{
		// any negative component
		if (IsGeometryEmpty(src))
			return src;
		// aspect already good
		float effective_aspect = GetBoxAspect(src);
		if (effective_aspect == aspect)
			return src;
		// make the update
		box2 result = src;
		if (effective_aspect > aspect) // width too large
			result.half_size.x = src.half_size.y * aspect;		
		else if (effective_aspect < aspect) // height too large
			result.half_size.y = src.half_size.x / aspect;

		return result;
	}

	box2 AlterBoxToAspect(box2 src, float aspect, bool prefer_width_update)
	{
		// any (non null) negative component
		if (IsGeometryEmpty(src)) 
			return src;
		// cannot have no size
		if (src.half_size.x == 0.0f && src.half_size.y == 0.0f) 
			return src;
		// width axis to update ?
		bool update_width = prefer_width_update;
		if (src.half_size.x == 0.0f)
			update_width = true;
		else if (src.half_size.y == 0.0f)
			update_width = false;
		// make the update
		box2 result = src;
		if (update_width)
			result.half_size.x = src.half_size.y * aspect;
		else
			result.half_size.y = src.half_size.x / aspect;

		return result;
	}

	glm::vec4 EncodeBoxToVector(chaos::box2 const & src)
	{
		glm::vec4 result;
		result.x = src.position.x;
		result.y = src.position.y;
		result.z = src.half_size.x;
		result.w = src.half_size.y;		
		return result;
	}

}; // namespace chaos
