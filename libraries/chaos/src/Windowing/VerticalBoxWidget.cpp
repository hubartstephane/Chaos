#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	LinearComposerLayout VerticalBoxWidget::GetComposerLayout() const
	{
		LinearComposerLayout result;
		result.orientation = Orientation::Vertical;
		return result;
	}

	WidgetSurfaceRequirement VerticalBoxWidget::GetSurfaceRequirement() const
	{
		return ComputeChildrenSurfaceRequirement<false, true>();
	}

}; // namespace chaos
