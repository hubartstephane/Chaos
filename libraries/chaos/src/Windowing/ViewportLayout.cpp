#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ViewportLayout::UpdateViewportPlacementHierarchy()
	{
		if (viewport_composer != nullptr)
			viewport_composer->UpdateViewportPlacementHierarchy();
	}

	ViewportPlacement ViewportLayout::ComputeViewportPlacement(Viewport * viewport, glm::ivec2 const& window_size, size_t viewport_index, size_t viewport_count) const
	{
		assert(0);
		return {};
	}

	void ViewportLayout::SetAllViewportPlacements(ViewportPlacement const& in_placement, std::vector<shared_ptr<Viewport>> & viewports)
	{





	}

}; // namespace chaos
