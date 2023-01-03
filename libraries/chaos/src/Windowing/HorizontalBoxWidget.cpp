#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	LinearComposerLayout HorizontalBoxWidget::GetComposerLayout() const
	{
		LinearComposerLayout result;
		result.orientation = Orientation::HORIZONTAL;
		return result;
	}

	WidgetSurfaceRequirement HorizontalBoxWidget::GetSurfaceRequirement() const
	{
		return ComputeChildrenSurfaceRequirement<true, false>();
	}

}; // namespace chaos
