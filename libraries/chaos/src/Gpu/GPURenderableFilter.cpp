#include "chaos/Chaos.h"

namespace chaos
{
	// ========================================================
	// GPURenderableFilter implementation
	// ========================================================

	bool GPURenderableFilter::CanRender(GPURenderable const * renderable) const
	{
		return name_filter.CheckName(*renderable);
	}

}; // namespace chaos
