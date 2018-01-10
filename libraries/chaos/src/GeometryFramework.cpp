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
	

#if 0
		if (size.x <= 0.0f || size.y <= 0.0f)
			return;

		int x = 0;
		int y = 0;
		if (aspect > 0.0f)
		{
			float effective_aspect = ((float)size.x) / ((float)size.y);

			glm::ivec2 ori_size = size;
			if (effective_aspect > aspect) // width too large
			{				
				size.x = (int)(aspect * (float)size.y);			
				x      = (ori_size.x - size.x) / 2;				
			}
			else if (effective_aspect < aspect) // height too large
			{
				size.y = (int)(((float)size.x) / aspect);			
				y      = (ori_size.y - size.y) / 2;
			}	
		} 
#endif
		return src;
	}

}; // namespace chaos
