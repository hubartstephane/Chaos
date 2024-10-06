#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

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
