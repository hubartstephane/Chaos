#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	WidgetPlacement ApplyAspectRatioConstraint(WidgetPlacement const& src, float aspect)
	{
		WidgetPlacement result = src;
		if (src.size.x != 0 && src.size.y != 0)
		{
			float viewport_aspect = float(src.size.x) / float(src.size.y);
			if (viewport_aspect != aspect)
			{
				if (viewport_aspect > aspect) // width too large
				{
					result.size.x = int(float(result.size.y) * aspect);
					result.position.x += (src.size.x - result.size.x) / 2;
				}
				else // height too large
				{
					result.size.y = int(float(result.size.x) / aspect);
					result.position.y += (src.size.y - result.size.y) / 2;
				}
			}
		}
		return result;
	}

}; // namespace chaos
