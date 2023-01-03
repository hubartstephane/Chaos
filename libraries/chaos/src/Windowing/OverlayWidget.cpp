#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void OverlayWidget::SetPlacement(aabox2 const& in_placement)
	{
		Widget::SetPlacement(in_placement);
		for (auto& child : child_widgets)
			if (child != nullptr)
				child->SetPlacement(placement); // XXX: use member instead of incoming parameter, because the member take into account the overlay layout
	}

	WidgetSurfaceRequirement OverlayWidget::GetSurfaceRequirement() const
	{
		WidgetSurfaceRequirement result;





		return result;
	}

}; // namespace chaos
