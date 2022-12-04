#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ViewportLayout::UpdatePlacementHierarchy()
	{
		if (viewport_composer != nullptr)
			viewport_composer->UpdatePlacementHierarchy();
	}

	void ViewportLayout::SetAllViewportPlacements(WidgetPlacement const& in_placement, std::vector<shared_ptr<Viewport>> & viewports)
	{





	}

}; // namespace chaos
