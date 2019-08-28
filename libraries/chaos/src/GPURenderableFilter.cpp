#include <chaos/GPURenderableFilter.h>

#include <chaos/GPURenderable.h>

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
