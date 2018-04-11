#include <chaos/GeometryFramework.h>

namespace chaos
{
	box2 ShrinkBoxToAspect(box2 src, float aspect)
	{
		if (src.IsEmpty())
			return src;

		float effective_aspect = GetBoxAspect(src);
		if (effective_aspect == aspect)
			return src;

		box2 result = src;
		if (effective_aspect > aspect) // width too large
		{
			result.half_size.x = src.half_size.y * aspect;		
		}
		else if (effective_aspect < aspect) // height too large
		{
			result.half_size.y = src.half_size.x / aspect;
		}

		return result;
	}

}; // namespace chaos
