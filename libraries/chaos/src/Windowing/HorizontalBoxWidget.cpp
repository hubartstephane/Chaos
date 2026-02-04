#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	LinearComposerLayout HorizontalBoxWidget::GetComposerLayout() const
	{
		LinearComposerLayout result;
		result.orientation = Orientation::Horizontal;
		return result;
	}

	WidgetSurfaceRequirement HorizontalBoxWidget::GetSurfaceRequirement() const
	{
		return ComputeChildrenSurfaceRequirement<true, false>();
	}

}; // namespace chaos
