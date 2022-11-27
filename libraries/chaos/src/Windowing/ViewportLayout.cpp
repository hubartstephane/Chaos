#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ViewportLayout::UpdateWindowViewportPlacements()
	{
		//if (viewport_server != nullptr)
		//	viewport_server->ComputeViewportPlacements(viewport_server->GetWindowSize());
	}

	ViewportPlacement ViewportLayout::ComputeViewportPlacement(Viewport * viewport, glm::ivec2 const& window_size, size_t viewport_index, size_t viewport_count) const
	{
		assert(0);
		return {};
	}

}; // namespace chaos
